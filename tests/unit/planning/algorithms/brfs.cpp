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
#include <filesystem>
#include <tyr/formalism/formalism.hpp>
#include <tyr/planning/planning.hpp>

namespace p = tyr::planning;
namespace fp = tyr::formalism::planning;

namespace tyr::tests
{
namespace
{
struct GroundSearchContext
{
    p::TaskPtr<p::GroundTag> task;
    p::SuccessorGeneratorPtr<p::GroundTag> successor_generator;
};

struct BrfsCase
{
    std::string name;
    std::filesystem::path domain_file;
    std::filesystem::path task_file;
};

BrfsCase parse_case(const boost::json::object& suite, const boost::json::object& object)
{
    return BrfsCase { tyr::common::as_string(object, "name", "case"),
                      tyr::common::suite_path(suite, tyr::common::as_string(object, "domain_file", "case")),
                      tyr::common::suite_path(suite, tyr::common::as_string(object, "task_file", "case")) };
}

std::vector<BrfsCase> load_cases()
{
    const auto suite = tyr::common::load_json_file(tyr::common::root_path() / "tests/unit/planning/algorithms/brfs_vs_blind_astar.json");
    const auto& suite_object = tyr::common::as_object(suite, "suite");
    const auto* cases_value = suite_object.if_contains("cases");
    if (!cases_value)
        throw std::runtime_error("suite.cases is required.");

    auto result = std::vector<BrfsCase> {};
    for (const auto& case_value : tyr::common::as_array(*cases_value, "suite.cases"))
        result.push_back(parse_case(suite_object, tyr::common::as_object(case_value, "case")));
    return result;
}

GroundSearchContext create_ground_context(const std::filesystem::path& domain_file, const std::filesystem::path& task_file)
{
    auto execution_context = ExecutionContext::create(1);
    auto task = p::Task<p::LiftedTag>(fp::Parser(domain_file).parse_task(task_file)).instantiate_ground_task(*execution_context).task;
    auto axiom_evaluator = p::AxiomEvaluatorFactory<p::GroundTag>().create(task, execution_context);
    auto state_repository = p::StateRepositoryFactory<p::GroundTag>().create(task, axiom_evaluator);
    auto successor_generator = p::SuccessorGeneratorFactory<p::GroundTag>().create(task, execution_context, state_repository);

    return GroundSearchContext { std::move(task), std::move(successor_generator) };
}
}

class BrfsTest : public ::testing::TestWithParam<BrfsCase>
{
};

TEST_P(BrfsTest, PlanLengthMatchesBlindAstar)
{
    const auto& param = GetParam();
    auto brfs_context = create_ground_context(param.domain_file, param.task_file);
    auto astar_context = create_ground_context(param.domain_file, param.task_file);
    auto heuristic = p::BlindHeuristic<p::GroundTag>::create();

    const auto brfs_result = p::brfs::find_solution(*brfs_context.task, *brfs_context.successor_generator);
    auto astar_options = p::astar_eager::Options<p::GroundTag>();
    astar_options.action_cost_mode = p::ActionCostMode::UNIT;
    const auto astar_result = p::astar_eager::find_solution(*astar_context.task, *astar_context.successor_generator, *heuristic, astar_options);

    ASSERT_EQ(brfs_result.status, p::SearchStatus::SOLVED);
    ASSERT_EQ(astar_result.status, p::SearchStatus::SOLVED);
    ASSERT_TRUE(brfs_result.plan);
    ASSERT_TRUE(astar_result.plan);

    EXPECT_EQ(brfs_result.plan->get_length(), astar_result.plan->get_length());
}

INSTANTIATE_TEST_SUITE_P(TyrPlanningBrfs,
                         BrfsTest,
                         ::testing::ValuesIn(load_cases()),
                         [](const testing::TestParamInfo<BrfsCase>& info) { return info.param.name; });

}
