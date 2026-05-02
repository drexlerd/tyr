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
    virtual bool is_dynamic_goal_satisfied(const StateView<Kind>& state) = 0;
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
    bool is_dynamic_goal_satisfied(const StateView<Kind>& state) override
    {
        const auto state_context = StateContext { *state.get_state_repository()->get_task(), state.get_unpacked_state(), float_t { 0 } };
        return is_dynamically_applicable(m_goal, state_context);
    }

private:
    formalism::planning::GroundConjunctiveConditionView m_goal;
};
}

#endif
