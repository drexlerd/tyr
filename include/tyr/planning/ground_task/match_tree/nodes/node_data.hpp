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

#ifndef TYR_PLANNING_GROUND_TASK_MATCH_TREE_NODES_NODE_DATA_HPP_
#define TYR_PLANNING_GROUND_TASK_MATCH_TREE_NODES_NODE_DATA_HPP_

#include "tyr/common/types.hpp"
#include "tyr/common/types_utils.hpp"
#include "tyr/common/variant.hpp"
#include "tyr/planning/ground_task/match_tree/declarations.hpp"
#include "tyr/planning/ground_task/match_tree/nodes/atom_index.hpp"
#include "tyr/planning/ground_task/match_tree/nodes/constraint_index.hpp"
#include "tyr/planning/ground_task/match_tree/nodes/generator_index.hpp"
#include "tyr/planning/ground_task/match_tree/nodes/negative_fact_index.hpp"
#include "tyr/planning/ground_task/match_tree/nodes/variable_index.hpp"

namespace tyr
{
template<typename Tag>
struct Data<planning::match_tree::Node<Tag>>
{
    using Variant = ::cista::offset::variant<Index<planning::match_tree::AtomSelectorNode<Tag>>,
                                             Index<planning::match_tree::NumericConstraintSelectorNode<Tag>>,
                                             Index<planning::match_tree::VariableSelectorNode<Tag>>,
                                             Index<planning::match_tree::NegativeFactSelectorNode<Tag>>,
                                             Index<planning::match_tree::ElementGeneratorNode<Tag>>>;

    Variant value;

    Data() = default;
    Data(Variant value) : value(value) {}

    void clear() noexcept { tyr::clear(value); }

    auto cista_members() const noexcept { return std::tie(value); }
    auto identifying_members() const noexcept { return std::tie(value); }
};

}

#endif
