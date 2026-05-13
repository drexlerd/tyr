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
#include <gtest/gtest.h>
#include <ostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <tyr/formalism/formalism.hpp>

namespace f = tyr::formalism;
namespace fp = tyr::formalism::planning;

namespace tyr::tests
{
namespace
{
struct ParserSymbolIndexStabilityCase
{
    std::string name;
    std::filesystem::path domain_file;
    std::filesystem::path task_file;
};

void PrintTo(const ParserSymbolIndexStabilityCase& test_case, std::ostream* os)
{
    *os << test_case.name << " (" << test_case.task_file << ")";
}

ParserSymbolIndexStabilityCase parse_case(const boost::json::object& suite, const boost::json::object& object)
{
    return ParserSymbolIndexStabilityCase { tyr::common::as_string(object, "name", "case"),
                                            tyr::common::suite_path(suite, tyr::common::as_string(object, "domain_file", "case")),
                                            tyr::common::suite_path(suite, tyr::common::as_string(object, "task_file", "case")) };
}

std::vector<ParserSymbolIndexStabilityCase> load_cases()
{
    const auto suite =
        tyr::common::load_json_file(tyr::common::root_path() / "tests/unit/formalism/planning/parser_symbol_index_stability.json");
    const auto& suite_object = tyr::common::as_object(suite, "suite");
    const auto* cases_value = suite_object.if_contains("cases");
    if (!cases_value)
        throw std::runtime_error("suite.cases is required.");

    auto result = std::vector<ParserSymbolIndexStabilityCase> {};
    for (const auto& case_value : tyr::common::as_array(*cases_value, "suite.cases"))
        result.push_back(parse_case(suite_object, tyr::common::as_object(case_value, "case")));
    return result;
}

fp::PlanningTask parse_task(const ParserSymbolIndexStabilityCase& test_case)
{
    return fp::Parser(test_case.domain_file).parse_task(test_case.task_file);
}

template<typename Range>
void expect_same_index_names(const Range& first, const Range& second, std::string_view label)
{
    ASSERT_EQ(first.size(), second.size()) << label;

    for (size_t i = 0; i < first.size(); ++i)
    {
        const auto first_symbol = first[i];
        const auto second_symbol = second[i];

        EXPECT_EQ(first_symbol.get_index(), second_symbol.get_index()) << label << "[" << i << "]";
        EXPECT_EQ(first_symbol.get_name(), second_symbol.get_name()) << label << "[" << i << "] index " << first_symbol.get_index().get_value();
    }
}

void expect_same_symbol_index_names(const fp::PlanningTask& first, const fp::PlanningTask& second)
{
    const auto first_domain = first.get_domain().get_domain();
    const auto second_domain = second.get_domain().get_domain();
    const auto first_task = first.get_task();
    const auto second_task = second.get_task();

    expect_same_index_names(first_domain.get_constants(), second_domain.get_constants(), "domain constants");
    expect_same_index_names(first_task.get_objects(), second_task.get_objects(), "task objects");

    expect_same_index_names(first_domain.get_predicates<f::StaticTag>(), second_domain.get_predicates<f::StaticTag>(), "static predicates");
    expect_same_index_names(first_domain.get_predicates<f::FluentTag>(), second_domain.get_predicates<f::FluentTag>(), "fluent predicates");
    expect_same_index_names(first_domain.get_predicates<f::DerivedTag>(), second_domain.get_predicates<f::DerivedTag>(), "derived predicates");

    expect_same_index_names(first_domain.get_functions<f::StaticTag>(), second_domain.get_functions<f::StaticTag>(), "static functions");
    expect_same_index_names(first_domain.get_functions<f::FluentTag>(), second_domain.get_functions<f::FluentTag>(), "fluent functions");
}
}

class ParserSymbolIndexStabilityTest : public ::testing::TestWithParam<ParserSymbolIndexStabilityCase>
{
};

TEST_P(ParserSymbolIndexStabilityTest, ReparsingKeepsSymbolNamesAtSameIndexes)
{
    const auto& param = GetParam();
    SCOPED_TRACE(param.task_file.string());

    const auto first_task = parse_task(param);
    const auto second_task = parse_task(param);

    expect_same_symbol_index_names(first_task, second_task);
}

INSTANTIATE_TEST_SUITE_P(TyrFormalismPlanningParser,
                         ParserSymbolIndexStabilityTest,
                         ::testing::ValuesIn(load_cases()),
                         [](const testing::TestParamInfo<ParserSymbolIndexStabilityCase>& info) { return info.param.name; });
}
