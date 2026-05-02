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

#include <string>

namespace p = tyr::planning;
namespace f = tyr::formalism;
namespace fp = tyr::formalism::planning;

namespace tyr::tests
{
namespace
{
struct GroundTaskCase
{
    std::string name;
    std::string domain_file;
    std::string task_file;
    size_t expected_fluent_atoms;
    size_t expected_derived_atoms;
    size_t expected_actions;
    size_t expected_axioms;
    size_t expected_successors;
};

GroundTaskCase parse_case(const boost::json::object& object)
{
    return GroundTaskCase { tyr::common::as_string(object, "name", "case"),
                            tyr::common::as_string(object, "domain_file", "case"),
                            tyr::common::as_string(object, "task_file", "case"),
                            tyr::common::as_size(object, "expected_fluent_atoms", "case"),
                            tyr::common::as_size(object, "expected_derived_atoms", "case"),
                            tyr::common::as_size(object, "expected_actions", "case"),
                            tyr::common::as_size(object, "expected_axioms", "case"),
                            tyr::common::as_size(object, "expected_successors", "case") };
}

std::vector<GroundTaskCase> load_cases()
{
    const auto suite = tyr::common::load_json_file(tyr::common::root_path() / "tests/unit/planning/ground_task.json");
    const auto& suite_object = tyr::common::as_object(suite, "suite");
    const auto* cases_value = suite_object.if_contains("cases");
    if (!cases_value)
        throw std::runtime_error("suite.cases is required.");

    auto result = std::vector<GroundTaskCase> {};
    for (const auto& case_value : tyr::common::as_array(*cases_value, "suite.cases"))
        result.push_back(parse_case(tyr::common::as_object(case_value, "case")));
    return result;
}

}

class GroundTaskTest : public ::testing::TestWithParam<GroundTaskCase>
{
};

TEST_P(GroundTaskTest, HasExpectedGroundTaskAndSuccessorCounts)
{
    const auto& param = GetParam();
    auto execution_context = ExecutionContext(1);
    auto ground_task = p::Task<p::LiftedTag>(fp::Parser(tyr::common::root_path() / param.domain_file).parse_task(tyr::common::root_path() / param.task_file))
                           .instantiate_ground_task(execution_context)
                           .task;

    EXPECT_EQ(ground_task->get_num_atoms<f::FluentTag>(), param.expected_fluent_atoms);
    EXPECT_EQ(ground_task->get_num_atoms<f::DerivedTag>(), param.expected_derived_atoms);
    EXPECT_EQ(ground_task->get_num_actions(), param.expected_actions);
    EXPECT_EQ(ground_task->get_num_axioms(), param.expected_axioms);

    auto successor_generator = p::SuccessorGenerator<p::GroundTag>(ground_task, ExecutionContext::create(1));

    EXPECT_EQ(successor_generator.get_labeled_successor_nodes(successor_generator.get_initial_node()).size(), param.expected_successors);
}

INSTANTIATE_TEST_SUITE_P(TyrPlanningGroundTask,
                         GroundTaskTest,
                         ::testing::ValuesIn(load_cases()),
                         [](const testing::TestParamInfo<GroundTaskCase>& info) { return info.param.name; });
}
