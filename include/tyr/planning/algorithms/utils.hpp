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

#ifndef TYR_PLANNING_ALGORITHMS_UTILS_HPP_
#define TYR_PLANNING_ALGORITHMS_UTILS_HPP_

#include "tyr/common/config.hpp"
#include "tyr/planning/node.hpp"
#include "tyr/planning/plan.hpp"

#include <optional>
#include <stdexcept>

namespace tyr::planning
{

enum class SearchStatus
{
    IN_PROGRESS,
    OUT_OF_TIME,
    OUT_OF_MEMORY,
    OUT_OF_STATES,
    FAILED,
    EXHAUSTED,
    SOLVED,
    UNSOLVABLE
};

enum class ActionCostMode
{
    UNIT,
    GENERAL
};

inline float_t compute_successor_g_value(float_t source_g_value, float_t generated_successor_g_value, ActionCostMode mode)
{
    switch (mode)
    {
        case ActionCostMode::UNIT: return FloatTolerance<float_t>::canonicalize(source_g_value + 1);
        case ActionCostMode::GENERAL: return FloatTolerance<float_t>::canonicalize(generated_successor_g_value);
    }

    throw std::runtime_error("compute_successor_g_value(...): unknown action cost mode.");
}

template<TaskKind Kind>
struct SearchResult
{
    SearchStatus status = SearchStatus::IN_PROGRESS;
    std::optional<Plan<Kind>> plan = std::nullopt;
    std::optional<Node<Kind>> goal_node = std::nullopt;
};

}

#endif
