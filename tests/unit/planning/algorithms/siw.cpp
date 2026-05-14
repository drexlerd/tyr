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

#include <filesystem>
#include <fmt/core.h>
#include <gtest/gtest.h>
#include <optional>
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

struct SiwCase
{
    std::string name;
    std::filesystem::path domain_file;
    std::filesystem::path task_file;
    uint_t max_arity;
    std::optional<p::SearchStatus> expected_status;
    std::optional<uint_t> expected_maximum_effective_width;
};

p::SearchStatus parse_status(const std::string& status)
{
    if (status == "SOLVED")
        return p::SearchStatus::SOLVED;
    if (status == "EXHAUSTED")
        return p::SearchStatus::EXHAUSTED;
    if (status == "UNSOLVABLE")
        return p::SearchStatus::UNSOLVABLE;
    if (status == "OUT_OF_TIME")
        return p::SearchStatus::OUT_OF_TIME;
    if (status == "OUT_OF_MEMORY")
        return p::SearchStatus::OUT_OF_MEMORY;
    if (status == "OUT_OF_STATES")
        return p::SearchStatus::OUT_OF_STATES;
    if (status == "FAILED")
        return p::SearchStatus::FAILED;
    if (status == "IN_PROGRESS")
        return p::SearchStatus::IN_PROGRESS;

    throw std::runtime_error("Unknown search status: " + status);
}

std::string to_string(p::SearchStatus status)
{
    switch (status)
    {
        case p::SearchStatus::IN_PROGRESS:
            return "IN_PROGRESS";
        case p::SearchStatus::OUT_OF_TIME:
            return "OUT_OF_TIME";
        case p::SearchStatus::OUT_OF_MEMORY:
            return "OUT_OF_MEMORY";
        case p::SearchStatus::OUT_OF_STATES:
            return "OUT_OF_STATES";
        case p::SearchStatus::FAILED:
            return "FAILED";
        case p::SearchStatus::EXHAUSTED:
            return "EXHAUSTED";
        case p::SearchStatus::SOLVED:
            return "SOLVED";
        case p::SearchStatus::UNSOLVABLE:
            return "UNSOLVABLE";
    }

    throw std::runtime_error("Unknown search status.");
}

SiwCase parse_case(const boost::json::object& suite, const boost::json::object& object)
{
    const auto max_arity = static_cast<uint_t>(tyr::common::as_size(suite, "max_arity", "suite"));
    auto expected_status = std::optional<p::SearchStatus> {};
    auto expected_maximum_effective_width = std::optional<uint_t> {};

    if (const auto* value = object.if_contains("expected_status"))
    {
        if (!value->is_string())
            throw std::runtime_error("case.expected_status must be a string.");
        expected_status = parse_status(std::string(value->as_string()));
    }
    if (const auto* value = object.if_contains("expected_maximum_effective_width"))
    {
        if (!value->is_int64() || value->as_int64() < 0)
            throw std::runtime_error("case.expected_maximum_effective_width must be a non-negative integer.");
        expected_maximum_effective_width = static_cast<uint_t>(value->as_int64());
    }

    return SiwCase { tyr::common::as_string(object, "name", "case"),
                     tyr::common::suite_path(suite, tyr::common::as_string(object, "domain_file", "case")),
                     tyr::common::suite_path(suite, tyr::common::as_string(object, "task_file", "case")),
                     max_arity,
                     expected_status,
                     expected_maximum_effective_width };
}

std::vector<SiwCase> load_cases()
{
    const auto suite = tyr::common::load_json_file(tyr::common::root_path() / "tests/unit/planning/algorithms/siw.json");
    const auto& suite_object = tyr::common::as_object(suite, "suite");
    const auto* cases_value = suite_object.if_contains("cases");
    if (!cases_value)
        throw std::runtime_error("suite.cases is required.");

    auto result = std::vector<SiwCase> {};
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

class SiwTest : public ::testing::TestWithParam<SiwCase>
{
};

TEST_P(SiwTest, MatchesExpectedOutcome)
{
    const auto& param = GetParam();
    auto context = create_ground_context(param.domain_file, param.task_file);
    auto brfs_solver = p::brfs::Solver<p::GroundTag> { context.task, context.successor_generator, p::brfs::Options<p::GroundTag> {} };
    brfs_solver.options.event_handler = p::brfs::DefaultEventHandler<p::GroundTag>::create();

    auto iw_solver = p::iw::Solver<p::GroundTag> { std::move(brfs_solver), param.max_arity, p::iw::Options<p::GroundTag> {} };
    const auto event_handler = p::siw::DefaultEventHandler<p::GroundTag>::create();

    auto options = p::siw::Options<p::GroundTag> {};
    options.event_handler = event_handler;

    const auto result = p::siw::find_solution(iw_solver, options);

    const auto plan_length = result.plan ? std::optional<uint_t>(result.plan->get_length()) : std::nullopt;
    const auto maximum_effective_width = event_handler->get_statistics().get_maximum_effective_width();
    const auto average_effective_width = event_handler->get_statistics().get_average_effective_width();
    fmt::println("SIW_OBSERVED {} {} {} {} {}",
                 param.name,
                 to_string(result.status),
                 plan_length ? std::to_string(*plan_length) : std::string("null"),
                 maximum_effective_width ? std::to_string(*maximum_effective_width) : std::string("null"),
                 average_effective_width ? std::to_string(*average_effective_width) : std::string("null"));

    if (param.expected_status)
    {
        EXPECT_EQ(result.status, *param.expected_status);
    }
    if (param.expected_maximum_effective_width)
    {
        ASSERT_TRUE(maximum_effective_width);
        EXPECT_EQ(*maximum_effective_width, *param.expected_maximum_effective_width);
    }
}

INSTANTIATE_TEST_SUITE_P(TyrPlanningSiw, SiwTest, ::testing::ValuesIn(load_cases()), [](const testing::TestParamInfo<SiwCase>& info) { return info.param.name; });

}
