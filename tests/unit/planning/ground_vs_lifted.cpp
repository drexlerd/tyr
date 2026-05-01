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
#include <string>
#include <tyr/formalism/formalism.hpp>
#include <tyr/planning/planning.hpp>

namespace p = tyr::planning;
namespace f = tyr::formalism;
namespace fp = tyr::formalism::planning;

namespace tyr::tests
{
namespace
{
/// @brief Compare statistics that must be the same for both configurations
/// Notes:
/// - plan length may differ because two plans of different length may have same optimal cost.
/// - number of generated states may differ because of f value improvements due to differences in labeled successor node orderings.
struct SearchSummary
{
    std::optional<uint64_t> expanded_last_snapshot;
    p::SearchStatus status;
    float_t plan_cost;

    bool operator==(const SearchSummary& other) const
    {
        return expanded_last_snapshot == other.expanded_last_snapshot && status == other.status && f::apply(f::OpEq {}, plan_cost, other.plan_cost);
    }
};

struct GroundVsLiftedCase
{
    std::string name;
    std::string domain_file;
    std::string task_file;
};

GroundVsLiftedCase parse_case(const boost::json::object& object)
{
    return GroundVsLiftedCase { tyr::common::as_string(object, "name", "case"),
                                tyr::common::as_string(object, "domain_file", "case"),
                                tyr::common::as_string(object, "task_file", "case") };
}

std::vector<GroundVsLiftedCase> load_cases()
{
    const auto suite = tyr::common::load_json_file(tyr::common::root_path() / "tests/unit/planning/ground_vs_lifted.json");
    const auto& suite_object = tyr::common::as_object(suite, "suite");
    const auto* cases_value = suite_object.if_contains("cases");
    if (!cases_value)
        throw std::runtime_error("suite.cases is required.");

    auto result = std::vector<GroundVsLiftedCase> {};
    for (const auto& case_value : tyr::common::as_array(*cases_value, "suite.cases"))
        result.push_back(parse_case(tyr::common::as_object(case_value, "case")));
    return result;
}

template<p::TaskKind Kind>
SearchSummary run_blind_astar(const fs::path& domain_filepath, const fs::path& problem_filepath)
{
    using TaskPtr = std::shared_ptr<p::Task<Kind>>;

    auto execution_context = ExecutionContext::create(1);

    TaskPtr task;
    if constexpr (std::same_as<Kind, p::GroundTag>)
        task = p::LiftedTask(fp::Parser(domain_filepath).parse_task(problem_filepath)).instantiate_ground_task(*execution_context).task;
    else if constexpr (std::same_as<Kind, p::LiftedTag>)
        task = p::LiftedTask::create(fp::Parser(domain_filepath).parse_task(problem_filepath));
    else
        static_assert(tyr::dependent_false<Kind>::value, "Missing case");

    auto successor_generator = p::SuccessorGenerator<Kind>(task, execution_context);
    auto heuristic = p::BlindHeuristic<Kind>::create();
    auto event_handler = p::astar_eager::DefaultEventHandler<Kind>::create(0);

    auto options = p::astar_eager::Options<Kind>();
    options.event_handler = event_handler;
    const auto result = p::astar_eager::find_solution(*task, successor_generator, *heuristic, options);

    SearchSummary summary {};

    if (!event_handler->get_progress_statistics().get_snapshots().empty())
    {
        const auto& last = event_handler->get_progress_statistics().get_snapshots().back();
        summary.expanded_last_snapshot = last.get_num_expanded();
    }

    summary.status = result.status;
    if (result.plan.has_value())
    {
        summary.plan_cost = result.plan->get_cost();
    }

    return summary;
}
}

class GroundVsLiftedTest : public ::testing::TestWithParam<GroundVsLiftedCase>
{
};

TEST_P(GroundVsLiftedTest, BlindAStarMatches)
{
    const auto& param = GetParam();
    const auto domain = tyr::common::root_path() / param.domain_file;
    const auto problem = tyr::common::root_path() / param.task_file;

    const auto lifted = run_blind_astar<p::LiftedTag>(domain, problem);
    const auto grounded = run_blind_astar<p::GroundTag>(domain, problem);

    EXPECT_EQ(lifted, grounded);
}

INSTANTIATE_TEST_SUITE_P(TyrTests,
                         GroundVsLiftedTest,
                         ::testing::ValuesIn(load_cases()),
                         [](const testing::TestParamInfo<GroundVsLiftedCase>& info) { return info.param.name; });
}
