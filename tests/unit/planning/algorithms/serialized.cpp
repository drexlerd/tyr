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

#include "tyr/common/json_loader.hpp"

#include <gtest/gtest.h>
#include <tyr/formalism/formalism.hpp>
#include <tyr/planning/planning.hpp>

namespace p = tyr::planning;
namespace fp = tyr::formalism::planning;

namespace tyr::tests
{
namespace
{
template<p::TaskKind Kind>
class SilentBrfsEventHandler : public p::brfs::EventHandler<Kind>
{
private:
    p::Statistics m_statistics;

public:
    void on_expand_node(const p::Node<Kind>& node) override { static_cast<void>(node); }
    void on_expand_goal_node(const p::Node<Kind>& node) override { static_cast<void>(node); }
    void on_generate_node(const p::Node<Kind>& source_node, const p::LabeledNode<Kind>& labeled_succ_node) override
    {
        static_cast<void>(source_node);
        static_cast<void>(labeled_succ_node);
    }
    void on_prune_node(const p::Node<Kind>& node) override { static_cast<void>(node); }
    void on_prune_node(const p::Node<Kind>& source_node, const p::LabeledNode<Kind>& labeled_succ_node) override
    {
        static_cast<void>(source_node);
        static_cast<void>(labeled_succ_node);
    }
    void on_start_search(const p::Node<Kind>& node) override
    {
        static_cast<void>(node);
        m_statistics = p::Statistics();
    }
    void on_finish_layer(uint_t layer) override { static_cast<void>(layer); }
    void on_end_search(p::SearchStatus status) override { static_cast<void>(status); }
    void on_solved(const p::Plan<Kind>& plan) override { static_cast<void>(plan); }
    const p::Statistics& get_search_statistics() const override { return m_statistics; }
    const p::Statistics& get_statistics() const override { return m_statistics; }
};

struct GroundSearchContext
{
    p::TaskPtr<p::GroundTag> task;
    p::SuccessorGeneratorPtr<p::GroundTag> successor_generator;
};

GroundSearchContext create_gripper_context()
{
    const auto root = tyr::common::root_path();
    const auto domain_file = root / "data/planning-benchmarks/tests/classical/gripper/domain.pddl";
    const auto task_file = root / "data/planning-benchmarks/tests/classical/gripper/test-1.pddl";

    auto execution_context = ExecutionContext::create(1);
    auto task = p::Task<p::LiftedTag>(fp::Parser(domain_file).parse_task(task_file)).instantiate_ground_task(*execution_context).task;
    auto axiom_evaluator = p::AxiomEvaluatorFactory<p::GroundTag>().create(task, execution_context);
    auto state_repository = p::StateRepositoryFactory<p::GroundTag>().create(task, axiom_evaluator);
    auto successor_generator = p::SuccessorGeneratorFactory<p::GroundTag>().create(task, execution_context, state_repository);

    return GroundSearchContext { std::move(task), std::move(successor_generator) };
}
}

TEST(TyrPlanningSerialized, BrfsSubsolverMatchesDirectBrfs)
{
    auto direct_context = create_gripper_context();
    auto serialized_context = create_gripper_context();

    auto direct_options = p::brfs::Options<p::GroundTag> {};
    direct_options.event_handler = std::make_shared<SilentBrfsEventHandler<p::GroundTag>>();
    const auto direct_result = p::brfs::find_solution(*direct_context.task, *direct_context.successor_generator, direct_options);

    auto brfs_solver = p::brfs::Solver<p::GroundTag> { serialized_context.task, serialized_context.successor_generator, p::brfs::Options<p::GroundTag> {} };
    brfs_solver.options.event_handler = std::make_shared<SilentBrfsEventHandler<p::GroundTag>>();

    auto serialized_options = p::serialized::Options<p::GroundTag, decltype(brfs_solver)> {};
    const auto event_handler = p::serialized::DefaultEventHandler<p::GroundTag, decltype(brfs_solver)>::create();
    serialized_options.event_handler = event_handler;
    serialized_options.subgoal_strategy = p::SerializedGoalStrategy<p::GroundTag>::create(*serialized_context.task);
    serialized_options.goal_strategy = p::ConjunctiveGoalStrategy<p::GroundTag>::create(*serialized_context.task);

    const auto serialized_result = p::serialized::find_solution(brfs_solver, serialized_options);

    ASSERT_EQ(direct_result.status, p::SearchStatus::SOLVED);
    ASSERT_TRUE(direct_result.plan);
    ASSERT_EQ(serialized_result.status, p::SearchStatus::SOLVED);
    ASSERT_TRUE(serialized_result.plan);

    EXPECT_EQ(serialized_result.plan->get_length(), direct_result.plan->get_length());
    EXPECT_GE(event_handler->get_statistics().get_num_subsearches(), 1);
    EXPECT_EQ(event_handler->get_statistics().get_search_statistics().size(), event_handler->get_statistics().get_num_subsearches());
    EXPECT_EQ(event_handler->get_statistics().get_solver_statistics().size(), event_handler->get_statistics().get_num_subsearches());
}

}
