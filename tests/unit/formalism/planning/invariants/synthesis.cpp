/*
 * Copyright (C) 2025 Dominik Drexler
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

#include <gtest/gtest.h>

namespace b = tyr::buffer;
namespace f = tyr::formalism;
namespace fp = tyr::formalism::planning;
namespace fpi = tyr::formalism::planning::invariant;

namespace tyr::tests
{

namespace
{
auto compute_lifted_task(const fs::path& domain_filepath, const fs::path& problem_filepath) { return fp::Parser(domain_filepath).parse_task(problem_filepath); }

fs::path absolute(const std::string& subdir) { return fs::path(std::string(DATA_DIR)) / subdir; }

inline std::vector<fpi::Invariant> sort_invariants(std::vector<fpi::Invariant> invariants)
{
    std::sort(invariants.begin(), invariants.end());
    return invariants;
}

inline void expect_invariant_sets_equal(std::vector<fpi::Invariant> actual, std::vector<fpi::Invariant> expected)
{
    EXPECT_EQ(actual.size(), expected.size());
    EXPECT_TRUE(std::is_permutation(actual.begin(), actual.end(), expected.begin(), expected.end()));
}

auto atom(fp::Repository& repo, std::string_view predicate_name, const std::vector<int>& parameters)
{
    auto predicate_builder = Data<f::Predicate<f::FluentTag>> { std::string(predicate_name), uint_t(parameters.size()) };
    const auto predicate = repo.get_or_create(predicate_builder).first;

    auto terms = std::vector<Data<f::Term>> {};
    for (const auto i : parameters)
        terms.push_back(Data<f::Term>(f::ParameterIndex(i)));

    return fpi::TempAtom { .predicate = predicate, .terms = terms };
}

auto inv(size_t num_rigid_variables, size_t num_counted_variables, std::initializer_list<fpi::TempAtom> atoms)
{
    return fpi::Invariant(num_rigid_variables, num_counted_variables, std::vector<fpi::TempAtom>(atoms));
}
}

/// @brief
/// Ground truth from Fast Downward:
///
/// <Invariant {current_stage((-1,)) [omitted_pos = 0]}>,
/// <Invariant {current_worker((-1,)) [omitted_pos = 0]}>,
/// <Invariant {max_worker((-1,)) [omitted_pos = 0]}>,
/// <Invariant {num_food((-1,)) [omitted_pos = 0]}>,
/// <Invariant {space_rooms((0,)) [omitted_pos = None]}>,
/// <Invariant {space_rooms((-1,)) [omitted_pos = 0]}>,
/// <Invariant {built_rooms((0, -1)) [omitted_pos = 1], space_rooms((0,)) [omitted_pos = None]}>
TEST(TyrTests, TyrFormalismPlanningInvariantsSynthesisAgricola)
{
    auto lifted_task = compute_lifted_task(absolute("agricola/domain.pddl"), absolute("agricola/test_problem.pddl"));
    auto& repository = *lifted_task.get_repository();

    auto actual = fpi::synthesize_invariants(lifted_task.get_task().get_domain());

    auto expected = std::vector<fpi::Invariant> {
        inv(0,
            1,
            {
                atom(repository, "current_stage", { 0 }),
            }),
        inv(0,
            1,
            {
                atom(repository, "current_worker", { 0 }),
            }),
        inv(0,
            1,
            {
                atom(repository, "max_worker", { 0 }),
            }),
        inv(0,
            1,
            {
                atom(repository, "num_food", { 0 }),
            }),
        inv(1,
            0,
            {
                atom(repository, "space_rooms", { 0 }),
            }),
        inv(0,
            1,
            {
                atom(repository, "space_rooms", { 0 }),
            }),
        inv(1,
            1,
            {
                atom(repository, "built_rooms", { 0, 1 }),
                atom(repository, "space_rooms", { 0 }),
            }),
    };

    expect_invariant_sets_equal(actual, expected);
}

/// @brief
/// Ground truth from Fast Downward:
///
/// <Invariant {at-segment((0, -1)) [omitted_pos = 1]}>,
/// <Invariant {facing((0, -1)) [omitted_pos = 1]}>,
/// <Invariant {is-pushing((0,)) [omitted_pos = None]}>,
/// <Invariant {is-pushing((-1,)) [omitted_pos = 0]}>,
/// <Invariant {airborne((0, -1)) [omitted_pos = 1], at-segment((0, -1)) [omitted_pos = 1]}>,
/// <Invariant {is-moving((0,)) [omitted_pos = None], is-pushing((0,)) [omitted_pos = None]}>,
/// <Invariant {is-moving((-1,)) [omitted_pos = 0], is-pushing((-1,)) [omitted_pos = 0]}>,
/// <Invariant {is-moving((0,)) [omitted_pos = None], is-parked((0, -1)) [omitted_pos = 1], is-pushing((0,)) [omitted_pos = None]}>]
TEST(TyrTests, TyrFormalismPlanningInvariantsSynthesisAirport)
{
    auto lifted_task = compute_lifted_task(absolute("airport/domain.pddl"), absolute("airport/test_problem.pddl"));
    auto& repository = *lifted_task.get_repository();

    auto actual = fpi::synthesize_invariants(lifted_task.get_task().get_domain());

    auto expected = std::vector<fpi::Invariant> {
        inv(1,
            1,
            {
                atom(repository, "at-segment", { 0, 1 }),
            }),
        inv(1,
            1,
            {
                atom(repository, "facing", { 0, 1 }),
            }),
        inv(1,
            0,
            {
                atom(repository, "is-pushing", { 0 }),
            }),
        inv(0,
            1,
            {
                atom(repository, "is-pushing", { 0 }),
            }),
        inv(1,
            1,
            {
                atom(repository, "airborne", { 0, 1 }),
                atom(repository, "at-segment", { 0, 1 }),
            }),
        inv(1,
            0,
            {
                atom(repository, "is-moving", { 0 }),
                atom(repository, "is-pushing", { 0 }),
            }),
        inv(0,
            1,
            {
                atom(repository, "is-moving", { 0 }),
                atom(repository, "is-pushing", { 0 }),
            }),
        inv(1,
            1,
            {
                atom(repository, "is-moving", { 0 }),
                atom(repository, "is-parked", { 0, 1 }),
                atom(repository, "is-pushing", { 0 }),
            }),
    };

    expect_invariant_sets_equal(actual, expected);
}

/// @brief
/// Ground truth from Fast Downward:
///
/// No invariants.
TEST(TyrTests, TyrFormalismPlanningInvariantsSynthesisAssembly)
{
    auto lifted_task = compute_lifted_task(absolute("assembly/domain.pddl"), absolute("assembly/test_problem.pddl"));

    auto actual = fpi::synthesize_invariants(lifted_task.get_task().get_domain());

    auto expected = std::vector<fpi::Invariant> {

    };

    expect_invariant_sets_equal(actual, expected);
}

/// @brief
/// Ground truth from Fast Downward:
///
/// <Invariant {shaker-level((0, -1)) [omitted_pos = 1]}>,
/// <Invariant {holding((-1, 0)) [omitted_pos = 0], ontable((0,)) [omitted_pos = None]}>,
/// <Invariant {handempty((0,)) [omitted_pos = None], holding((0, -1)) [omitted_pos = 1]}>
TEST(TyrTests, TyrFormalismPlanningInvariantsSynthesisBarman)
{
    auto lifted_task = compute_lifted_task(absolute("barman/domain.pddl"), absolute("barman/test_problem.pddl"));
    auto& repository = *lifted_task.get_repository();

    auto actual = fpi::synthesize_invariants(lifted_task.get_task().get_domain());

    auto expected = std::vector<fpi::Invariant> {
        inv(1,
            1,
            {
                atom(repository, "shaker-level", { 0, 1 }),
            }),
        inv(1,
            1,
            {
                atom(repository, "holding", { 1, 0 }),
                atom(repository, "ontable", { 0 }),
            }),
        inv(1,
            1,
            {
                atom(repository, "handempty", { 0 }),
                atom(repository, "holding", { 0, 1 }),
            }),
    };

    expect_invariant_sets_equal(actual, expected);
}

/// @brief
/// Ground truth from Fast Downward:
///
/// <Invariant {clear((0,)) [omitted_pos = None], on((-1, 0)) [omitted_pos = 0]}>,
/// <Invariant {on((0, -1)) [omitted_pos = 1], on-table((0,)) [omitted_pos = None]}>
TEST(TyrTests, TyrFormalismPlanningInvariantsSynthesisBlocks3)
{
    auto lifted_task = compute_lifted_task(absolute("blocks_3/domain.pddl"), absolute("blocks_3/test_problem.pddl"));
    auto& repository = *lifted_task.get_repository();

    auto actual = fpi::synthesize_invariants(lifted_task.get_task().get_domain());

    auto expected = std::vector<fpi::Invariant> {
        inv(1,
            1,
            {
                atom(repository, "clear", { 0 }),
                atom(repository, "on", { 1, 0 }),
            }),
        inv(1,
            1,
            {
                atom(repository, "on", { 0, 1 }),
                atom(repository, "on-table", { 0 }),
            }),
    };

    expect_invariant_sets_equal(actual, expected);
}

/// @brief
/// Ground truth from Fast Downward:
///
/// <Invariant {arm-empty(()) [omitted_pos = None], holding((-1,)) [omitted_pos = 0]}>,
/// <Invariant {clear((0,)) [omitted_pos = None], holding((0,)) [omitted_pos = None], on((-1, 0)) [omitted_pos = 0]}>,
/// <Invariant {holding((0,)) [omitted_pos = None], on((0, -1)) [omitted_pos = 1], on-table((0,)) [omitted_pos = None]}>
TEST(TyrTests, TyrFormalismPlanningInvariantsSynthesisBlocks4)
{
    auto lifted_task = compute_lifted_task(absolute("blocks_4/domain.pddl"), absolute("blocks_4/test_problem.pddl"));
    auto& repository = *lifted_task.get_repository();

    auto actual = fpi::synthesize_invariants(lifted_task.get_task().get_domain());

    auto expected = std::vector<fpi::Invariant> {
        inv(0,
            1,
            {
                atom(repository, "arm-empty", {}),
                atom(repository, "holding", { 0 }),
            }),
        inv(1,
            1,
            {
                atom(repository, "clear", { 0 }),
                atom(repository, "holding", { 0 }),
                atom(repository, "on", { 1, 0 }),
            }),
        inv(1,
            1,
            {
                atom(repository, "holding", { 0 }),
                atom(repository, "on", { 0, 1 }),
                atom(repository, "on-table", { 0 }),
            }),
    };

    expect_invariant_sets_equal(actual, expected);
}

/// @brief
/// Ground truth from Fast Downward:
///
/// <Invariant {at_kitchen_bread((0,)) [omitted_pos = None]}>,
/// <Invariant {at_kitchen_bread((-1,)) [omitted_pos = 0]}>,
/// <Invariant {at_kitchen_content((0,)) [omitted_pos = None]}>,
/// <Invariant {at_kitchen_content((-1,)) [omitted_pos = 0]}>,
/// <Invariant {at((0, -1)) [omitted_pos = 1]}>,
/// <Invariant {notexist((0,)) [omitted_pos = None]}>,
/// <Invariant {notexist((-1,)) [omitted_pos = 0]}>,
/// <Invariant {at_kitchen_sandwich((0,)) [omitted_pos = None], notexist((0,)) [omitted_pos = None]}>,
/// <Invariant {at_kitchen_bread((-1,)) [omitted_pos = 0], at_kitchen_sandwich((-1,)) [omitted_pos = 0]}>,
/// <Invariant {at_kitchen_content((-1,)) [omitted_pos = 0], at_kitchen_sandwich((-1,)) [omitted_pos = 0]}>,
/// <Invariant {at_kitchen_sandwich((-1,)) [omitted_pos = 0], notexist((-1,)) [omitted_pos = 0]}>,
/// <Invariant {no_gluten_sandwich((0,)) [omitted_pos = None], notexist((0,)) [omitted_pos = None]}>,
/// <Invariant {at_kitchen_bread((-1,)) [omitted_pos = 0], no_gluten_sandwich((-1,)) [omitted_pos = 0]}>,
/// <Invariant {at_kitchen_content((-1,)) [omitted_pos = 0], no_gluten_sandwich((-1,)) [omitted_pos = 0]}>,
/// <Invariant {no_gluten_sandwich((-1,)) [omitted_pos = 0], notexist((-1,)) [omitted_pos = 0]}>,
/// <Invariant {at_kitchen_sandwich((0,)) [omitted_pos = None], notexist((0,)) [omitted_pos = None], ontray((0, -1)) [omitted_pos = 1]}>
TEST(TyrTests, TyrFormalismPlanningInvariantsSynthesisChildsnack)
{
    auto lifted_task = compute_lifted_task(absolute("childsnack/domain.pddl"), absolute("childsnack/test_problem.pddl"));
    auto& repository = *lifted_task.get_repository();

    auto actual = fpi::synthesize_invariants(lifted_task.get_task().get_domain());

    auto expected = std::vector<fpi::Invariant> {
        inv(1,
            0,
            {
                atom(repository, "at_kitchen_bread", { 0 }),
            }),
        inv(0,
            1,
            {
                atom(repository, "at_kitchen_bread", { 0 }),
            }),
        inv(1,
            0,
            {
                atom(repository, "at_kitchen_content", { 0 }),
            }),
        inv(0,
            1,
            {
                atom(repository, "at_kitchen_content", { 0 }),
            }),
        inv(1,
            1,
            {
                atom(repository, "at", { 0, 1 }),
            }),
        inv(1,
            0,
            {
                atom(repository, "notexist", { 0 }),
            }),
        inv(0,
            1,
            {
                atom(repository, "notexist", { 0 }),
            }),
        inv(1,
            0,
            {
                atom(repository, "at_kitchen_sandwich", { 0 }),
                atom(repository, "notexist", { 0 }),
            }),
        inv(0,
            1,
            {
                atom(repository, "at_kitchen_bread", { 0 }),
                atom(repository, "at_kitchen_sandwich", { 0 }),
            }),
        inv(0,
            1,
            {
                atom(repository, "at_kitchen_content", { 0 }),
                atom(repository, "at_kitchen_sandwich", { 0 }),
            }),
        inv(0,
            1,
            {
                atom(repository, "at_kitchen_sandwich", { 0 }),
                atom(repository, "notexist", { 0 }),
            }),
        inv(1,
            0,
            {
                atom(repository, "no_gluten_sandwich", { 0 }),
                atom(repository, "notexist", { 0 }),
            }),
        inv(0,
            1,
            {
                atom(repository, "at_kitchen_bread", { 0 }),
                atom(repository, "no_gluten_sandwich", { 0 }),
            }),
        inv(0,
            1,
            {
                atom(repository, "at_kitchen_content", { 0 }),
                atom(repository, "no_gluten_sandwich", { 0 }),
            }),
        inv(0,
            1,
            {
                atom(repository, "no_gluten_sandwich", { 0 }),
                atom(repository, "notexist", { 0 }),
            }),
        inv(1,
            1,
            {
                atom(repository, "at_kitchen_sandwich", { 0 }),
                atom(repository, "notexist", { 0 }),
                atom(repository, "ontray", { 0, 1 }),
            }),
    };

    expect_invariant_sets_equal(actual, expected);
}

/// @brief
/// Ground truth from Fast Downward:
///
/// <Invariant {at((0, -1)) [omitted_pos = 1], carrying((-1, 0)) [omitted_pos = 0]}>,
/// <Invariant {carrying((0, -1)) [omitted_pos = 1], empty((0,)) [omitted_pos = None]}>]
TEST(TyrTests, TyrFormalismPlanningInvariantsSynthesisDelivery)
{
    auto lifted_task = compute_lifted_task(absolute("delivery/domain.pddl"), absolute("delivery/test_problem.pddl"));
    auto& repository = *lifted_task.get_repository();

    auto actual = fpi::synthesize_invariants(lifted_task.get_task().get_domain());

    auto expected = std::vector<fpi::Invariant> {
        inv(1,
            1,
            {
                atom(repository, "at", { 0, 1 }),
                atom(repository, "carrying", { 1, 0 }),
            }),
        inv(1,
            1,
            {
                atom(repository, "carrying", { 0, 1 }),
                atom(repository, "empty", { 0 }),
            }),
    };

    expect_invariant_sets_equal(actual, expected);
}

/// @brief
/// Ground truth from Fast Downward:
///
/// <Invariant {driving((-1, 0)) [omitted_pos = 0], empty((0,)) [omitted_pos = None]}>,
/// <Invariant {at((0, -1)) [omitted_pos = 1], driving((0, -1)) [omitted_pos = 1], in((0, -1)) [omitted_pos = 1]}>
TEST(TyrTests, TyrFormalismPlanningInvariantsSynthesisDriverlog)
{
    auto lifted_task = compute_lifted_task(absolute("driverlog/domain.pddl"), absolute("driverlog/test_problem.pddl"));
    auto& repository = *lifted_task.get_repository();

    auto actual = fpi::synthesize_invariants(lifted_task.get_task().get_domain());

    auto expected = std::vector<fpi::Invariant> {
        inv(1,
            1,
            {
                atom(repository, "driving", { 1, 0 }),
                atom(repository, "empty", { 0 }),
            }),
        inv(1,
            1,
            {
                atom(repository, "at", { 0, 1 }),
                atom(repository, "driving", { 0, 1 }),
                atom(repository, "in", { 0, 1 }),
            }),
    };

    expect_invariant_sets_equal(actual, expected);
}

/// @brief
/// Ground truth from Fast Downward:
///
/// <Invariant {at-ferry((-1,)) [omitted_pos = 0]}>,
/// <Invariant {at((0, -1)) [omitted_pos = 1], on((0,)) [omitted_pos = None]}>,
/// <Invariant {empty-ferry(()) [omitted_pos = None], on((-1,)) [omitted_pos = 0]}>
TEST(TyrTests, TyrFormalismPlanningInvariantsSynthesisFerry)
{
    auto lifted_task = compute_lifted_task(absolute("ferry/domain.pddl"), absolute("ferry/test_problem.pddl"));
    auto& repository = *lifted_task.get_repository();

    auto actual = fpi::synthesize_invariants(lifted_task.get_task().get_domain());

    auto expected = std::vector<fpi::Invariant> {
        inv(0,
            1,
            {
                atom(repository, "at-ferry", { 0 }),
            }),
        inv(1,
            1,
            {
                atom(repository, "at", { 0, 1 }),
                atom(repository, "on", { 0 }),
            }),
        inv(0,
            1,
            {
                atom(repository, "empty-ferry", {}),
                atom(repository, "on", { 0 }),
            }),
    };

    expect_invariant_sets_equal(actual, expected);
}

/// @brief
/// No ground truth.
///
/// No invariants.
TEST(TyrTests, TyrFormalismPlanningInvariantsSynthesisFoCounters)
{
    auto lifted_task = compute_lifted_task(absolute("fo-counters/domain.pddl"), absolute("fo-counters/test_problem.pddl"));

    auto actual = fpi::synthesize_invariants(lifted_task.get_task().get_domain());

    auto expected = std::vector<fpi::Invariant> {};

    expect_invariant_sets_equal(actual, expected);
}

/// @brief
/// Ground truth from Fast Downward:
///
/// <Invariant {at-robot((-1,)) [omitted_pos = 0]}>,
/// <Invariant {locked((0,)) [omitted_pos = None]}>,
/// <Invariant {locked((-1,)) [omitted_pos = 0]}>,
/// <Invariant {at((0, -1)) [omitted_pos = 1], holding((0,)) [omitted_pos = None]}>,
/// <Invariant {arm-empty(()) [omitted_pos = None], holding((-1,)) [omitted_pos = 0]}>,
/// <Invariant {locked((0,)) [omitted_pos = None], open((0,)) [omitted_pos = None]}>,
/// <Invariant {locked((-1,)) [omitted_pos = 0], open((-1,)) [omitted_pos = 0]}>
TEST(TyrTests, TyrFormalismPlanningInvariantsSynthesisGrid)
{
    auto lifted_task = compute_lifted_task(absolute("grid/domain.pddl"), absolute("grid/test_problem.pddl"));
    auto& repository = *lifted_task.get_repository();

    auto actual = fpi::synthesize_invariants(lifted_task.get_task().get_domain());

    auto expected = std::vector<fpi::Invariant> {
        inv(0,
            1,
            {
                atom(repository, "at-robot", { 0 }),
            }),
        inv(1,
            0,
            {
                atom(repository, "locked", { 0 }),
            }),
        inv(0,
            1,
            {
                atom(repository, "locked", { 0 }),
            }),
        inv(1,
            1,
            {
                atom(repository, "at", { 0, 1 }),
                atom(repository, "holding", { 0 }),
            }),
        inv(0,
            1,
            {
                atom(repository, "arm-empty", {}),
                atom(repository, "holding", { 0 }),
            }),
        inv(1,
            0,
            {
                atom(repository, "locked", { 0 }),
                atom(repository, "open", { 0 }),
            }),
        inv(0,
            1,
            {
                atom(repository, "locked", { 0 }),
                atom(repository, "open", { 0 }),
            }),
    };

    expect_invariant_sets_equal(actual, expected);
}

/// @brief
/// Ground truth from Fast Downward:
///
/// <Invariant {at-robby((-1,)) [omitted_pos = 0]}>,
/// <Invariant {at((0, -1)) [omitted_pos = 1], carry((0, -1)) [omitted_pos = 1]}>,
/// <Invariant {carry((-1, 0)) [omitted_pos = 0], free((0,)) [omitted_pos = None]}>
TEST(TyrTests, TyrFormalismPlanningInvariantsSynthesisGripper)
{
    auto lifted_task = compute_lifted_task(absolute("gripper/domain.pddl"), absolute("gripper/test_problem.pddl"));
    auto& repository = *lifted_task.get_repository();

    auto actual = fpi::synthesize_invariants(lifted_task.get_task().get_domain());

    auto expected = std::vector<fpi::Invariant> {
        inv(0,
            1,
            {
                atom(repository, "at-robby", { 0 }),
            }),
        inv(1,
            1,
            {
                atom(repository, "at", { 0, 1 }),
                atom(repository, "carry", { 0, 1 }),
            }),
        inv(1,
            1,
            {
                atom(repository, "carry", { 1, 0 }),
                atom(repository, "free", { 0 }),
            }),
    };

    expect_invariant_sets_equal(actual, expected);
}

/// @brief
/// Ground truth from Fast Downward:
///
/// <Invariant {at_tent((0, -1)) [omitted_pos = 1]}>,
/// <Invariant {at_person((0, -1)) [omitted_pos = 1]}>,
/// <Invariant {at_car((0, -1)) [omitted_pos = 1]}>,
/// <Invariant {walked((0, -1)) [omitted_pos = 1]}>,
/// <Invariant {down((0,)) [omitted_pos = None], up((0,)) [omitted_pos = None]}>,
/// <Invariant {down((-1,)) [omitted_pos = 0], up((-1,)) [omitted_pos = 0]}>
TEST(TyrTests, TyrFormalismPlanningInvariantsSynthesisHiking)
{
    auto lifted_task = compute_lifted_task(absolute("hiking/domain.pddl"), absolute("hiking/test_problem.pddl"));
    auto& repository = *lifted_task.get_repository();

    auto actual = fpi::synthesize_invariants(lifted_task.get_task().get_domain());

    auto expected = std::vector<fpi::Invariant> {
        inv(1,
            1,
            {
                atom(repository, "at_tent", { 0, 1 }),
            }),
        inv(1,
            1,
            {
                atom(repository, "at_person", { 0, 1 }),
            }),
        inv(1,
            1,
            {
                atom(repository, "at_car", { 0, 1 }),
            }),
        inv(1,
            1,
            {
                atom(repository, "walked", { 0, 1 }),
            }),
        inv(1,
            0,
            {
                atom(repository, "down", { 0 }),
                atom(repository, "up", { 0 }),
            }),
        inv(0,
            1,
            {
                atom(repository, "down", { 0 }),
                atom(repository, "up", { 0 }),
            }),
    };

    expect_invariant_sets_equal(actual, expected);
}

/// @brief
/// Ground truth from Fast Downward:
///
/// <Invariant {at((0, -1)) [omitted_pos = 1], in((0, -1)) [omitted_pos = 1]}>
TEST(TyrTests, TyrFormalismPlanningInvariantsSynthesisLogistics)
{
    auto lifted_task = compute_lifted_task(absolute("logistics/domain.pddl"), absolute("logistics/test_problem.pddl"));
    auto& repository = *lifted_task.get_repository();

    auto actual = fpi::synthesize_invariants(lifted_task.get_task().get_domain());

    auto expected = std::vector<fpi::Invariant> {
        inv(1,
            1,
            {
                atom(repository, "at", { 0, 1 }),
                atom(repository, "in", { 0, 1 }),
            }),
    };

    expect_invariant_sets_equal(actual, expected);
}

/// @brief
/// Ground truth from Fast Downward:
///
/// <Invariant {origin((0, 1)) [omitted_pos = None]}>,
/// <Invariant {origin((-1, 0)) [omitted_pos = 0]}>,
/// <Invariant {origin((0, -1)) [omitted_pos = 1]}>,
/// <Invariant {lift-at((-1,)) [omitted_pos = 0]}>,
/// <Invariant {boarded((0,)) [omitted_pos = None], origin((0, -1)) [omitted_pos = 1]}>,
/// <Invariant {boarded((0,)) [omitted_pos = None], origin((0, -1)) [omitted_pos = 1], served((0,)) [omitted_pos = None]}>
TEST(TyrTests, TyrFormalismPlanningInvariantsSynthesisMiconic)
{
    auto lifted_task = compute_lifted_task(absolute("miconic/domain.pddl"), absolute("miconic/test_problem.pddl"));
    auto& repository = *lifted_task.get_repository();

    auto actual = fpi::synthesize_invariants(lifted_task.get_task().get_domain());

    auto expected = std::vector<fpi::Invariant> {
        inv(2,
            0,
            {
                atom(repository, "origin", { 0, 1 }),
            }),
        inv(1,
            1,
            {
                atom(repository, "origin", { 1, 0 }),
            }),
        inv(1,
            1,
            {
                atom(repository, "origin", { 0, 1 }),
            }),
        inv(0,
            1,
            {
                atom(repository, "lift-at", { 0 }),
            }),
        inv(1,
            1,
            {
                atom(repository, "boarded", { 0 }),
                atom(repository, "origin", { 0, 1 }),
            }),
        inv(1,
            1,
            {
                atom(repository, "boarded", { 0 }),
                atom(repository, "origin", { 0, 1 }),
                atom(repository, "served", { 0 }),
            }),
    };

    expect_invariant_sets_equal(actual, expected);
}

/// @brief
/// Ground truth from Fast Downward:
///
/// <Invariant {lift-at((-1,)) [omitted_pos = 0]}>
TEST(TyrTests, TyrFormalismPlanningInvariantsSynthesisMiconicFulladl)
{
    auto lifted_task = compute_lifted_task(absolute("miconic-fulladl/domain.pddl"), absolute("miconic-fulladl/test_problem.pddl"));
    auto& repository = *lifted_task.get_repository();

    auto actual = fpi::synthesize_invariants(lifted_task.get_task().get_domain());

    auto expected = std::vector<fpi::Invariant> {
        inv(0,
            1,
            {
                atom(repository, "lift-at", { 0 }),
            }),
    };

    expect_invariant_sets_equal(actual, expected);
}

/// @brief
/// Ground truth from Fast Downward:
///
/// <Invariant {lift-at((-1,)) [omitted_pos = 0]}>
TEST(TyrTests, TyrFormalismPlanningInvariantsSynthesisMiconicSimpleadl)
{
    auto lifted_task = compute_lifted_task(absolute("miconic-simpleadl/domain.pddl"), absolute("miconic-simpleadl/test_problem.pddl"));
    auto& repository = *lifted_task.get_repository();

    auto actual = fpi::synthesize_invariants(lifted_task.get_task().get_domain());

    auto expected = std::vector<fpi::Invariant> {
        inv(0,
            1,
            {
                atom(repository, "lift-at", { 0 }),
            }),
    };

    expect_invariant_sets_equal(actual, expected);
}

/// @brief
/// Ground truth from Fast Downward:
///
/// <Invariant {location((0, -1)) [omitted_pos = 1]}>,
/// <Invariant {notprintedwith((0, 1, 2)) [omitted_pos = None]}>,
/// <Invariant {notprintedwith((-1, 0, 1)) [omitted_pos = 0]}>,
/// <Invariant {notprintedwith((0, -1, 1)) [omitted_pos = 1]}>,
/// <Invariant {notprintedwith((0, 1, -1)) [omitted_pos = 2]}>,
/// <Invariant {uninitialized(()) [omitted_pos = None]}>,
/// <Invariant {hasimage((0, 1, -1)) [omitted_pos = 2], notprintedwith((0, 1, -1)) [omitted_pos = 2]}>
TEST(TyrTests, TyrFormalismPlanningInvariantsSynthesisParcprinter)
{
    auto lifted_task = compute_lifted_task(absolute("parcprinter/domain.pddl"), absolute("parcprinter/test_problem.pddl"));
    auto& repository = *lifted_task.get_repository();

    auto actual = fpi::synthesize_invariants(lifted_task.get_task().get_domain());

    auto expected = std::vector<fpi::Invariant> {
        inv(1,
            1,
            {
                atom(repository, "location", { 0, 1 }),
            }),
        inv(3,
            0,
            {
                atom(repository, "notprintedwith", { 0, 1, 2 }),
            }),
        inv(2,
            1,
            {
                atom(repository, "notprintedwith", { 2, 0, 1 }),
            }),
        inv(2,
            1,
            {
                atom(repository, "notprintedwith", { 0, 2, 1 }),
            }),
        inv(2,
            1,
            {
                atom(repository, "notprintedwith", { 0, 1, 2 }),
            }),
        inv(0,
            0,
            {
                atom(repository, "uninitialized", {}),
            }),
        inv(2,
            1,
            {
                atom(repository, "hasimage", { 0, 1, 2 }),
                atom(repository, "notprintedwith", { 0, 1, 2 }),
            }),
    };

    expect_invariant_sets_equal(actual, expected);
}

/// @brief
/// Ground truth from Fast Downward:
///
/// <Invariant {num-subs((-1,)) [omitted_pos = 0]}>
TEST(TyrTests, TyrFormalismPlanningInvariantsSynthesisPathways)
{
    auto lifted_task = compute_lifted_task(absolute("pathways/domain.pddl"), absolute("pathways/test_problem.pddl"));
    auto& repository = *lifted_task.get_repository();

    auto actual = fpi::synthesize_invariants(lifted_task.get_task().get_domain());

    auto expected = std::vector<fpi::Invariant> {
        inv(0,
            1,
            {
                atom(repository, "num-subs", { 0 }),
            }),
    };

    expect_invariant_sets_equal(actual, expected);
}

/// @brief
/// Ground truth from Fast Downward:
///
/// <Invariant {at-process((0, -1)) [omitted_pos = 1]}>,
/// <Invariant {queue-head((0, -1)) [omitted_pos = 1]}>,
/// <Invariant {queue-tail((0, -1)) [omitted_pos = 1]}>,
/// <Invariant {queue-size((0, -1)) [omitted_pos = 1]}>,
/// <Invariant {activate((0, -1)) [omitted_pos = 1], enabled((0, -1)) [omitted_pos = 1], pending((0,)) [omitted_pos = None]}>,
/// <Invariant {advance-head((0,)) [omitted_pos = None], advance-tail((0,)) [omitted_pos = None], settled((0,)) [omitted_pos = None]}>,
/// <Invariant {advance-head((-1,)) [omitted_pos = 0], advance-tail((-1,)) [omitted_pos = 0], settled((-1,)) [omitted_pos = 0]}>
TEST(TyrTests, TyrFormalismPlanningInvariantsSynthesisPhilosophers)
{
    auto lifted_task = compute_lifted_task(absolute("philosophers/domain.pddl"), absolute("philosophers/test_problem.pddl"));
    auto& repository = *lifted_task.get_repository();

    auto actual = fpi::synthesize_invariants(lifted_task.get_task().get_domain());

    auto expected = std::vector<fpi::Invariant> {
        inv(1,
            1,
            {
                atom(repository, "at-process", { 0, 1 }),
            }),
        inv(1,
            1,
            {
                atom(repository, "queue-head", { 0, 1 }),
            }),
        inv(1,
            1,
            {
                atom(repository, "queue-tail", { 0, 1 }),
            }),
        inv(1,
            1,
            {
                atom(repository, "queue-size", { 0, 1 }),
            }),
        inv(1,
            1,
            {
                atom(repository, "activate", { 0, 1 }),
                atom(repository, "enabled", { 0, 1 }),
                atom(repository, "pending", { 0 }),
            }),
        inv(1,
            0,
            {
                atom(repository, "advance-head", { 0 }),
                atom(repository, "advance-tail", { 0 }),
                atom(repository, "settled", { 0 }),
            }),
        inv(0,
            1,
            {
                atom(repository, "advance-head", { 0 }),
                atom(repository, "advance-tail", { 0 }),
                atom(repository, "settled", { 0 }),
            }),
    };

    expect_invariant_sets_equal(actual, expected);
}

/// @brief
/// Ground truth from Fast Downward:
///
/// No Invariants.
TEST(TyrTests, TyrFormalismPlanningInvariantsSynthesisPsrMiddle)
{
    auto lifted_task = compute_lifted_task(absolute("psr-middle/domain.pddl"), absolute("psr-middle/test_problem.pddl"));

    auto actual = fpi::synthesize_invariants(lifted_task.get_task().get_domain());

    auto expected = std::vector<fpi::Invariant> {};

    expect_invariant_sets_equal(actual, expected);
}

/// @brief
/// Ground truth from Fast Downward:
///
/// <Invariant {at((0, -1)) [omitted_pos = 1]}>
TEST(TyrTests, TyrFormalismPlanningInvariantsSynthesisPushworld)
{
    auto lifted_task = compute_lifted_task(absolute("pushworld/domain.pddl"), absolute("pushworld/test_problem.pddl"));
    auto& repository = *lifted_task.get_repository();

    auto actual = fpi::synthesize_invariants(lifted_task.get_task().get_domain());

    auto expected = std::vector<fpi::Invariant> {
        inv(1,
            1,
            {
                atom(repository, "at", { 0, 1 }),
            }),
    };

    expect_invariant_sets_equal(actual, expected);
}

/// @brief
/// No ground truth.
///
/// No invariants.
TEST(TyrTests, TyrFormalismPlanningInvariantsSynthesisRefuel)
{
    auto lifted_task = compute_lifted_task(absolute("refuel/domain.pddl"), absolute("refuel/test_problem.pddl"));

    auto actual = fpi::synthesize_invariants(lifted_task.get_task().get_domain());

    auto expected = std::vector<fpi::Invariant> {};

    expect_invariant_sets_equal(actual, expected);
}

/// @brief
/// No ground truth.
TEST(TyrTests, TyrFormalismPlanningInvariantsSynthesisRefuelAdl)
{
    auto lifted_task = compute_lifted_task(absolute("refuel-adl/domain.pddl"), absolute("refuel-adl/test_problem.pddl"));
    auto& repository = *lifted_task.get_repository();

    auto actual = fpi::synthesize_invariants(lifted_task.get_task().get_domain());

    auto expected = std::vector<fpi::Invariant> {
        inv(1,
            1,
            {
                atom(repository, "at", { 0, 1 }),
            }),
    };

    expect_invariant_sets_equal(actual, expected);
}

/// @brief
/// Ground truth from Fast Downward:
///
/// <Invariant {at((-1,)) [omitted_pos = 0]}>,
/// <Invariant {reward((0,)) [omitted_pos = None]}>,
/// <Invariant {reward((-1,)) [omitted_pos = 0]}>,
/// <Invariant {picked((0,)) [omitted_pos = None], reward((0,)) [omitted_pos = None]}>,
/// <Invariant {picked((-1,)) [omitted_pos = 0], reward((-1,)) [omitted_pos = 0]}>
TEST(TyrTests, TyrFormalismPlanningInvariantsSynthesisReward)
{
    auto lifted_task = compute_lifted_task(absolute("reward/domain.pddl"), absolute("reward/test_problem.pddl"));
    auto& repository = *lifted_task.get_repository();

    auto actual = fpi::synthesize_invariants(lifted_task.get_task().get_domain());

    auto expected = std::vector<fpi::Invariant> {
        inv(0,
            1,
            {
                atom(repository, "at", { 0 }),
            }),
        inv(1,
            0,
            {
                atom(repository, "reward", { 0 }),
            }),
        inv(0,
            1,
            {
                atom(repository, "reward", { 0 }),
            }),
        inv(1,
            0,
            {
                atom(repository, "picked", { 0 }),
                atom(repository, "reward", { 0 }),
            }),
        inv(0,
            1,
            {
                atom(repository, "picked", { 0 }),
                atom(repository, "reward", { 0 }),
            }),
    };

    expect_invariant_sets_equal(actual, expected);
}

/// @brief
/// Ground truth from Fast Downward:
///
/// <Invariant {at((0, -1)) [omitted_pos = 1]}>,
/// <Invariant {at_soil_sample((0,)) [omitted_pos = None]}>,
/// <Invariant {at_soil_sample((-1,)) [omitted_pos = 0]}>,
/// <Invariant {at_rock_sample((0,)) [omitted_pos = None]}>,
/// <Invariant {at_rock_sample((-1,)) [omitted_pos = 0]}>,
/// <Invariant {empty((0,)) [omitted_pos = None], full((0,)) [omitted_pos = None]}>,
/// <Invariant {empty((-1,)) [omitted_pos = 0], full((-1,)) [omitted_pos = 0]}>,
/// <Invariant {at_rock_sample((0,)) [omitted_pos = None], have_rock_analysis((-1, 0)) [omitted_pos = 0]}>,
/// <Invariant {at_soil_sample((0,)) [omitted_pos = None], have_soil_analysis((-1, 0)) [omitted_pos = 0]}>,
/// <Invariant {at_soil_sample((-1,)) [omitted_pos = 0], empty((-1,)) [omitted_pos = 0], full((-1,)) [omitted_pos = 0]}>,
/// <Invariant {at_rock_sample((-1,)) [omitted_pos = 0], at_soil_sample((-1,)) [omitted_pos = 0], full((-1,)) [omitted_pos = 0]}>
///
/// Note: we find additional invariants where possible bindings to objects are disjoint because of typing, which we compiled away
/// Invariant(
//     num_rigid_variables = 0,
//     num_counted_variables = 1,
//     atoms = [available(V0), channel_free(V0)],
//     predicates = {channel_free/1, available/1},
// )
TEST(TyrTests, TyrFormalismPlanningInvariantsSynthesisRovers)
{
    auto lifted_task = compute_lifted_task(absolute("rovers/domain.pddl"), absolute("rovers/test_problem.pddl"));
    auto& repository = *lifted_task.get_repository();

    auto actual = fpi::synthesize_invariants(lifted_task.get_task().get_domain());

    auto expected = std::vector<fpi::Invariant> {
        inv(1,
            1,
            {
                atom(repository, "at", { 0, 1 }),
            }),
        inv(1,
            0,
            {
                atom(repository, "at_soil_sample", { 0 }),
            }),
        inv(0,
            1,
            {
                atom(repository, "at_soil_sample", { 0 }),
            }),
        inv(1,
            0,
            {
                atom(repository, "at_rock_sample", { 0 }),
            }),
        inv(0,
            1,
            {
                atom(repository, "at_rock_sample", { 0 }),
            }),
        inv(1,
            0,
            {
                atom(repository, "empty", { 0 }),
                atom(repository, "full", { 0 }),
            }),
        inv(0,
            1,
            {
                atom(repository, "empty", { 0 }),
                atom(repository, "full", { 0 }),
            }),
        inv(1,
            1,
            {
                atom(repository, "at_rock_sample", { 0 }),
                atom(repository, "have_rock_analysis", { 1, 0 }),
            }),
        inv(1,
            1,
            {
                atom(repository, "at_soil_sample", { 0 }),
                atom(repository, "have_soil_analysis", { 1, 0 }),
            }),
        inv(0,
            1,
            {
                atom(repository, "at_soil_sample", { 0 }),
                atom(repository, "empty", { 0 }),
                atom(repository, "full", { 0 }),
            }),
        inv(0,
            1,
            {
                atom(repository, "at_rock_sample", { 0 }),
                atom(repository, "at_soil_sample", { 0 }),
                atom(repository, "full", { 0 }),
            }),

        // Additional
        inv(0,
            1,
            {
                atom(repository, "available", { 0 }),
                atom(repository, "channel_free", { 0 }),
            }),
        inv(0,
            1,
            {
                atom(repository, "available", { 0 }),
                atom(repository, "channel_free", { 0 }),
                atom(repository, "communicated_rock_data", { 0 }),
            }),
        inv(0,
            1,
            {
                atom(repository, "available", { 0 }),
                atom(repository, "channel_free", { 0 }),
                atom(repository, "communicated_soil_data", { 0 }),
            }),
    };

    expect_invariant_sets_equal(actual, expected);
}

/// @brief
/// Ground truth from Fast Downward:
///
/// <Invariant {pointing((0, -1)) [omitted_pos = 1]}>,
/// <Invariant {power_avail((-1,)) [omitted_pos = 0], power_on((-1,)) [omitted_pos = 0]}>
TEST(TyrTests, TyrFormalismPlanningInvariantsSynthesisSatellite)
{
    auto lifted_task = compute_lifted_task(absolute("satellite/domain.pddl"), absolute("satellite/test_problem.pddl"));
    auto& repository = *lifted_task.get_repository();

    auto actual = fpi::synthesize_invariants(lifted_task.get_task().get_domain());

    auto expected = std::vector<fpi::Invariant> {
        inv(1,
            1,
            {
                atom(repository, "pointing", { 0, 1 }),
            }),
        inv(0,
            1,
            {
                atom(repository, "power_avail", { 0 }),
                atom(repository, "power_on", { 0 }),
            }),
    };

    expect_invariant_sets_equal(actual, expected);
}

/// @brief
/// Ground truth from Fast Downward:
///
/// No invariants.
TEST(TyrTests, TyrFormalismPlanningInvariantsSynthesisSchedule)
{
    auto lifted_task = compute_lifted_task(absolute("schedule/domain.pddl"), absolute("schedule/test_problem.pddl"));

    auto actual = fpi::synthesize_invariants(lifted_task.get_task().get_domain());

    auto expected = std::vector<fpi::Invariant> {};

    expect_invariant_sets_equal(actual, expected);
}

/// @brief
/// Ground truth from Fast Downward:
///
/// <Invariant {at-robot((-1,)) [omitted_pos = 0]}>,
/// <Invariant {at((0, -1)) [omitted_pos = 1]}>,
/// <Invariant {clear((-1,)) [omitted_pos = 0]}>,
/// <Invariant {at((-1, 0)) [omitted_pos = 0], clear((0,)) [omitted_pos = None]}>
TEST(TyrTests, TyrFormalismPlanningInvariantsSynthesisSokoban)
{
    auto lifted_task = compute_lifted_task(absolute("sokoban/domain.pddl"), absolute("sokoban/test_problem.pddl"));
    auto& repository = *lifted_task.get_repository();

    auto actual = fpi::synthesize_invariants(lifted_task.get_task().get_domain());

    auto expected = std::vector<fpi::Invariant> {
        inv(0,
            1,
            {
                atom(repository, "at-robot", { 0 }),
            }),
        inv(1,
            1,
            {
                atom(repository, "at", { 0, 1 }),
            }),
        inv(0,
            1,
            {
                atom(repository, "clear", { 0 }),
            }),
        inv(1,
            1,
            {
                atom(repository, "at", { 1, 0 }),
                atom(repository, "clear", { 0 }),
            }),
    };

    expect_invariant_sets_equal(actual, expected);
}

/// @brief
/// Ground truth from Fast Downward:
///
/// <Invariant {at((0, -1)) [omitted_pos = 1]}>,
/// <Invariant {useable((0,)) [omitted_pos = None]}>,
/// <Invariant {useable((-1,)) [omitted_pos = 0]}>,
/// <Invariant {loose((0,)) [omitted_pos = None]}>,
/// <Invariant {loose((-1,)) [omitted_pos = 0]}>,
/// <Invariant {at((0, -1)) [omitted_pos = 1], carrying((-1, 0)) [omitted_pos = 0]}>,
/// <Invariant {loose((0,)) [omitted_pos = None], tightened((0,)) [omitted_pos = None]}>,
/// <Invariant {loose((-1,)) [omitted_pos = 0], tightened((-1,)) [omitted_pos = 0]}>,
/// <Invariant {tightened((-1,)) [omitted_pos = 0], useable((-1,)) [omitted_pos = 0]}>
TEST(TyrTests, TyrFormalismPlanningInvariantsSynthesisSpanner)
{
    auto lifted_task = compute_lifted_task(absolute("spanner/domain.pddl"), absolute("spanner/test_problem.pddl"));
    auto& repository = *lifted_task.get_repository();

    auto actual = fpi::synthesize_invariants(lifted_task.get_task().get_domain());

    auto expected = std::vector<fpi::Invariant> {
        inv(1,
            1,
            {
                atom(repository, "at", { 0, 1 }),
            }),
        inv(1,
            0,
            {
                atom(repository, "useable", { 0 }),
            }),
        inv(0,
            1,
            {
                atom(repository, "useable", { 0 }),
            }),
        inv(1,
            0,
            {
                atom(repository, "loose", { 0 }),
            }),
        inv(0,
            1,
            {
                atom(repository, "loose", { 0 }),
            }),
        inv(1,
            1,
            {
                atom(repository, "at", { 0, 1 }),
                atom(repository, "carrying", { 1, 0 }),
            }),
        inv(1,
            0,
            {
                atom(repository, "loose", { 0 }),
                atom(repository, "tightened", { 0 }),
            }),
        inv(0,
            1,
            {
                atom(repository, "loose", { 0 }),
                atom(repository, "tightened", { 0 }),
            }),
        inv(0,
            1,
            {
                atom(repository, "tightened", { 0 }),
                atom(repository, "useable", { 0 }),
            }),
    };

    expect_invariant_sets_equal(actual, expected);
}

/// @brief
/// No ground truth.
///
TEST(TyrTests, TyrFormalismPlanningInvariantsSynthesisTpp)
{
    auto lifted_task = compute_lifted_task(absolute("tpp/numeric/domain.pddl"), absolute("tpp/numeric/test_problem.pddl"));
    auto& repository = *lifted_task.get_repository();

    auto actual = fpi::synthesize_invariants(lifted_task.get_task().get_domain());

    auto expected = std::vector<fpi::Invariant> {
        inv(1,
            1,
            {
                atom(repository, "loc", { 0, 1 }),
            }),
    };

    expect_invariant_sets_equal(actual, expected);
}

/// @brief
/// Ground truth from Fast Downward:
///
/// <Invariant {capacity((0, -1)) [omitted_pos = 1]}>,
/// <Invariant {at((0, -1)) [omitted_pos = 1], in((0, -1)) [omitted_pos = 1]}>
TEST(TyrTests, TyrFormalismPlanningInvariantsSynthesisTransport)
{
    auto lifted_task = compute_lifted_task(absolute("transport/domain.pddl"), absolute("transport/test_problem.pddl"));
    auto& repository = *lifted_task.get_repository();

    auto actual = fpi::synthesize_invariants(lifted_task.get_task().get_domain());

    auto expected = std::vector<fpi::Invariant> {
        inv(1,
            1,
            {
                atom(repository, "capacity", { 0, 1 }),
            }),
        inv(1,
            1,
            {
                atom(repository, "at", { 0, 1 }),
                atom(repository, "in", { 0, 1 }),
            }),
    };

    expect_invariant_sets_equal(actual, expected);
}

/// @brief
/// Ground truth from Fast Downward:
///
/// <Invariant {at-robot((-1,)) [omitted_pos = 0]}>
TEST(TyrTests, TyrFormalismPlanningInvariantsSynthesisVisitall)
{
    auto lifted_task = compute_lifted_task(absolute("visitall/domain.pddl"), absolute("visitall/test_problem.pddl"));
    auto& repository = *lifted_task.get_repository();

    auto actual = fpi::synthesize_invariants(lifted_task.get_task().get_domain());

    auto expected = std::vector<fpi::Invariant> {
        inv(0,
            1,
            {
                atom(repository, "at-robot", { 0 }),
            }),
    };

    expect_invariant_sets_equal(actual, expected);
}

/// @brief
/// Ground truth from Fast Downward:
///
/// <Invariant {unused((0,)) [omitted_pos = None]}>,
/// <Invariant {unused((-1,)) [omitted_pos = 0]}>,
/// <Invariant {surface-condition((0, -1)) [omitted_pos = 1], unused((0,)) [omitted_pos = None]}>,
/// <Invariant {treatment((0, -1)) [omitted_pos = 1], unused((0,)) [omitted_pos = None]}>,
/// <Invariant {unused((0,)) [omitted_pos = None], wood((0, -1)) [omitted_pos = 1]}>,
/// <Invariant {empty((0,)) [omitted_pos = None], in-highspeed-saw((-1, 0)) [omitted_pos = 0]}>,
/// <Invariant {available((0,)) [omitted_pos = None], in-highspeed-saw((0, -1)) [omitted_pos = 1], unused((0,)) [omitted_pos = None]}>
TEST(TyrTests, TyrFormalismPlanningInvariantsSynthesisWoodworking)
{
    auto lifted_task = compute_lifted_task(absolute("woodworking/domain.pddl"), absolute("woodworking/test_problem.pddl"));
    auto& repository = *lifted_task.get_repository();

    auto actual = fpi::synthesize_invariants(lifted_task.get_task().get_domain());

    auto expected = std::vector<fpi::Invariant> {
        inv(1,
            0,
            {
                atom(repository, "unused", { 0 }),
            }),
        inv(0,
            1,
            {
                atom(repository, "unused", { 0 }),
            }),
        inv(1,
            1,
            {
                atom(repository, "surface-condition", { 0, 1 }),
                atom(repository, "unused", { 0 }),
            }),
        inv(1,
            1,
            {
                atom(repository, "treatment", { 0, 1 }),
                atom(repository, "unused", { 0 }),
            }),
        inv(1,
            1,
            {
                atom(repository, "unused", { 0 }),
                atom(repository, "wood", { 0, 1 }),
            }),
        inv(1,
            1,
            {
                atom(repository, "empty", { 0 }),
                atom(repository, "in-highspeed-saw", { 1, 0 }),
            }),
        inv(1,
            1,
            {
                atom(repository, "available", { 0 }),
                atom(repository, "in-highspeed-saw", { 0, 1 }),
                atom(repository, "unused", { 0 }),
            }),
    };

    expect_invariant_sets_equal(actual, expected);
}

/// @brief
/// No ground truth.
///
TEST(TyrTests, TyrFormalismPlanningInvariantsSynthesisZenotravel)
{
    auto lifted_task = compute_lifted_task(absolute("zenotravel/numeric/domain.pddl"), absolute("zenotravel/numeric/test_problem.pddl"));
    auto& repository = *lifted_task.get_repository();

    auto actual = fpi::synthesize_invariants(lifted_task.get_task().get_domain());

    auto expected = std::vector<fpi::Invariant> {
        inv(1,
            1,
            {
                atom(repository, "located", { 0, 1 }),
                atom(repository, "in", { 0, 1 }),
            }),
    };

    expect_invariant_sets_equal(actual, expected);
}

}