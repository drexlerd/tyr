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

#ifndef TYR_PLANNING_GROUND_TASK_MATCH_TREE_NODES_GENERATOR_INDEX_HPP_
#define TYR_PLANNING_GROUND_TASK_MATCH_TREE_NODES_GENERATOR_INDEX_HPP_

#include "tyr/common/index_mixins.hpp"
#include "tyr/common/types.hpp"
#include "tyr/planning/ground_task/match_tree/declarations.hpp"

namespace tyr
{
template<typename Tag>
struct Index<planning::match_tree::ElementGeneratorNode<Tag>> : IndexMixin<Index<planning::match_tree::ElementGeneratorNode<Tag>>>
{
    // Inherit constructors
    using Base = IndexMixin<Index<planning::match_tree::ElementGeneratorNode<Tag>>>;
    using Base::Base;
};

}

#endif
