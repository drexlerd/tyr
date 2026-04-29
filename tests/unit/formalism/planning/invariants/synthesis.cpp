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

#include "tyr/formalism/formalism.hpp"
#include "tyr/common/json_loader.hpp"

#include <boost/json.hpp>

#include <gtest/gtest.h>

namespace json = boost::json;

namespace f = tyr::formalism;
namespace fp = tyr::formalism::planning;
namespace fpi = tyr::formalism::planning::invariant;

namespace tyr::tests
{

namespace
{
auto compute_lifted_task(const fs::path& domain_filepath, const fs::path& problem_filepath) { return fp::Parser(domain_filepath).parse_task(problem_filepath); }

fs::path absolute(const std::string& filepath) { return fs::path(std::string(ROOT_DIR)) / filepath; }

const json::object& as_object(const json::value& value, std::string_view context)
{
    if (!value.is_object())
        throw std::runtime_error(std::string(context) + " must be an object.");
    return value.as_object();
}

const json::array& as_array(const json::value& value, std::string_view context)
{
    if (!value.is_array())
        throw std::runtime_error(std::string(context) + " must be an array.");
    return value.as_array();
}

std::string as_string(const json::object& object, std::string_view key, std::string_view context)
{
    const auto* value = object.if_contains(key);
    if (!value || !value->is_string())
        throw std::runtime_error(std::string(context) + "." + std::string(key) + " must be a string.");
    return std::string(value->as_string());
}

size_t as_size(const json::object& object, std::string_view key, std::string_view context)
{
    const auto* value = object.if_contains(key);
    if (!value || !value->is_int64() || value->as_int64() < 0)
        throw std::runtime_error(std::string(context) + "." + std::string(key) + " must be a non-negative integer.");
    return static_cast<size_t>(value->as_int64());
}

std::vector<int> parse_parameters(const json::object& atom_object)
{
    const auto* value = atom_object.if_contains("parameters");
    if (!value)
        throw std::runtime_error("atom.parameters is required.");

    auto result = std::vector<int> {};
    for (const auto& parameter : as_array(*value, "atom.parameters"))
    {
        if (!parameter.is_int64() || parameter.as_int64() < 0)
            throw std::runtime_error("atom.parameters entries must be non-negative integers.");
        result.push_back(static_cast<int>(parameter.as_int64()));
    }
    return result;
}

auto atom(fp::Repository& repo, std::string_view predicate_name, const std::vector<int>& parameters)
{
    auto predicate_builder = Data<f::Predicate<f::FluentTag>> { std::string(predicate_name), uint_t(parameters.size()) };
    const auto predicate = repo.get_or_create(predicate_builder).first;

    auto terms = std::vector<Data<f::Term>> {};
    for (const auto i : parameters)
        terms.push_back(Data<f::Term>(f::ParameterIndex(i)));

    return fp::MutableAtom<f::FluentTag>(predicate, terms);
}

fpi::Invariant parse_invariant(fp::Repository& repository, const json::object& invariant_object)
{
    auto atoms = std::vector<fp::MutableAtom<f::FluentTag>> {};
    const auto* atoms_value = invariant_object.if_contains("atoms");
    if (!atoms_value)
        throw std::runtime_error("invariant.atoms is required.");

    for (const auto& atom_value : as_array(*atoms_value, "invariant.atoms"))
    {
        const auto& atom_object = as_object(atom_value, "atom");
        atoms.push_back(atom(repository, as_string(atom_object, "predicate", "atom"), parse_parameters(atom_object)));
    }

    return fpi::Invariant(as_size(invariant_object, "num_rigid_variables", "invariant"),
                          as_size(invariant_object, "num_counted_variables", "invariant"),
                          std::move(atoms));
}

std::vector<fpi::Invariant> parse_invariants(fp::Repository& repository, const json::object& case_object)
{
    const auto* invariants_value = case_object.if_contains("invariants");
    if (!invariants_value)
        throw std::runtime_error("case.invariants is required.");

    auto result = std::vector<fpi::Invariant> {};
    for (const auto& invariant_value : as_array(*invariants_value, "case.invariants"))
        result.push_back(parse_invariant(repository, as_object(invariant_value, "invariant")));
    return result;
}

void expect_invariant_sets_equal(std::vector<fpi::Invariant> actual, std::vector<fpi::Invariant> expected)
{
    EXPECT_EQ(actual.size(), expected.size());
    EXPECT_TRUE(std::is_permutation(actual.begin(), actual.end(), expected.begin(), expected.end()));
}
}

TEST(TyrTests, TyrFormalismPlanningInvariantsSynthesis)
{
    const auto suite = tyr::common::load_json_file(absolute("tests/unit/formalism/planning/invariants/synthesis.json"));
    const auto& suite_object = as_object(suite, "suite");
    const auto* cases_value = suite_object.if_contains("cases");
    ASSERT_TRUE(cases_value);

    for (const auto& case_value : as_array(*cases_value, "suite.cases"))
    {
        const auto& case_object = as_object(case_value, "case");
        const auto name = as_string(case_object, "name", "case");

        SCOPED_TRACE(name);

        auto lifted_task = compute_lifted_task(absolute(as_string(case_object, "domain_file", "case")), absolute(as_string(case_object, "task_file", "case")));
        auto& repository = *lifted_task.get_repository();

        auto actual = fpi::synthesize_invariants(lifted_task.get_task().get_domain());
        auto expected = parse_invariants(repository, case_object);

        expect_invariant_sets_equal(actual, expected);
    }
}

}
