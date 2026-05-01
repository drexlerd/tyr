/*
 * Copyright (C) 2025-2026 Dominik Drexler
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "tyr/datalog/bottom_up.hpp"

#include "tyr/common/chrono.hpp"
#include "tyr/common/comparators.hpp"
#include "tyr/common/config.hpp"
#include "tyr/common/equal_to.hpp"
#include "tyr/common/formatter.hpp"
#include "tyr/common/hash.hpp"
#include "tyr/common/types.hpp"
#include "tyr/common/vector.hpp"
#include "tyr/datalog/applicability.hpp"
#include "tyr/datalog/assignment_sets.hpp"
#include "tyr/datalog/consistency_graph.hpp"
#include "tyr/datalog/declarations.hpp"
#include "tyr/datalog/delta_kpkc.hpp"
#include "tyr/datalog/fact_sets.hpp"
#include "tyr/datalog/formatter.hpp"
#include "tyr/datalog/policies/aggregation.hpp"
#include "tyr/datalog/policies/annotation.hpp"
#include "tyr/datalog/policies/numeric_support.hpp"
#include "tyr/datalog/policies/termination.hpp"
#include "tyr/datalog/rule_scheduler.hpp"
#include "tyr/datalog/workspaces/facts.hpp"
#include "tyr/datalog/workspaces/program.hpp"
#include "tyr/datalog/workspaces/rule.hpp"
#include "tyr/formalism/datalog/conjunctive_condition_view.hpp"
#include "tyr/formalism/datalog/declarations.hpp"
#include "tyr/formalism/datalog/formatter.hpp"
#include "tyr/formalism/datalog/grounder.hpp"
#include "tyr/formalism/datalog/merge.hpp"
#include "tyr/formalism/datalog/repository.hpp"
#include "tyr/formalism/datalog/views.hpp"

#include <algorithm>
#include <assert.h>
#include <cmath>
#include <fmt/ostream.h>
#include <iostream>
#include <memory>
#include <oneapi/tbb/parallel_for_each.h>
#include <oneapi/tbb/parallel_invoke.h>
#include <stdexcept>
#include <tuple>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

namespace f = tyr::formalism;
namespace fd = tyr::formalism::datalog;

namespace tyr::datalog
{

static void create_nullary_binding(IndexList<f::Object>& binding) { binding.clear(); }

static void create_general_binding(std::span<const kpkc::Vertex> clique, const StaticConsistencyGraph& consistency_graph, IndexList<f::Object>& binding)
{
    const auto k = clique.size();

    binding.resize(k);

    for (uint_t p = 0; p < k; ++p)
    {
        const auto& vertex = consistency_graph.get_vertex(clique[p].index);
        assert(uint_t(vertex.get_parameter_index()) == p);

        binding[p] = vertex.get_object_index();
    }
}

template<AndAnnotationPolicyConcept AndAP>
static void insert_propositional_update(fd::AtomView<f::FluentTag> head,
                                        fd::RuleView rule,
                                        fd::ConjunctiveConditionView witness_condition,
                                        const NumericSupportSelector& numeric_support_selector,
                                        NumericSupportSelectorWorkspace& numeric_support_selector_workspace,
                                        Cost current_cost,
                                        const SelectedPredicateAnnotations& program_and_annot,
                                        const SelectedFunctionAnnotations& program_numeric_and_annot,
                                        const AndAP& and_ap,
                                        fd::GrounderContext& solve_context,
                                        fd::GrounderContext& iteration_context,
                                        RuleHeadIteration& head_iteration,
                                        SelectedPredicateAnnotations& and_annot)
{
    const auto program_head = fd::ground_binding(head, iteration_context).first;
    const auto worker_head = fd::ground_binding(head, solve_context).first;

    std::get<PredicateHeadIteration>(head_iteration).rows.insert(worker_head.get_index().row);

    and_ap.update_annotation(program_head,
                             worker_head,
                             current_cost,
                             rule,
                             witness_condition,
                             numeric_support_selector,
                             numeric_support_selector_workspace,
                             program_and_annot,
                             program_numeric_and_annot,
                             and_annot,
                             solve_context,
                             iteration_context);
}

template<AndAnnotationPolicyConcept AndAP>
static void insert_numeric_update(fd::NumericEffectOperatorView<f::FluentTag> head,
                                  fd::RuleView rule,
                                  fd::ConjunctiveConditionView witness_condition,
                                  const FactSets& fact_sets,
                                  const NumericSupportSelector& numeric_support_selector,
                                  NumericSupportSelectorWorkspace& numeric_support_selector_workspace,
                                  Cost current_cost,
                                  const SelectedPredicateAnnotations& program_and_annot,
                                  const SelectedFunctionAnnotations& program_numeric_and_annot,
                                  const AndAP& and_ap,
                                  fd::GrounderContext& solve_context,
                                  fd::GrounderContext& iteration_context,
                                  RuleHeadIteration& head_iteration,
                                  SelectedFunctionAnnotations& numeric_and_annot)
{
    const auto interval = is_valid_binding(head, fact_sets, iteration_context);
    if (empty(interval))
        return;

    visit(
        [&](auto&& effect)
        {
            const auto program_head = fd::ground(effect.get_fterm(), iteration_context).first.get_row();
            const auto worker_head = fd::ground(effect.get_fterm(), solve_context).first;

            and_ap.update_annotation(program_head,
                                     worker_head.get_row(),
                                     interval,
                                     current_cost,
                                     rule,
                                     witness_condition,
                                     numeric_support_selector,
                                     numeric_support_selector_workspace,
                                     program_and_annot,
                                     program_numeric_and_annot,
                                     numeric_and_annot,
                                     solve_context,
                                     iteration_context);

            std::get<FunctionHeadIteration>(head_iteration).updates.emplace(worker_head.get_row().get_index().row, interval, current_cost + rule.get_cost());
        },
        head.get_variant());
}

template<OrAnnotationPolicyConcept OrAP, AndAnnotationPolicyConcept AndAP, TerminationPolicyConcept TP>
void generate_nullary_case(RuleExecutionContext<OrAP, AndAP, TP>& rctx)
{
    auto wrctx = rctx.get_rule_worker_execution_context();

    const auto& in = wrctx.in();
    auto& out = wrctx.out();
    const auto& numeric_support_selector = in.numeric_support_selector();
    ++out.statistics().num_executions;
    ++out.statistics().num_generated_rules;

    create_nullary_binding(out.ground_context_solve().binding);

    // Note: we never go through the consistency graph, and hence, have to check validity on the entire rule body.
    if (is_applicable(in.cws_rule().get_nullary_condition(), in.fact_sets())
        && is_valid_binding(in.cws_rule().get_rule().get_body(), in.fact_sets(), out.ground_context_iteration()))
    {
        visit(
            [&](auto&& head)
            {
                using Head = std::decay_t<decltype(head)>;

                if constexpr (std::is_same_v<Head, fd::AtomView<f::FluentTag>>)
                {
                    insert_propositional_update(head,
                                                in.cws_rule().get_rule(),
                                                in.cws_rule().get_witness_rule().get_body(),
                                                numeric_support_selector,
                                                out.numeric_support_selector_workspace(),
                                                in.cost_buckets().current_cost(),
                                                in.and_annot(),
                                                in.numeric_and_annot(),
                                                in.and_ap(),
                                                out.ground_context_solve(),
                                                out.ground_context_iteration(),
                                                out.head(),
                                                out.and_annot());
                }
                else
                {
                    assert(ensure_applicability(in.cws_rule().get_rule(), out.ground_context_iteration(), in.fact_sets()));

                    insert_numeric_update(head,
                                          in.cws_rule().get_rule(),
                                          in.cws_rule().get_witness_rule().get_body(),
                                          in.fact_sets(),
                                          numeric_support_selector,
                                          out.numeric_support_selector_workspace(),
                                          in.cost_buckets().current_cost(),
                                          in.and_annot(),
                                          in.numeric_and_annot(),
                                          in.and_ap(),
                                          out.ground_context_solve(),
                                          out.ground_context_iteration(),
                                          out.head(),
                                          out.numeric_and_annot());
                }
            },
            in.cws_rule().get_rule().get_head());
    }
}

[[maybe_unused]] static bool ensure_applicability(fd::RuleView rule, fd::GrounderContext& context, const FactSets& fact_sets)
{
    const auto ground_rule = ground(rule, context).first;

    const auto applicable = is_applicable(ground_rule, fact_sets);

    if (!applicable)
    {
        std::cout << "Delta-KPKC generated false positive." << std::endl;
        fmt::print(std::cout, "{}\n", rule);
        fmt::print(std::cout, "{}\n", ground_rule);
    }

    return applicable;
}

[[maybe_unused]] static bool ensure_novel_binding(const IndexList<f::Object>& binding, UnorderedSet<IndexList<f::Object>>& set)
{
    const auto inserted = set.insert(binding).second;

    if (!inserted)
    {
        std::cout << "Delta-KPKC generated duplicate binding." << std::endl;
    }

    return inserted;
}

static bool is_statically_applicable(fd::GroundConjunctiveConditionView nullary_condition,
                                     fd::ConjunctiveConditionView conflicting_condition,
                                     const FactSets& fact_sets,
                                     fd::GrounderContext& context)
{
    return is_applicable(nullary_condition.get_literals<f::StaticTag>(), fact_sets)
           && is_valid_binding(conflicting_condition.get_literals<f::StaticTag>(), fact_sets, context);
}

static bool is_dynamically_applicable(fd::GroundConjunctiveConditionView nullary_condition,
                                      fd::ConjunctiveConditionView conflicting_condition,
                                      const FactSets& fact_sets,
                                      fd::GrounderContext& context)
{
    return is_applicable(nullary_condition, fact_sets) && is_valid_binding(conflicting_condition, fact_sets, context);
}

template<OrAnnotationPolicyConcept OrAP, AndAnnotationPolicyConcept AndAP, TerminationPolicyConcept TP>
void process_clique(RuleWorkerExecutionContext<OrAP, AndAP, TP>& wrctx,
                    const NumericSupportSelector& numeric_support_selector,
                    std::span<const kpkc::Vertex> clique,
                    bool require_novel_binding)
{
    const auto& in = wrctx.in();
    auto& out = wrctx.out();

    create_general_binding(clique, in.cws_rule().get_static_consistency_graph(), out.ground_context_solve().binding);

    assert(!require_novel_binding || ensure_novel_binding(out.ground_context_solve().binding, out.seen_bindings_dbg()));

    ++out.statistics().num_generated_rules;

    const auto nullary_condition = in.cws_rule().get_nullary_condition();
    const auto conflicting_condition = in.cws_rule().get_conflicting_overapproximation_rule().get_body();

    if (!is_statically_applicable(nullary_condition, conflicting_condition, in.fact_sets(), out.ground_context_iteration()))
        return;

    const auto dynamically_applicable = is_dynamically_applicable(nullary_condition, conflicting_condition, in.fact_sets(), out.ground_context_iteration());

    visit(
        [&](auto&& head)
        {
            using Head = std::decay_t<decltype(head)>;

            if constexpr (std::is_same_v<Head, fd::AtomView<f::FluentTag>>)
            {
                const auto program_head = fd::ground_binding(head, out.ground_context_iteration()).first;
                if (in.fact_sets().template get<f::FluentTag>().predicate.contains(program_head))
                    return;  ///< optimal cost proven

                // IMPORTANT: A binding can fail the nullary part (e.g., arm-empty) even though the clique already exists.
                // Later, nullary may become true without any new kPKC edges/vertices, so delta-kPKC will NOT re-enumerate this binding.
                // Therefore we must store it as pending (keyed by binding) and recheck in the next fact envelope.
                if (!dynamically_applicable)
                {
                    ++out.statistics().num_pending_rules;

                    const auto rule_binding = fd::ground_binding(in.cws_rule().get_conflicting_overapproximation_rule(), out.ground_context_solve()).first;

                    out.pending_rule_bindings().emplace(rule_binding);
                    return;
                }

                assert(ensure_applicability(in.cws_rule().get_rule(), out.ground_context_iteration(), in.fact_sets()));

                insert_propositional_update(head,
                                            in.cws_rule().get_rule(),
                                            in.cws_rule().get_witness_rule().get_body(),
                                            numeric_support_selector,
                                            out.numeric_support_selector_workspace(),
                                            in.cost_buckets().current_cost(),
                                            in.and_annot(),
                                            in.numeric_and_annot(),
                                            in.and_ap(),
                                            out.ground_context_solve(),
                                            out.ground_context_iteration(),
                                            out.head(),
                                            out.and_annot());
            }
            else
            {
                if (!dynamically_applicable)
                    return;

                assert(ensure_applicability(in.cws_rule().get_rule(), out.ground_context_iteration(), in.fact_sets()));

                insert_numeric_update(head,
                                      in.cws_rule().get_rule(),
                                      in.cws_rule().get_witness_rule().get_body(),
                                      in.fact_sets(),
                                      numeric_support_selector,
                                      out.numeric_support_selector_workspace(),
                                      in.cost_buckets().current_cost(),
                                      in.and_annot(),
                                      in.numeric_and_annot(),
                                      in.and_ap(),
                                      out.ground_context_solve(),
                                      out.ground_context_iteration(),
                                      out.head(),
                                      out.numeric_and_annot());
            }
        },
        in.cws_rule().get_rule().get_head());
}

template<OrAnnotationPolicyConcept OrAP, AndAnnotationPolicyConcept AndAP, TerminationPolicyConcept TP>
void generate_general_case(RuleExecutionContext<OrAP, AndAP, TP>& rctx)
{
    auto& rule_out = rctx.out();
    const auto& kpkc_algorithm = rule_out.kpkc();

    auto for_each_relevant_clique = [&](auto&& head, auto&& callback, auto& workspace)
    {
        using Head = std::decay_t<decltype(head)>;

        // Note: rules with numeric effects are non-idempotent, so we must consider all k-cliques, not just new ones.
        if constexpr (std::is_same_v<Head, fd::NumericEffectOperatorView<f::FluentTag>>)
            kpkc_algorithm.for_each_k_clique(std::forward<decltype(callback)>(callback), workspace);
        else
            kpkc_algorithm.for_each_new_k_clique(std::forward<decltype(callback)>(callback), workspace);
    };

#ifdef TYR_ENABLE_INNER_PARALLELISM

    const auto& in = rctx.in();

    constexpr size_t PAR_THRESHOLD = 1024;

    const auto arena_conc = static_cast<size_t>(oneapi::tbb::this_task_arena::max_concurrency());

    const auto& delta_edges = kpkc_algorithm.get_delta_edges();

    // Decide whether we want to trigger the parallel loop.
    // Seeding from edges in first iteration is wasteful.
    // We could seed from all vertices in a partition instead.
    const bool do_parallel_inner =
        false && kpkc_algorithm.get_iteration() > 1 && (in.cws_rule().get_rule().get_arity() > 2) && (delta_edges.size() >= PAR_THRESHOLD) && (arena_conc >= 2);

    if (do_parallel_inner)
    {
        // A very basic version which works well on rovers-large-simple.

        auto run_stripe = [&](size_t tid, size_t num_stripes)
        {
            auto wrctx = rctx.get_rule_worker_execution_context();
            const auto& numeric_support_selector = wrctx.in().numeric_support_selector();
            auto& out = wrctx.out();
            auto& ws = out.kpkc_workspace();
            ++out.statistics().num_executions;

            for (size_t e = tid; e < delta_edges.size(); e += num_stripes)
            {
                const auto& edge = delta_edges[e];
                if (!kpkc_algorithm.seed_from_anchor(edge, ws))
                    continue;

                kpkc_algorithm.template complete_from_seed<kpkc::Edge>([&](auto&& clique) { process_clique(wrctx, numeric_support_selector, clique, true); },
                                                                       0,
                                                                       ws);
            }
        };

        oneapi::tbb::parallel_invoke([&] { run_stripe(0, 2); }, [&] { run_stripe(1, 2); });
    }
    else
    {
        auto wrctx = rctx.get_rule_worker_execution_context();
        const auto& numeric_support_selector = wrctx.in().numeric_support_selector();
        auto& out = wrctx.out();
        auto& kpkc_workspace = out.kpkc_workspace();
        ++out.statistics().num_executions;

        visit(
            [&](auto&& head)
            {
                using Head = std::decay_t<decltype(head)>;
                for_each_relevant_clique(
                    head,
                    [&](auto&& clique)
                    { process_clique(wrctx, numeric_support_selector, clique, !std::is_same_v<Head, fd::NumericEffectOperatorView<f::FluentTag>>); },
                    kpkc_workspace);
            },
            rctx.in().cws_rule().get_rule().get_head());
    }

#else

    auto wrctx = rctx.get_rule_worker_execution_context();
    const auto& numeric_support_selector = wrctx.in().numeric_support_selector();
    auto& out = wrctx.out();
    auto& kpkc_workspace = out.kpkc_workspace();
    ++out.statistics().num_executions;

    visit(
        [&](auto&& head)
        {
            using Head = std::decay_t<decltype(head)>;
            for_each_relevant_clique(
                head,
                [&](auto&& clique)
                { process_clique(wrctx, numeric_support_selector, clique, !std::is_same_v<Head, fd::NumericEffectOperatorView<f::FluentTag>>); },
                kpkc_workspace);
        },
        rctx.in().cws_rule().get_rule().get_head());

#endif
}

template<OrAnnotationPolicyConcept OrAP, AndAnnotationPolicyConcept AndAP, TerminationPolicyConcept TP>
void generate(RuleExecutionContext<OrAP, AndAP, TP>& rctx)
{
    const auto arity = rctx.in().cws_rule().get_rule().get_arity();

    if (arity == 0)
        generate_nullary_case(rctx);
    else
        generate_general_case(rctx);
}

template<OrAnnotationPolicyConcept OrAP, AndAnnotationPolicyConcept AndAP, TerminationPolicyConcept TP>
void process_pending_rule_bindings(RuleExecutionContext<OrAP, AndAP, TP>& rctx)
{
    for (auto& worker : rctx.out().workers())
    {
        auto wrctx = RuleWorkerExecutionContext(rctx, worker);

        const auto& in = wrctx.in();
        auto& out = wrctx.out();
        const auto& numeric_support_selector = in.numeric_support_selector();

        for (auto it = out.pending_rule_bindings().begin(); it != out.pending_rule_bindings().end();)
        {
            out.ground_context_solve().binding.clear();
            for (const auto object : it->get_objects())
                out.ground_context_solve().binding.push_back(object.get_index());

            assert(out.ground_context_solve().binding == out.ground_context_iteration().binding);

            visit(
                [&](auto&& head)
                {
                    using Head = std::decay_t<decltype(head)>;

                    if constexpr (std::is_same_v<Head, fd::AtomView<f::FluentTag>>)
                    {
                        const auto program_head = fd::ground_binding(head, out.ground_context_iteration()).first;

                        if (in.fact_sets().template get<f::FluentTag>().predicate.contains(program_head))  ///< optimal cost proven
                        {
                            it = out.pending_rule_bindings().erase(it);
                        }
                        else if (is_dynamically_applicable(in.cws_rule().get_nullary_condition(),
                                                           in.cws_rule().get_conflicting_overapproximation_rule().get_body(),
                                                           in.fact_sets(),
                                                           out.ground_context_iteration()))
                        {
                            assert(ensure_applicability(in.cws_rule().get_rule(), out.ground_context_iteration(), in.fact_sets()));

                            insert_propositional_update(head,
                                                        in.cws_rule().get_rule(),
                                                        in.cws_rule().get_witness_rule().get_body(),
                                                        numeric_support_selector,
                                                        out.numeric_support_selector_workspace(),
                                                        in.cost_buckets().current_cost(),
                                                        in.and_annot(),
                                                        in.numeric_and_annot(),
                                                        in.and_ap(),
                                                        out.ground_context_solve(),
                                                        out.ground_context_iteration(),
                                                        out.head(),
                                                        out.and_annot());

                            it = out.pending_rule_bindings().erase(it);
                        }
                        else
                        {
                            ++it;
                        }
                    }
                    else
                    {
                        throw std::logic_error("Numeric-head rules should not be stored as pending.");
                    }
                },
                in.cws_rule().get_rule().get_head());
        }
    }
}

template<OrAnnotationPolicyConcept OrAP, AndAnnotationPolicyConcept AndAP, TerminationPolicyConcept TP>
void solve_bottom_up_for_stratum(StratumExecutionContext<OrAP, AndAP, TP>& ctx)
{
    auto& out = ctx.out();
    auto& scheduler = out.scheduler();
    auto& program_out = out.program();
    auto& facts = program_out.facts();
    auto& cost_buckets = program_out.cost_buckets();
    auto& tp = program_out.tp();

    scheduler.activate_all();

    cost_buckets.clear();

    while (true)
    {
        // Check whether min cost for goal was proven.
        if (tp.check(FactSets { ctx.in().program().facts().fact_sets, facts.fact_sets }))
        {
            return;
        }

        scheduler.on_start_iteration();

        const auto& active_rules = scheduler.get_active_rules();

        /**
         * Parallel process pending applicability checks and generate ground witnesses.
         */

        {
            const auto program_stopwatch = StopwatchScope(program_out.statistics().parallel_time);

            oneapi::tbb::parallel_for_each(active_rules.begin(),
                                           active_rules.end(),
                                           [&](auto&& rule_index)
                                           {
                                               // std::cout << make_view(rule_index, ws.repository) << std::endl;

                                               auto rctx = ctx.get_rule_execution_context(rule_index);
                                               auto& rule_out = rctx.out();

                                               const auto total_time = StopwatchScope(rule_out.statistics().total_time);
                                               ++rule_out.statistics().num_executions;

                                               rctx.clear_iteration();  ///< Clear iteration before process_pending_rule_bindings/generate

                                               {
                                                   const auto initialize_time = StopwatchScope(rule_out.statistics().initialize_time);

                                                   rctx.initialize();  ///< Initialize before process_pending_rule_bindings/generate
                                               }

                                               {
                                                   const auto process_pending_time = StopwatchScope(rule_out.statistics().process_pending_time);

                                                   process_pending_rule_bindings(rctx);
                                               }

                                               {
                                                   const auto process_generate_time = StopwatchScope(rule_out.statistics().process_generate_time);

                                                   generate(rctx);
                                               }
                                           });
        }

        // Clear current bucket to avoid duplicate handling
        cost_buckets.clear_current();

        /**
         * Sequential merge results from workers into program
         */

        {
            for (const auto rule_index : active_rules)
            {
                const auto i = uint_t(rule_index);
                auto merge_context = fd::MergeContext { program_out.datalog_builder(), program_out.workspace_repository() };
                const auto& ws_rule = program_out.rules()[i];

                for (const auto& worker : ws_rule->worker)
                {
                    std::visit(
                        [&](const auto& head_iteration)
                        {
                            using HeadIteration = std::decay_t<decltype(head_iteration)>;

                            if constexpr (std::is_same_v<HeadIteration, PredicateHeadIteration>)
                            {
                                for (const auto worker_head_index : head_iteration.rows)
                                {
                                    const auto worker_head =
                                        make_view(Index<f::RelationBinding<f::Predicate<f::FluentTag>>> { head_iteration.relation, worker_head_index },
                                                  worker.solve.program_overlay_repository);

                                    // Merge head from delta into the program
                                    const auto program_head = fd::merge_d2d(worker_head, merge_context).first;

                                    // Update annotation
                                    const auto cost_update =
                                        program_out.or_ap().update_annotation(program_head, worker_head, worker.iteration.and_annot, program_out.and_annot());

                                    cost_buckets.update(cost_update, program_head);
                                }
                            }
                            else
                            {
                                for (const auto& update : head_iteration.updates)
                                {
                                    const auto worker_head =
                                        make_view(Index<f::RelationBinding<f::Function<f::FluentTag>>> { head_iteration.relation, update.row },
                                                  worker.solve.program_overlay_repository);

                                    const auto program_head = fd::merge_d2d(worker_head, merge_context).first;
                                    const auto* worker_annotation = worker.iteration.numeric_and_annot.find(worker_head, update.interval);
                                    if (worker_annotation)
                                        program_out.numeric_and_annot().insert(program_head, update.interval, *worker_annotation);

                                    cost_buckets.insert(update.cost, program_head, update.interval);
                                }
                            }
                        },
                        worker.iteration.head);
                }
            }

            if (!cost_buckets.advance_to_next_nonempty())
                return;  // Terminate if no-nonempty bucket was found.

            // Insert next bucket heads into fact and assignment sets + trigger scheduler.
            for (const auto head : cost_buckets.get_current_bucket())
            {
                // Update fact sets
                const auto changed = facts.fact_sets.predicate.insert(head);
                if (changed)
                {
                    // Notify scheduler
                    scheduler.on_generate(head.get_index().relation);

                    // Update assignment sets
                    facts.assignment_sets.predicate.insert(head);
                }
            }

            for (const auto& [head, interval] : cost_buckets.get_current_function_bucket())
            {
                // Update fact sets
                const auto changed = facts.fact_sets.function.insert(head, interval);
                if (changed)
                {
                    // Notify scheduler
                    scheduler.on_generate(head.get_index().relation);

                    // Update assignment sets
                    facts.assignment_sets.function.insert(head, interval);
                }
            }

            program_out.rebuild_numeric_support_selector(ctx.in().program().facts().fact_sets);
        }

        scheduler.on_finish_iteration();
    }
}

template<OrAnnotationPolicyConcept OrAP, AndAnnotationPolicyConcept AndAP, TerminationPolicyConcept TP>
void solve_bottom_up(ProgramExecutionContext<OrAP, AndAP, TP>& ctx)
{
    auto& out = ctx.out();
    const auto program_stopwatch = StopwatchScope(out.statistics().total_time);
    ++out.statistics().num_executions;

    for (auto stratum_ctx : ctx.get_stratum_execution_contexts())
    {
        solve_bottom_up_for_stratum(stratum_ctx);
    }
}

template void solve_bottom_up(ProgramExecutionContext<NoOrAnnotationPolicy, NoAndAnnotationPolicy, NoTerminationPolicy>& ctx);
template void solve_bottom_up(ProgramExecutionContext<OrAnnotationPolicy, AndAnnotationPolicy<SumAggregation>, NoTerminationPolicy>& ctx);
template void solve_bottom_up(ProgramExecutionContext<OrAnnotationPolicy, AndAnnotationPolicy<SumAggregation>, TerminationPolicy<SumAggregation>>& ctx);
template void solve_bottom_up(ProgramExecutionContext<OrAnnotationPolicy, AndAnnotationPolicy<MaxAggregation>, NoTerminationPolicy>& ctx);
template void solve_bottom_up(ProgramExecutionContext<OrAnnotationPolicy, AndAnnotationPolicy<MaxAggregation>, TerminationPolicy<MaxAggregation>>& ctx);
}
