

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

#ifndef TYR_PLANNING_GROUND_TASK_MATCH_TREE_CANONICALIZATION_HPP_
#define TYR_PLANNING_GROUND_TASK_MATCH_TREE_CANONICALIZATION_HPP_

#include "tyr/common/canonicalization.hpp"
#include "tyr/common/comparators.hpp"
#include "tyr/planning/ground_task/match_tree/nodes/atom_data.hpp"
#include "tyr/planning/ground_task/match_tree/nodes/constraint_data.hpp"
#include "tyr/planning/ground_task/match_tree/nodes/generator_data.hpp"
#include "tyr/planning/ground_task/match_tree/nodes/node_data.hpp"
#include "tyr/planning/ground_task/match_tree/nodes/variable_data.hpp"

#include <algorithm>

namespace tyr::planning::match_tree
{

template<typename Tag>
bool is_canonical(const Data<AtomSelectorNode<Tag>>& element)
{
    return true;
};

template<typename Tag>
bool is_canonical(const Data<VariableSelectorNode<Tag>>& element)
{
    return true;
};

template<typename Tag>
bool is_canonical(const Data<NegativeFactSelectorNode<Tag>>& element)
{
    return true;
};

template<typename Tag>
bool is_canonical(const Data<NumericConstraintSelectorNode<Tag>>& element)
{
    return true;
};

template<typename Tag>
bool is_canonical(const Data<ElementGeneratorNode<Tag>>& element)
{
    return true;
};

template<typename Tag>
bool is_canonical(const Data<Node<Tag>>& element)
{
    return true;
};

template<typename Tag>
void canonicalize(Data<AtomSelectorNode<Tag>>& element) {};

template<typename Tag>
void canonicalize(Data<VariableSelectorNode<Tag>>& element) {};

template<typename Tag>
void canonicalize(Data<NegativeFactSelectorNode<Tag>>& element) {};

template<typename Tag>
void canonicalize(Data<NumericConstraintSelectorNode<Tag>>& element) {};

template<typename Tag>
void canonicalize(Data<ElementGeneratorNode<Tag>>& element) {};

template<typename Tag>
void canonicalize(Data<Node<Tag>>& element) {};

}

#endif