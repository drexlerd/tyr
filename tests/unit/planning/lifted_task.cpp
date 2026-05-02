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

#include <algorithm>
#include <string>

namespace p = tyr::planning;
namespace f = tyr::formalism;
namespace fp = tyr::formalism::planning;

namespace tyr::tests
{
namespace
{
struct LiftedSuccessorCountCase
{
    std::string name;
    std::string domain_file;
    std::string task_file;
    size_t expected_successors;
};

LiftedSuccessorCountCase parse_case(const boost::json::object& object)
{
    return LiftedSuccessorCountCase { tyr::common::as_string(object, "name", "case"),
                                      tyr::common::as_string(object, "domain_file", "case"),
                                      tyr::common::as_string(object, "task_file", "case"),
                                      tyr::common::as_size(object, "expected_successors", "case") };
}

std::vector<LiftedSuccessorCountCase> load_cases()
{
    const auto suite = tyr::common::load_json_file(tyr::common::root_path() / "tests/unit/planning/lifted_task.json");
    const auto& suite_object = tyr::common::as_object(suite, "suite");
    const auto* cases_value = suite_object.if_contains("cases");
    if (!cases_value)
        throw std::runtime_error("suite.cases is required.");

    auto result = std::vector<LiftedSuccessorCountCase> {};
    for (const auto& case_value : tyr::common::as_array(*cases_value, "suite.cases"))
        result.push_back(parse_case(tyr::common::as_object(case_value, "case")));
    return result;
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

void expect_action_binding_apis_match_ground_actions(const LiftedSuccessorCountCase& test_case)
{
    auto lifted_task =
        p::Task<p::LiftedTag>::create(fp::Parser(tyr::common::root_path() / test_case.domain_file).parse_task(tyr::common::root_path() / test_case.task_file));
    auto successor_generator = p::SuccessorGenerator<p::LiftedTag>(lifted_task, ExecutionContext::create(1));
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
    auto lifted_task =
        p::Task<p::LiftedTag>::create(fp::Parser(tyr::common::root_path() / param.domain_file).parse_task(tyr::common::root_path() / param.task_file));
    auto successor_generator = p::SuccessorGenerator<p::LiftedTag>(lifted_task, ExecutionContext::create(1));

    EXPECT_EQ(successor_generator.get_labeled_successor_nodes(successor_generator.get_initial_node()).size(), param.expected_successors);
}

TEST_P(LiftedTaskSuccessorCountTest, ActionBindingApisMatchGroundActions)
{
    expect_action_binding_apis_match_ground_actions(GetParam());
}

INSTANTIATE_TEST_SUITE_P(TyrPlanningLiftedTask,
                         LiftedTaskSuccessorCountTest,
                         ::testing::ValuesIn(load_cases()),
                         [](const testing::TestParamInfo<LiftedSuccessorCountCase>& info) { return info.param.name; });
}
