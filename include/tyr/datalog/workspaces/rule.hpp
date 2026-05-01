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

#ifndef TYR_DATALOG_WORKSPACES_RULE_HPP_
#define TYR_DATALOG_WORKSPACES_RULE_HPP_

#include "tyr/common/declarations.hpp"
#include "tyr/common/equal_to.hpp"
#include "tyr/common/hash.hpp"
#include "tyr/datalog/consistency_graph.hpp"
#include "tyr/datalog/delta_kpkc.hpp"
#include "tyr/datalog/policies/annotation_concept.hpp"
#include "tyr/datalog/statistics/rule.hpp"
#include "tyr/formalism/binding_index.hpp"
#include "tyr/formalism/datalog/builder.hpp"
#include "tyr/formalism/datalog/ground_atom_index.hpp"
#include "tyr/formalism/datalog/repository.hpp"
#include "tyr/formalism/datalog/views.hpp"
#include "tyr/formalism/object_index.hpp"

#include <atomic>
#include <cassert>
#include <chrono>
#include <oneapi/tbb/enumerable_thread_specific.h>
#include <oneapi/tbb/spin_mutex.h>
#include <utility>
#include <type_traits>
#include <variant>
#include <vector>

namespace tyr::datalog
{

struct PredicateHeadIteration
{
    Index<formalism::Predicate<formalism::FluentTag>> relation;
    UnorderedSet<Index<formalism::Row>> rows;

    PredicateHeadIteration() = default;
    explicit PredicateHeadIteration(Index<formalism::Predicate<formalism::FluentTag>> relation) : relation(relation), rows() {}

    void clear() noexcept { rows.clear(); }
};

struct FunctionHeadUpdate
{
    Index<formalism::Row> row;
    ClosedInterval<float_t> interval;
    Cost cost;

    FunctionHeadUpdate(Index<formalism::Row> row, ClosedInterval<float_t> interval, Cost cost) : row(row), interval(interval), cost(cost) {}

    auto identifying_members() const noexcept { return std::tie(row, interval, cost); }
};

struct FunctionHeadIteration
{
    Index<formalism::Function<formalism::FluentTag>> relation;
    UnorderedSet<FunctionHeadUpdate> updates;

    FunctionHeadIteration() = default;
    explicit FunctionHeadIteration(Index<formalism::Function<formalism::FluentTag>> relation) : relation(relation), updates() {}

    void clear() noexcept { updates.clear(); }
};

using RuleHeadIteration = std::variant<PredicateHeadIteration, FunctionHeadIteration>;

template<typename AndAP>
struct RuleWorkspace
{
    struct Common
    {
        explicit Common(const formalism::datalog::Repository& program_repository,
                        const formalism::datalog::Repository& workspace_repository,
                        const StaticConsistencyGraph& static_consistency_graph);

        void initialize_iteration(const StaticConsistencyGraph& static_consistency_graph, const AssignmentSets& assignment_sets);

        void clear() noexcept;

        /// Program repository to ground witnesses for which ground entities must already exist and we can simply call find.
        const formalism::datalog::Repository& program_repository;
        const formalism::datalog::Repository& workspace_repository;

        /// KPKC
        kpkc::DeltaKPKC kpkc;

        /// Statistics
        RuleStatistics statistics;
    };

    /// @brief Each iteration consists of
    /// - generate all k-cliques
    /// - ground witnesses
    /// - annotate witnesses
    struct Iteration
    {
        explicit Iteration(formalism::datalog::RepositoryFactory& factory, const ConstRuleWorkspace& cws, const Common& common);

        void clear() noexcept;

        /// Merge stage into rule execution context
        formalism::datalog::Repository workspace_overlay_repository;

        /// Heads
        RuleHeadIteration head;

        // Annotations stored in program_overlay_repository
        AndAnnotationsMap and_annot;
        NumericAndAnnotationsMap numeric_and_annot;

        /// KPKC
        kpkc::Workspace kpkc_workspace;
    };

    struct Solve
    {
        explicit Solve(formalism::datalog::RepositoryFactory& factory,
                       const formalism::datalog::Repository& program_repository,
                       const formalism::datalog::Repository& workspace_repository,
                       const AndAP& and_ap);

        void clear() noexcept;

        AndAP and_ap;

        /// Persistent memory
        formalism::datalog::Repository program_overlay_repository;

        /// In debug mode, we accumulate all bindings to verify the correctness of delta-kpkc
        UnorderedSet<IndexList<formalism::Object>> seen_bindings_dbg;

        UnorderedSet<formalism::datalog::RuleBindingView> pending_rule_bindings;

        /// Statistics
        RuleWorkerStatistics statistics;
    };

    struct Worker
    {
        explicit Worker(formalism::datalog::RepositoryFactory& factory,
                        const formalism::datalog::Repository& program_repository,
                        const formalism::datalog::Repository& workspace_repository,
                        const ConstRuleWorkspace& cws,
                        const Common& common,
                        const AndAP& and_ap);

        void clear() noexcept;

        formalism::datalog::Builder builder;
        IndexList<formalism::Object> binding;

        Iteration iteration;
        Solve solve;
    };

    RuleWorkspace(formalism::datalog::RepositoryFactory& factory,
                  const formalism::datalog::Repository& program_repository,
                  const formalism::datalog::Repository& workspace_repository,
                  const ConstRuleWorkspace& cws,
                  const AndAP& and_ap);
    RuleWorkspace(const RuleWorkspace& other) = delete;
    RuleWorkspace& operator=(const RuleWorkspace& other) = delete;
    RuleWorkspace(RuleWorkspace&& other) = delete;
    RuleWorkspace& operator=(RuleWorkspace&& other) = delete;

    void clear() noexcept;

    Common common;

    oneapi::tbb::enumerable_thread_specific<Worker> worker;
};

struct ConstRuleWorkspace
{
public:
    auto get_rule() const noexcept { return rule; }
    auto get_witness_rule() const noexcept { return witness_rule; }
    auto get_nullary_condition() const noexcept { return nullary_condition; }
    auto get_unary_overapproximation_rule() const noexcept { return unary_overapproximation_rule; }
    auto get_binary_overapproximation_rule() const noexcept { return binary_overapproximation_rule; }
    auto get_static_binary_overapproximation_rule() const noexcept { return static_binary_overapproximation_rule; }
    auto get_conflicting_overapproximation_rule() const noexcept { return conflicting_overapproximation_rule; }
    const auto& get_static_consistency_graph() const noexcept { return static_consistency_graph; }

    ConstRuleWorkspace(formalism::datalog::RuleView rule,
                       formalism::datalog::Repository& repository,
                       const analysis::VariableDomainList& parameter_domains,
                       size_t num_objects,
                       size_t num_fluent_predicates,
                       const TaggedAssignmentSets<formalism::StaticTag>& static_assignment_sets);

private:
    formalism::datalog::RuleView rule;
    formalism::datalog::RuleView witness_rule;
    formalism::datalog::GroundConjunctiveConditionView nullary_condition;
    formalism::datalog::RuleView unary_overapproximation_rule;
    formalism::datalog::RuleView binary_overapproximation_rule;
    formalism::datalog::RuleView static_binary_overapproximation_rule;
    formalism::datalog::RuleView conflicting_overapproximation_rule;

    StaticConsistencyGraph static_consistency_graph;
};

/**
 * Implementations
 */

template<typename AndAP>
RuleWorkspace<AndAP>::Common::Common(const formalism::datalog::Repository& program_repository,
                                     const formalism::datalog::Repository& workspace_repository,
                                     const StaticConsistencyGraph& static_consistency_graph) :
    program_repository(program_repository),
    workspace_repository(workspace_repository),
    kpkc(static_consistency_graph),
    statistics()
{
}

template<typename AndAP>
void RuleWorkspace<AndAP>::Common::clear() noexcept
{
    kpkc.reset();
}

template<typename AndAP>
void RuleWorkspace<AndAP>::Common::initialize_iteration(const StaticConsistencyGraph& static_consistency_graph, const AssignmentSets& assignment_sets)
{
    kpkc.set_next_assignment_sets(static_consistency_graph, assignment_sets);
}

template<typename AndAP>
RuleWorkspace<AndAP>::Iteration::Iteration(formalism::datalog::RepositoryFactory& factory, const ConstRuleWorkspace& cws, const Common& common) :
    workspace_overlay_repository(factory.create(&common.workspace_repository)),
    head(visit(
        [](auto&& head) -> RuleHeadIteration
        {
            using Head = std::decay_t<decltype(head)>;

            if constexpr (std::is_same_v<Head, formalism::datalog::AtomView<formalism::FluentTag>>)
            {
                return PredicateHeadIteration(head.get_predicate().get_index());
            }
            else
            {
                return visit(
                    [](auto&& effect) -> RuleHeadIteration
                    {
                        return FunctionHeadIteration(effect.get_fterm().get_function().get_index());
                    },
                    head.get_variant());
            }
        },
        cws.get_rule().get_head())),
    and_annot(),
    numeric_and_annot(),
    kpkc_workspace(common.kpkc.get_graph_layout())
{
}

template<typename AndAP>
void RuleWorkspace<AndAP>::Iteration::clear() noexcept
{
    workspace_overlay_repository.clear();
    std::visit([](auto& arg) { arg.clear(); }, head);
    and_annot.clear();
    numeric_and_annot.clear();
}

template<typename AndAP>
RuleWorkspace<AndAP>::Solve::Solve(formalism::datalog::RepositoryFactory& factory,
                                   const formalism::datalog::Repository& program_repository,
                                   const formalism::datalog::Repository& workspace_repository,
                                   const AndAP& and_ap) :
    and_ap(and_ap),
    program_overlay_repository(factory.create(&program_repository)),
    seen_bindings_dbg(),
    pending_rule_bindings(),
    statistics()
{
}

template<typename AndAP>
void RuleWorkspace<AndAP>::Solve::clear() noexcept
{
    program_overlay_repository.clear();
    seen_bindings_dbg.clear();
    pending_rule_bindings.clear();
}

template<typename AndAP>
RuleWorkspace<AndAP>::Worker::Worker(formalism::datalog::RepositoryFactory& factory,
                                     const formalism::datalog::Repository& program_repository,
                                     const formalism::datalog::Repository& workspace_repository,
                                     const ConstRuleWorkspace& cws,
                                     const Common& common,
                                     const AndAP& and_ap) :
    builder(),
    binding(),
    iteration(factory, cws, common),
    solve(factory, program_repository, workspace_repository, and_ap)
{
}

template<typename AndAP>
void RuleWorkspace<AndAP>::Worker::clear() noexcept
{
    iteration.clear();
    solve.clear();
}

template<typename AndAP>
RuleWorkspace<AndAP>::RuleWorkspace(formalism::datalog::RepositoryFactory& factory_,
                                    const formalism::datalog::Repository& program_repository_,
                                    const formalism::datalog::Repository& workspace_repository_,
                                    const ConstRuleWorkspace& cws_,
                                    const AndAP& and_ap_) :
    common(program_repository_, workspace_repository_, cws_.get_static_consistency_graph()),
    worker([this, program_repository = &program_repository_, workspace_repository = &workspace_repository_, factory = &factory_, cws = &cws_, and_ap = &and_ap_]
           { return Worker(*factory, *program_repository, *workspace_repository, *cws, this->common, *and_ap); })
{
}

template<typename AndAP>
void RuleWorkspace<AndAP>::clear() noexcept
{
    common.clear();
    for (auto& w : worker)
        w.clear();
}

}

#endif
