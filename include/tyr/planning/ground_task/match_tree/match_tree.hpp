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

#ifndef TYR_PLANNING_GROUND_TASK_MATCH_TREE_MATCH_TREE_HPP_
#define TYR_PLANNING_GROUND_TASK_MATCH_TREE_MATCH_TREE_HPP_

#include "tyr/common/types.hpp"
#include "tyr/formalism/planning/declarations.hpp"
#include "tyr/formalism/planning/repository.hpp"
#include "tyr/planning/declarations.hpp"
#include "tyr/planning/ground_task/match_tree/declarations.hpp"
#include "tyr/planning/ground_task/match_tree/nodes/node_data.hpp"

#include <optional>
#include <vector>

namespace tyr::planning::match_tree
{

template<typename Tag>
class MatchTree
{
private:
    IndexList<Tag> m_elements;

    RepositoryPtr<Tag> m_context;

    std::optional<Data<Node<Tag>>> m_root;

    std::vector<Data<Node<Tag>>> m_evaluate_stack;  ///< temporary during evaluation.

public:
    MatchTree(IndexList<Tag> elements, const formalism::planning::Repository& context);
    ~MatchTree();

    static MatchTreePtr<Tag> create(IndexList<Tag> elements, const formalism::planning::Repository& context);

    // Uncopieable and unmoveable to prohibit invalidating spans on m_elements.
    MatchTree(const MatchTree& other) = delete;
    MatchTree& operator=(const MatchTree& other) = delete;
    MatchTree(MatchTree&& other) = delete;
    MatchTree& operator=(MatchTree&& other) = delete;

    void generate(const StateContext<GroundTag>& state, IndexList<Tag>& out_applicable_elements);
};

}

#endif
