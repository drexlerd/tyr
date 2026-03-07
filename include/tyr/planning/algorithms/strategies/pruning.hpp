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

#ifndef TYR_PLANNING_ALGORITHMS_STRATEGIES_PRUNING_HPP_
#define TYR_PLANNING_ALGORITHMS_STRATEGIES_PRUNING_HPP_

#include "tyr/planning/declarations.hpp"
#include "tyr/planning/state.hpp"

#include <memory>

namespace tyr::planning
{

template<typename Task>
class PruningStrategy
{
public:
    virtual ~PruningStrategy() = default;

    static std::shared_ptr<PruningStrategy<Task>> create() { return std::make_shared<PruningStrategy<Task>>(); }

    virtual bool should_prune_state(const State<Task>& state) { return false; }

    virtual bool should_prune_successor_state(const State<Task>& state, const State<Task>& succ_state, bool is_new_succ) { return false; }
};

}

#endif
