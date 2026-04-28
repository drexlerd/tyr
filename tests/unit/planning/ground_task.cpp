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

#include <string>

namespace p = tyr::planning;
namespace f = tyr::formalism;
namespace fp = tyr::formalism::planning;

namespace tyr::tests
{
namespace
{
p::GroundTaskPtr compute_ground_task(const fs::path& domain_filepath, const fs::path& problem_filepath)
{
    auto execution_context = ExecutionContext(1);
    return p::LiftedTask(fp::Parser(domain_filepath).parse_task(problem_filepath)).instantiate_ground_task(execution_context).task;
}

p::SuccessorGenerator<p::GroundTag> create_successor_generator(std::shared_ptr<p::Task<p::GroundTag>> task)
{
    return p::SuccessorGenerator<p::GroundTag>(task, ExecutionContext::create(1));
}

fs::path absolute(const std::string& subdir) { return fs::path(std::string(ROOT_DIR)) / "data" / "tests" / subdir; }

struct GroundTaskCase
{
    std::string name;
    std::string subdir;
    size_t expected_fluent_atoms;
    size_t expected_derived_atoms;
    size_t expected_actions;
    size_t expected_axioms;
    size_t expected_successors;
};

std::string test_name(const testing::TestParamInfo<GroundTaskCase>& info)
{
    return info.param.name;
}
}

class GroundTaskTest : public ::testing::TestWithParam<GroundTaskCase>
{
};

TEST_P(GroundTaskTest, HasExpectedGroundTaskAndSuccessorCounts)
{
    const auto& param = GetParam();
    auto ground_task = compute_ground_task(absolute(param.subdir + "/domain.pddl"), absolute(param.subdir + "/test-1.pddl"));

    EXPECT_EQ(ground_task->get_num_atoms<f::FluentTag>(), param.expected_fluent_atoms);
    EXPECT_EQ(ground_task->get_num_atoms<f::DerivedTag>(), param.expected_derived_atoms);
    EXPECT_EQ(ground_task->get_num_actions(), param.expected_actions);
    EXPECT_EQ(ground_task->get_num_axioms(), param.expected_axioms);

    auto successor_generator = create_successor_generator(ground_task);

    EXPECT_EQ(successor_generator.get_labeled_successor_nodes(successor_generator.get_initial_node()).size(), param.expected_successors);
}

INSTANTIATE_TEST_SUITE_P(TyrPlanningGroundTask,
                         GroundTaskTest,
                         ::testing::Values(GroundTaskCase { "Agricola", "classical/agricola", 141, 0, 12443, 0, 8 },
                                           GroundTaskCase { "Airport", "classical/airport", 58, 355, 43, 420, 2 },
                                           GroundTaskCase { "Assembly", "classical/assembly", 7, 2, 6, 2, 3 },
                                           GroundTaskCase { "Barman", "classical/barman", 26, 0, 66, 0, 4 },
                                           GroundTaskCase { "Blocks3", "classical/blocks_3", 15, 0, 45, 0, 2 },
                                           GroundTaskCase { "Blocks4", "classical/blocks_4", 19, 0, 24, 0, 2 },
                                           GroundTaskCase { "Childsnack", "classical/childsnack", 8, 0, 7, 0, 3 },
                                           GroundTaskCase { "Delivery", "classical/delivery", 10, 0, 16, 0, 2 },
                                           GroundTaskCase { "Driverlog", "classical/driverlog", 10, 0, 14, 0, 2 },
                                           GroundTaskCase { "Ferry", "classical/ferry", 9, 0, 10, 0, 3 },
                                           GroundTaskCase { "FoCounters", "numeric/fo-counters", 0, 0, 12, 0, 9 },
                                           GroundTaskCase { "Grid", "classical/grid", 21, 0, 29, 0, 1 },
                                           GroundTaskCase { "Gripper", "classical/gripper", 12, 0, 20, 0, 6 },
                                           GroundTaskCase { "Hiking", "classical/hiking", 12, 0, 41, 0, 18 },
                                           GroundTaskCase { "Logistics", "classical/logistics", 9, 0, 14, 0, 6 },
                                           GroundTaskCase { "Miconic", "classical/miconic", 8, 0, 6, 0, 3 },
                                           GroundTaskCase { "MiconicFulladl", "classical/miconic-fulladl", 9, 7, 10, 15, 3 },
                                           GroundTaskCase { "MiconicSimpleadl", "classical/miconic-simpleadl", 4, 0, 4, 0, 2 },
                                           GroundTaskCase { "Parcprinter", "classical/parcprinter", 43, 0, 25, 0, 1 },
                                           GroundTaskCase { "Pathways", "classical/pathways", 47, 0, 78, 0, 16 },
                                           GroundTaskCase { "Philosophers", "classical/philosophers", 50, 21, 34, 34, 2 },
                                           GroundTaskCase { "PsrMiddle", "classical/psr-middle", 13, 351, 28, 467, 1 },
                                           GroundTaskCase { "Pushworld", "classical/pushworld", 327, 0, 1126, 0, 4 },
                                           GroundTaskCase { "Refuel", "numeric/refuel", 0, 0, 1, 0, 1 },
                                           GroundTaskCase { "RefuelAdl", "numeric/refuel-adl", 6, 1, 15, 3, 5 },
                                           GroundTaskCase { "Reward", "classical/reward", 7, 0, 6, 0, 1 },
                                           GroundTaskCase { "Rovers", "classical/rovers", 12, 0, 7, 0, 2 },
                                           GroundTaskCase { "Satellite", "classical/satellite", 12, 0, 18, 0, 4 },
                                           GroundTaskCase { "Schedule", "classical/schedule", 45, 0, 49, 0, 44 },
                                           GroundTaskCase { "Sokoban", "classical/sokoban", 260, 0, 526, 0, 3 },
                                           GroundTaskCase { "Spanner", "classical/spanner", 9, 0, 4, 0, 1 },
                                           GroundTaskCase { "Tpp", "numeric/tpp", 6, 0, 56, 0, 5 },
                                           GroundTaskCase { "Transport", "classical/transport", 26, 0, 104, 0, 5 },
                                           GroundTaskCase { "Visitall", "classical/visitall", 14, 0, 12, 0, 2 },
                                           GroundTaskCase { "Woodworking", "classical/woodworking", 52, 0, 198, 0, 8 },
                                           GroundTaskCase { "Zenotravel", "numeric/zenotravel", 15, 0, 37, 0, 7 }),
                         test_name);
}
