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

#ifndef TYR_PLANNING_HEURISTICS_BLIND_HPP_
#define TYR_PLANNING_HEURISTICS_BLIND_HPP_

#include "tyr/planning/heuristic.hpp"

namespace tyr::planning
{

template<typename Task>
class BlindHeuristic : public Heuristic<Task>
{
public:
    BlindHeuristic() = default;

    static std::shared_ptr<BlindHeuristic> create() { return std::make_shared<BlindHeuristic>(); }

    void set_goal(View<Index<formalism::planning::GroundConjunctiveCondition>, formalism::planning::Repository> goal) override {}

    float_t evaluate(const State<Task>& state) override { return float_t { 0 }; }
};

}

#endif
