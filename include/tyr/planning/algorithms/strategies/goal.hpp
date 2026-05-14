/*
 * Copyright (C) 2023 Dominik Drexler and Simon Stahlberg
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

#ifndef TYR_PLANNING_ALGORITHMS_STRATEGIES_GOAL_HPP_
#define TYR_PLANNING_ALGORITHMS_STRATEGIES_GOAL_HPP_

#include "tyr/planning/applicability.hpp"
#include "tyr/planning/declarations.hpp"
#include "tyr/planning/state_view.hpp"

#include <memory>

namespace tyr::planning
{

template<TaskKind Kind>
class GoalStrategy
{
public:
    virtual ~GoalStrategy() = default;

    virtual bool is_static_goal_satisfied(const Task<Kind>& task) = 0;
    virtual bool is_dynamic_goal_satisfied(const StateView<Kind>& seed_state, const StateView<Kind>& state) = 0;
};

template<TaskKind Kind>
class ConjunctiveGoalStrategy : public GoalStrategy<Kind>
{
public:
    ConjunctiveGoalStrategy(const Task<Kind>& task) : m_goal(task.get_task().get_goal()) {}
    ConjunctiveGoalStrategy(formalism::planning::GroundConjunctiveConditionView goal) : m_goal(goal) {}

    void set_goal(formalism::planning::GroundConjunctiveConditionView goal) { m_goal = goal; }

    static std::shared_ptr<ConjunctiveGoalStrategy<Kind>> create(const Task<Kind>& task) { return std::make_shared<ConjunctiveGoalStrategy<Kind>>(task); }
    static std::shared_ptr<ConjunctiveGoalStrategy<Kind>> create(formalism::planning::GroundConjunctiveConditionView goal)
    {
        return std::make_shared<ConjunctiveGoalStrategy<Kind>>(goal);
    }

    bool is_static_goal_satisfied(const Task<Kind>& task) override { return is_statically_applicable(m_goal, task.get_static_atoms_bitset()); }
    bool is_dynamic_goal_satisfied(const StateView<Kind>& seed_state, const StateView<Kind>& state) override
    {
        static_cast<void>(seed_state);
        const auto state_context = StateContext { *state.get_state_repository()->get_task(), state.get_unpacked_state(), float_t { 0 } };
        return is_dynamically_applicable(m_goal, state_context);
    }

private:
    formalism::planning::GroundConjunctiveConditionView m_goal;
};

template<TaskKind Kind>
class SerializedGoalStrategy : public GoalStrategy<Kind>
{
public:
    SerializedGoalStrategy(const Task<Kind>& task) : m_goal(task.get_task().get_goal()) {}
    SerializedGoalStrategy(formalism::planning::GroundConjunctiveConditionView goal) : m_goal(goal) {}

    void clear() noexcept {}

    static std::shared_ptr<SerializedGoalStrategy<Kind>> create(const Task<Kind>& task) { return std::make_shared<SerializedGoalStrategy<Kind>>(task); }
    static std::shared_ptr<SerializedGoalStrategy<Kind>> create(formalism::planning::GroundConjunctiveConditionView goal)
    {
        return std::make_shared<SerializedGoalStrategy<Kind>>(goal);
    }

    bool is_static_goal_satisfied(const Task<Kind>& task) override { return is_statically_applicable(m_goal, task.get_static_atoms_bitset()); }

    bool is_dynamic_goal_satisfied(const StateView<Kind>& seed_state, const StateView<Kind>& state) override
    {
        return count_satisfied_goals(state) > count_satisfied_goals(seed_state);
    }

private:
    uint_t count_satisfied_goals(const StateView<Kind>& state) const
    {
        const auto state_context = StateContext { *state.get_state_repository()->get_task(), state.get_unpacked_state(), float_t { 0 } };
        auto result = uint_t { 0 };

        for (auto literal : m_goal.template get_literals<formalism::StaticTag>())
            result += is_applicable(literal, state_context) ? 1 : 0;
        for (auto literal : m_goal.template get_literals<formalism::DerivedTag>())
            result += is_applicable(literal, state_context) ? 1 : 0;
        for (auto fact : m_goal.template get_facts<formalism::PositiveTag>())
            result += is_applicable<formalism::PositiveTag>(fact, state_context) ? 1 : 0;
        for (auto fact : m_goal.template get_facts<formalism::NegativeTag>())
            result += is_applicable<formalism::NegativeTag>(fact, state_context) ? 1 : 0;
        for (auto numeric_constraint : m_goal.get_numeric_constraints())
            result += is_applicable(numeric_constraint, state_context) ? 1 : 0;

        return result;
    }

    formalism::planning::GroundConjunctiveConditionView m_goal;
};

template<TaskKind Kind>
class ExhaustiveGoalStrategy : public GoalStrategy<Kind>
{
public:
    static std::shared_ptr<ExhaustiveGoalStrategy<Kind>> create() { return std::make_shared<ExhaustiveGoalStrategy<Kind>>(); }

    bool is_static_goal_satisfied(const Task<Kind>& task) override
    {
        static_cast<void>(task);
        return true;
    }

    bool is_dynamic_goal_satisfied(const StateView<Kind>& seed_state, const StateView<Kind>& state) override
    {
        static_cast<void>(seed_state);
        static_cast<void>(state);
        return false;
    }
};

}

#endif
