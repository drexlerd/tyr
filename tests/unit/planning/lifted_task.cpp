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

#include <gtest/gtest.h>
#include <tyr/formalism/formalism.hpp>
#include <tyr/planning/planning.hpp>

#include <algorithm>
#include <string>

namespace p = tyr::planning;
namespace f = tyr::formalism;
namespace fp = tyr::formalism::planning;

namespace tyr::tests
{
namespace
{
p::LiftedTaskPtr compute_lifted_task(const fs::path& domain_filepath, const fs::path& problem_filepath)
{
    return p::LiftedTask::create(fp::Parser(domain_filepath).parse_task(problem_filepath));
}

p::SuccessorGenerator<p::LiftedTag> create_successor_generator(std::shared_ptr<p::Task<p::LiftedTag>> task)
{
    return p::SuccessorGenerator<p::LiftedTag>(task, ExecutionContext::create(1));
}

fs::path absolute(const std::string& subdir) { return fs::path(std::string(ROOT_DIR)) / "data" / "tests" / subdir; }

struct LiftedSuccessorCountCase
{
    std::string name;
    std::string subdir;
    size_t expected_successors;
};

std::string test_name(const testing::TestParamInfo<LiftedSuccessorCountCase>& info)
{
    return info.param.name;
}

void expect_same_node(const p::Node<p::LiftedTag>& expected, const p::Node<p::LiftedTag>& actual)
{
    EXPECT_EQ(uint_t(expected.get_state().get_index()), uint_t(actual.get_state().get_index()));
    EXPECT_TRUE(f::apply(f::OpEq {}, expected.get_metric(), actual.get_metric()))
        << "expected metric " << expected.get_metric() << ", actual metric " << actual.get_metric();
}

void expect_same_binding(fp::ActionBindingView expected, fp::ActionBindingView actual)
{
    EXPECT_EQ(uint_t(expected.get_relation().get_index()), uint_t(actual.get_relation().get_index()));

    const auto expected_objects = expected.get_data();
    const auto actual_objects = actual.get_data();
    ASSERT_EQ(expected_objects.size(), actual_objects.size());
    for (size_t i = 0; i < expected_objects.size(); ++i)
        EXPECT_EQ(uint_t(expected_objects[i]), uint_t(actual_objects[i]));
}

void expect_same_binding(fp::ActionBindingView expected, const Data<formalism::RelationBinding<formalism::planning::Action>>& actual)
{
    EXPECT_EQ(uint_t(expected.get_relation().get_index()), uint_t(actual.relation));

    const auto expected_objects = expected.get_data();
    ASSERT_EQ(expected_objects.size(), actual.objects.size());
    for (size_t i = 0; i < expected_objects.size(); ++i)
        EXPECT_EQ(uint_t(expected_objects[i]), uint_t(actual.objects[i]));
}

bool are_same_binding(fp::ActionBindingView lhs, fp::ActionBindingView rhs)
{
    if (lhs.get_relation().get_index() != rhs.get_relation().get_index())
        return false;

    const auto lhs_objects = lhs.get_data();
    const auto rhs_objects = rhs.get_data();
    return std::ranges::equal(lhs_objects, rhs_objects);
}

bool are_same_binding(fp::ActionBindingView lhs, const Data<formalism::RelationBinding<formalism::planning::Action>>& rhs)
{
    if (lhs.get_relation().get_index() != rhs.relation)
        return false;

    return std::ranges::equal(lhs.get_data(), rhs.objects);
}

void expect_action_binding_apis_match_ground_actions(const std::string& subdir)
{
    auto lifted_task = compute_lifted_task(absolute(subdir + "/domain.pddl"), absolute(subdir + "/test-1.pddl"));
    auto successor_generator = create_successor_generator(lifted_task);
    const auto initial_node = successor_generator.get_initial_node();

    const auto ground_successors = successor_generator.get_labeled_successor_nodes(initial_node);
    const auto interned_bindings = successor_generator.get_applicable_action_bindings(initial_node);

    ASSERT_EQ(ground_successors.size(), interned_bindings.size());
    for (const auto binding : interned_bindings)
    {
        const auto expected =
            std::ranges::find_if(ground_successors, [&](const auto& successor) { return are_same_binding(successor.label.get_row(), binding); });
        ASSERT_NE(expected, ground_successors.end());

        expect_same_binding(expected->label.get_row(), binding);
        expect_same_node(expected->node, successor_generator.get_successor_node(initial_node, binding));
    }

    size_t no_interning_pos = 0;
    successor_generator.for_each_applicable_action_binding(initial_node,
                                                           [&](const auto& binding)
                                                           {
                                                               const auto expected = std::ranges::find_if(ground_successors,
                                                                                                          [&](const auto& successor)
                                                                                                          {
                                                                                                              return are_same_binding(
                                                                                                                  successor.label.get_row(),
                                                                                                                  binding);
                                                                                                          });
                                                               ASSERT_NE(expected, ground_successors.end());

                                                               expect_same_binding(expected->label.get_row(), binding);
                                                               expect_same_node(expected->node, successor_generator.get_successor_node(initial_node, binding));
                                                               ++no_interning_pos;
                                                           });

    EXPECT_EQ(no_interning_pos, ground_successors.size());
}
}

class LiftedTaskSuccessorCountTest : public ::testing::TestWithParam<LiftedSuccessorCountCase>
{
};

TEST_P(LiftedTaskSuccessorCountTest, InitialNodeHasExpectedSuccessorCount)
{
    const auto& param = GetParam();
    auto lifted_task = compute_lifted_task(absolute(param.subdir + "/domain.pddl"), absolute(param.subdir + "/test-1.pddl"));
    auto successor_generator = create_successor_generator(lifted_task);

    EXPECT_EQ(successor_generator.get_labeled_successor_nodes(successor_generator.get_initial_node()).size(), param.expected_successors);
}

TEST_P(LiftedTaskSuccessorCountTest, ActionBindingApisMatchGroundActions)
{
    expect_action_binding_apis_match_ground_actions(GetParam().subdir);
}

INSTANTIATE_TEST_SUITE_P(TyrPlanningLiftedTask,
                         LiftedTaskSuccessorCountTest,
                         ::testing::Values(LiftedSuccessorCountCase { "Agricola", "classical/agricola", 8 },
                                           LiftedSuccessorCountCase { "Airport", "classical/airport", 2 },
                                           LiftedSuccessorCountCase { "Assembly", "classical/assembly", 3 },
                                           LiftedSuccessorCountCase { "Barman", "classical/barman", 4 },
                                           LiftedSuccessorCountCase { "Blocks3", "classical/blocks_3", 2 },
                                           LiftedSuccessorCountCase { "Blocks4", "classical/blocks_4", 2 },
                                           LiftedSuccessorCountCase { "Childsnack", "classical/childsnack", 3 },
                                           LiftedSuccessorCountCase { "Delivery", "classical/delivery", 2 },
                                           LiftedSuccessorCountCase { "Driverlog", "classical/driverlog", 2 },
                                           LiftedSuccessorCountCase { "Ferry", "classical/ferry", 3 },
                                           LiftedSuccessorCountCase { "FoCounters", "numeric/fo-counters", 9 },
                                           LiftedSuccessorCountCase { "Grid", "classical/grid", 1 },
                                           LiftedSuccessorCountCase { "Gripper", "classical/gripper", 6 },
                                           LiftedSuccessorCountCase { "Hiking", "classical/hiking", 18 },
                                           LiftedSuccessorCountCase { "Logistics", "classical/logistics", 6 },
                                           LiftedSuccessorCountCase { "Miconic", "classical/miconic", 3 },
                                           LiftedSuccessorCountCase { "MiconicFulladl", "classical/miconic-fulladl", 3 },
                                           LiftedSuccessorCountCase { "MiconicSimpleadl", "classical/miconic-simpleadl", 2 },
                                           LiftedSuccessorCountCase { "Parcprinter", "classical/parcprinter", 1 },
                                           LiftedSuccessorCountCase { "Pathways", "classical/pathways", 16 },
                                           LiftedSuccessorCountCase { "Philosophers", "classical/philosophers", 2 },
                                           LiftedSuccessorCountCase { "PsrMiddle", "classical/psr-middle", 1 },
                                           LiftedSuccessorCountCase { "Pushworld", "classical/pushworld", 4 },
                                           LiftedSuccessorCountCase { "Refuel", "numeric/refuel", 1 },
                                           LiftedSuccessorCountCase { "RefuelAdl", "numeric/refuel-adl", 5 },
                                           LiftedSuccessorCountCase { "Reward", "classical/reward", 1 },
                                           LiftedSuccessorCountCase { "Rovers", "classical/rovers", 2 },
                                           LiftedSuccessorCountCase { "Satellite", "classical/satellite", 4 },
                                           LiftedSuccessorCountCase { "Schedule", "classical/schedule", 44 },
                                           LiftedSuccessorCountCase { "Sokoban", "classical/sokoban", 3 },
                                           LiftedSuccessorCountCase { "Spanner", "classical/spanner", 1 },
                                           LiftedSuccessorCountCase { "Tpp", "numeric/tpp", 5 },
                                           LiftedSuccessorCountCase { "Transport", "classical/transport", 5 },
                                           LiftedSuccessorCountCase { "Visitall", "classical/visitall", 2 },
                                           LiftedSuccessorCountCase { "Woodworking", "classical/woodworking", 8 },
                                           LiftedSuccessorCountCase { "Zenotravel", "numeric/zenotravel", 7 }),
                         test_name);
}
