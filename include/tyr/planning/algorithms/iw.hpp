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

#ifndef TYR_PLANNING_ALGORITHMS_IW_HPP_
#define TYR_PLANNING_ALGORITHMS_IW_HPP_

#include "tyr/planning/algorithms/brfs.hpp"
#include "tyr/planning/algorithms/iw/event_handler.hpp"
#include "tyr/planning/algorithms/iw/novelty_table.hpp"
#include "tyr/planning/algorithms/iw/pruning_strategy.hpp"
#include "tyr/planning/algorithms/iw/statistics.hpp"
#include "tyr/planning/algorithms/iw/utils.hpp"
#include "tyr/planning/algorithms/utils.hpp"
#include "tyr/planning/declarations.hpp"

#include <memory>
#include <optional>
#include <vector>

namespace tyr::planning::iw
{

template<TaskKind Kind>
struct Options
{
    std::optional<Node<Kind>> start_node = std::nullopt;
    EventHandlerPtr<Kind> event_handler = nullptr;
    GoalStrategyPtr<Kind> goal_strategy = nullptr;
    uint_t max_num_states = std::numeric_limits<uint_t>::max();
    std::optional<std::chrono::steady_clock::duration> max_time = std::nullopt;
    uint64_t random_seed = 0;
    bool shuffle_labeled_succ_nodes = false;

    Options() = default;
};

template<TaskKind Kind>
SearchResult<Kind> find_solution(brfs::Solver<Kind>& brfs_solver, uint_t max_arity, const Options<Kind>& options = Options<Kind>());

/// @brief Adapter that exposes IW search through the generic solver interface.
template<TaskKind Kind>
struct Solver
{
    brfs::Solver<Kind> brfs_solver;
    uint_t max_arity;
    Options<Kind> options;

    SearchResult<Kind> solve() { return find_solution(brfs_solver, max_arity, options); }
};

}

#endif
