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

#ifndef TYR_PLANNING_GROUND_TASK_MATCH_TREE_NODES_GENERATOR_DATA_HPP_
#define TYR_PLANNING_GROUND_TASK_MATCH_TREE_NODES_GENERATOR_DATA_HPP_

#include "tyr/common/types.hpp"
#include "tyr/common/types_utils.hpp"
#include "tyr/formalism/planning/ground_action_index.hpp"
#include "tyr/formalism/planning/ground_axiom_index.hpp"
#include "tyr/planning/ground_task/match_tree/declarations.hpp"
#include "tyr/planning/ground_task/match_tree/nodes/generator_index.hpp"
#include "tyr/planning/ground_task/match_tree/nodes/node_data.hpp"

namespace tyr
{
template<typename Tag>
struct Data<planning::match_tree::ElementGeneratorNode<Tag>>
{
    Index<planning::match_tree::ElementGeneratorNode<Tag>> index;
    IndexList<Tag> elements;

    Data() = default;
    Data(Index<planning::match_tree::ElementGeneratorNode<Tag>> index, IndexList<Tag> elements) : index(index), elements(std::move(elements)) {}
    Data(const Data& other) = delete;
    Data& operator=(const Data& other) = delete;
    Data(Data&& other) = default;
    Data& operator=(Data&& other) = default;

    void clear() noexcept
    {
        tyr::clear(index);
        tyr::clear(elements);
    }

    auto cista_members() const noexcept { return std::tie(index, elements); }
    auto identifying_members() const noexcept { return std::tie(elements); }
};
}

#endif
