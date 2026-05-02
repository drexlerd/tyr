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

#include "tyr/planning/lifted_task/successor_generator.hpp"

#include "../metric.hpp"
#include "tyr/datalog/bottom_up.hpp"
#include "tyr/datalog/contexts/program.hpp"
#include "tyr/datalog/formatter.hpp"
#include "tyr/formalism/planning/grounder.hpp"
#include "tyr/formalism/planning/merge_datalog.hpp"
#include "tyr/planning/applicability_lifted.hpp"
#include "tyr/planning/declarations.hpp"
#include "tyr/planning/ground_task/match_tree/match_tree.hpp"
#include "tyr/planning/lifted_task.hpp"
#include "tyr/planning/lifted_task/axiom_evaluator.hpp"
#include "tyr/planning/lifted_task/node.hpp"
#include "tyr/planning/lifted_task/state_repository.hpp"
#include "tyr/planning/lifted_task/state_view.hpp"
#include "tyr/planning/lifted_task/unpacked_state.hpp"
#include "tyr/planning/programs/action.hpp"
#include "tyr/planning/successor_generator.hpp"
#include "tyr/planning/task_utils.hpp"

#include <fmt/ostream.h>

namespace d = tyr::datalog;
namespace f = tyr::formalism;
namespace fp = tyr::formalism::planning;
namespace df = tyr::formalism::datalog;

namespace tyr::planning
{
namespace
{
template<typename Callback>
void for_each_action_binding(const d::ProgramWorkspace<d::NoOrAnnotationPolicy, d::NoAndAnnotationPolicy, d::NoTerminationPolicy>& workspace,
                             const ApplicableActionProgram& program,
                             IndexList<f::Object>& binding_scratch,
                             Callback&& callback)
{
    const auto& mapping = program.get_predicate_to_action_mapping();

    for (const auto& set : workspace.facts.fact_sets.predicate.get_sets())
    {
        for (const auto& binding : set.get_bindings())
        {
            if (const auto it = mapping.find(binding.get_relation()); it != mapping.end())
            {
                binding_scratch.clear();

                for (const auto object : binding.get_objects())
                    binding_scratch.push_back(object.get_index());

                callback(it->second, binding_scratch);
            }
        }
    }
}
}

SuccessorGenerator<LiftedTag>::SuccessorGenerator(TaskPtr<LiftedTag> task, ExecutionContextPtr execution_context) :
    m_task(std::move(task)),
    m_execution_context(std::move(execution_context)),
    m_workspace(m_task->get_action_program().get_program_context(),
                m_task->get_action_program().get_const_program_workspace(),
                d::NoOrAnnotationPolicy(),
                d::NoAndAnnotationPolicy(),
                d::NoTerminationPolicy()),
    m_state_repository(std::make_shared<StateRepository<LiftedTag>>(m_task, m_execution_context)),
    m_executor()
{
}

SuccessorGeneratorPtr<LiftedTag> SuccessorGenerator<LiftedTag>::create(TaskPtr<LiftedTag> task, ExecutionContextPtr execution_context)
{
    return std::make_shared<SuccessorGenerator<LiftedTag>>(std::move(task), std::move(execution_context));
}

Node<LiftedTag> SuccessorGenerator<LiftedTag>::get_initial_node()
{
    auto initial_state = m_state_repository->get_initial_state();
    const auto state_context = StateContext<LiftedTag>(*m_task, initial_state.get_unpacked_state(), 0);
    const auto state_metric = evaluate_metric(m_task->get_task().get_metric(), m_task->get_task().get_auxiliary_fterm_value(), state_context);
    return Node<LiftedTag>(std::move(initial_state), state_metric);
}

std::vector<LabeledNode<LiftedTag>> SuccessorGenerator<LiftedTag>::get_labeled_successor_nodes(const Node<LiftedTag>& node)
{
    auto result = std::vector<LabeledNode<LiftedTag>> {};
    get_labeled_successor_nodes(node, result);
    return result;
}

void SuccessorGenerator<LiftedTag>::get_labeled_successor_nodes(const Node<LiftedTag>& node, std::vector<LabeledNode<LiftedTag>>& out_nodes)
{
    out_nodes.clear();

    compute_action_facts(node);

    auto grounder_context = fp::GrounderContext { m_workspace.planning_builder, *m_task->get_repository(), m_workspace.binding };
    const auto state_context = StateContext<LiftedTag>(*m_task, node.get_state().get_unpacked_state(), node.get_metric());

    for_each_action_binding(m_workspace,
                            m_task->get_action_program(),
                            m_workspace.binding,
                            [&](const auto& action, const auto&)
                            {
                                const auto ground_action = fp::ground(action,
                                                                      grounder_context,
                                                                      m_task->get_grounder_cache(),
                                                                      m_task->get_formalism_task().get_variable_domains().action_domains.at(action.get_index()),
                                                                      m_cartesian_workspace,
                                                                      *m_task->get_fdr_context())
                                                               .first;

                                if (m_executor.is_applicable(ground_action, state_context))
                                    out_nodes.emplace_back(ground_action, m_executor.apply_action(state_context, ground_action, *m_state_repository));
                            });
}

Node<LiftedTag> SuccessorGenerator<LiftedTag>::get_successor_node(const Node<LiftedTag>& node, fp::GroundActionView action)
{
    const auto& state = node.get_state();
    const auto state_context = StateContext<LiftedTag>(*m_task, state.get_unpacked_state(), node.get_metric());
    return m_executor.apply_action(state_context, action, *m_state_repository);
}

// Action binding API (interning)
Node<LiftedTag> SuccessorGenerator<LiftedTag>::get_successor_node(const Node<LiftedTag>& node, formalism::planning::ActionBindingView binding)
{
    m_scratch_action_binding.relation = binding.get_relation().get_index();
    m_scratch_action_binding.objects.clear();
    for (const auto object : binding.get_data())
        m_scratch_action_binding.objects.push_back(object);

    return get_successor_node(node, m_scratch_action_binding);
}

std::vector<formalism::planning::ActionBindingView> SuccessorGenerator<LiftedTag>::get_applicable_action_bindings(const Node<LiftedTag>& node)
{
    auto result = std::vector<formalism::planning::ActionBindingView> {};
    get_applicable_action_bindings(node, result);
    return result;
}

void SuccessorGenerator<LiftedTag>::get_applicable_action_bindings(const Node<LiftedTag>& node,
                                                                   std::vector<formalism::planning::ActionBindingView>& out_bindings)
{
    out_bindings.clear();

    compute_action_facts(node);

    const auto state_context = StateContext<LiftedTag>(*m_task, node.get_state().get_unpacked_state(), node.get_metric());
    auto grounder_context = fp::GrounderContext { m_workspace.planning_builder, *m_task->get_repository(), m_workspace.binding };

    for_each_action_binding(m_workspace,
                            m_task->get_action_program(),
                            m_workspace.binding,
                            [&](const auto& action, const auto& binding)
                            {
                                m_scratch_action_binding.relation = action.get_index();
                                m_scratch_action_binding.objects = binding;

                                if (m_executor.is_applicable(action, state_context, grounder_context, *m_task->get_fdr_context()))
                                    out_bindings.emplace_back(m_task->get_repository()->get_or_create(m_scratch_action_binding).first);
                            });
}

// Action binding API (no interning)
Node<LiftedTag> SuccessorGenerator<LiftedTag>::get_successor_node(const Node<LiftedTag>& node,
                                                                  const Data<formalism::RelationBinding<formalism::planning::Action>>& binding)
{
    m_workspace.binding.clear();
    for (const auto object : binding.objects)
        m_workspace.binding.push_back(object);

    auto grounder_context = fp::GrounderContext { m_workspace.planning_builder, *m_task->get_repository(), m_workspace.binding };
    const auto state_context = StateContext<LiftedTag>(*m_task, node.get_state().get_unpacked_state(), node.get_metric());
    const auto action = make_view(binding.relation, *m_task->get_repository());

    return m_executor.apply_action(state_context, action, grounder_context, *m_task->get_fdr_context(), *m_state_repository);
}

void SuccessorGenerator<LiftedTag>::for_each_applicable_action_binding_impl(const Node<LiftedTag>& node,
                                                                            Data<formalism::RelationBinding<formalism::planning::Action>>& scratch_binding,
                                                                            ActionBindingCallback callback,
                                                                            void* callback_data)
{
    compute_action_facts(node);

    const auto state_context = StateContext<LiftedTag>(*m_task, node.get_state().get_unpacked_state(), node.get_metric());
    auto grounder_context = fp::GrounderContext { m_workspace.planning_builder, *m_task->get_repository(), scratch_binding.objects };
    const auto& mapping = m_task->get_action_program().get_predicate_to_action_mapping();

    for (const auto& set : m_workspace.facts.fact_sets.predicate.get_sets())
    {
        for (const auto& binding : set.get_bindings())
        {
            const auto it = mapping.find(binding.get_relation());
            if (it == mapping.end())
                continue;

            scratch_binding.relation = it->second.get_index();
            scratch_binding.objects.clear();
            for (const auto object : binding.get_objects())
                scratch_binding.objects.push_back(object.get_index());

            if (m_executor.is_applicable(it->second, state_context, grounder_context, *m_task->get_fdr_context()))
                callback(scratch_binding, callback_data);
        }
    }
}

// Lookup
Node<LiftedTag> SuccessorGenerator<LiftedTag>::get_node(Index<State<LiftedTag>> state_index)
{
    auto state = m_state_repository->get_registered_state(state_index);
    const auto state_context = StateContext<LiftedTag>(*m_task, state.get_unpacked_state(), 0);
    const auto state_metric = evaluate_metric(m_task->get_task().get_metric(), m_task->get_task().get_auxiliary_fterm_value(), state_context);
    return Node<LiftedTag>(std::move(state), state_metric);
}

// Diagnostics
void SuccessorGenerator<LiftedTag>::print_summary(size_t verbosity) const
{
    if (verbosity < 1)
        return;

    std::cout << "[Successor generator] Summary" << std::endl;
    fmt::print(std::cout, "{}\n", m_workspace.statistics);
    auto successor_generator_rule_statistics = std::vector<datalog::RuleStatistics> {};
    for (const auto& ws_rule : m_workspace.rules)
        successor_generator_rule_statistics.push_back(ws_rule->common.statistics);
    fmt::print(std::cout, "{}\n", datalog::compute_aggregated_rule_statistics(successor_generator_rule_statistics));
    auto successor_generator_rule_worker_statistics = std::vector<datalog::RuleWorkerStatistics> {};
    for (const auto& ws_rule : m_workspace.rules)
        for (const auto& worker : ws_rule->worker)
            successor_generator_rule_worker_statistics.push_back(worker.solve.statistics);
    fmt::print(std::cout, "{}\n", datalog::compute_aggregated_rule_worker_statistics(successor_generator_rule_worker_statistics));
}

void SuccessorGenerator<LiftedTag>::compute_action_facts(const Node<LiftedTag>& node)
{
    const auto state = node.get_state();
    auto merge_context = fp::MergeDatalogContext { m_workspace.datalog_builder, m_workspace.workspace_repository };
    const auto& program = m_task->get_action_program();

    insert_extended_state(state.get_unpacked_state(),
                          *m_task->get_repository(),
                          program.get_translation_context().p2d,
                          merge_context,
                          m_workspace.facts.fact_sets,
                          m_workspace.facts.assignment_sets);

    auto ctx = d::ProgramExecutionContext<d::NoOrAnnotationPolicy, d::NoAndAnnotationPolicy, d::NoTerminationPolicy>(m_workspace,
                                                                                                                     program.get_const_program_workspace());
    ctx.clear();
    m_execution_context->arena().execute([&] { d::solve_bottom_up(ctx); });
}

static_assert(SuccessorGeneratorConcept<SuccessorGenerator<LiftedTag>, LiftedTag>);
}
