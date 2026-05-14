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

#ifndef TYR_PLANNING_ALGORITHMS_SERIALIZED_HPP_
#define TYR_PLANNING_ALGORITHMS_SERIALIZED_HPP_

#include "tyr/planning/algorithms/concepts.hpp"
#include "tyr/planning/algorithms/serialized/event_handler.hpp"
#include "tyr/planning/algorithms/serialized/statistics.hpp"
#include "tyr/planning/algorithms/strategies/goal.hpp"
#include "tyr/planning/algorithms/utils.hpp"
#include "tyr/planning/declarations.hpp"
#include "tyr/planning/plan.hpp"

#include <memory>
#include <limits>
#include <optional>
#include <stdexcept>
#include <vector>

namespace tyr::planning::serialized
{

template<TaskKind Kind, SolverConcept<Kind> Subsolver>
struct Options
{
    std::optional<Node<Kind>> start_node = std::nullopt;
    EventHandlerPtr<Kind, Subsolver> event_handler = nullptr;
    GoalStrategyPtr<Kind> subgoal_strategy = nullptr;
    GoalStrategyPtr<Kind> goal_strategy = nullptr;
    uint_t max_num_subsearches = std::numeric_limits<uint_t>::max();

    Options() = default;
};

template<typename T, typename Kind>
concept SerializedSolverConcept = SolverConcept<T, Kind> && requires(T solver, std::optional<Node<Kind>> start_node, GoalStrategyPtr<Kind> goal_strategy) {
    typename T::EventHandlerType;
    typename T::EventHandlerType::StatisticsType;
    solver.options.start_node = start_node;
    solver.options.goal_strategy = goal_strategy;
    solver.options.event_handler->get_search_statistics();
    solver.options.event_handler->get_statistics();
};

namespace detail
{

template<TaskKind Kind>
SearchResult<Kind> make_empty_result(const Node<Kind>& start_node)
{
    auto result = SearchResult<Kind> {};
    result.status = SearchStatus::SOLVED;
    result.goal_node = start_node;
    result.plan = Plan<Kind>(start_node);
    return result;
}

template<TaskKind Kind>
void append_plan(const Plan<Kind>& subplan, Node<Kind>& current_node, LabeledNodeList<Kind>& labeled_succ_nodes)
{
    auto previous_metric = subplan.get_start_node().get_metric();
    auto cumulative_metric = current_node.get_metric();

    for (const auto& labeled_succ_node : subplan.get_labeled_succ_nodes())
    {
        const auto step_cost = labeled_succ_node.node.get_metric() - previous_metric;
        cumulative_metric = FloatTolerance<float_t>::canonicalize(cumulative_metric + step_cost);
        current_node = Node<Kind>(labeled_succ_node.node.get_state(), cumulative_metric);
        labeled_succ_nodes.emplace_back(labeled_succ_node.label, current_node);
        previous_metric = labeled_succ_node.node.get_metric();
    }
}

}

template<TaskKind Kind, SerializedSolverConcept<Kind> Solver>
SearchResult<Kind> find_solution(Solver& solver, const Options<Kind, Solver>& options = Options<Kind, Solver>())
{
    const auto event_handler = options.event_handler ? options.event_handler : DefaultEventHandler<Kind, Solver>::create();

    if (!options.subgoal_strategy)
        throw std::invalid_argument("serialized::find_solution(...): subgoal strategy is required.");
    if (!options.goal_strategy)
        throw std::invalid_argument("serialized::find_solution(...): goal strategy is required.");

    if (options.max_num_subsearches == 0)
    {
        if (!options.start_node && !solver.options.start_node)
            throw std::invalid_argument("serialized::find_solution(...): start node is required if max_num_subsearches is 0.");
        const auto result = detail::make_empty_result(options.start_node ? *options.start_node : *solver.options.start_node);
        event_handler->on_start_search();
        event_handler->on_solved(*result.plan);
        event_handler->on_end_search(result.status);
        return result;
    }

    event_handler->on_start_search();

    auto current_start_node = options.start_node ? options.start_node : solver.options.start_node;
    auto combined_start_node = std::optional<Node<Kind>> {};
    auto combined_labeled_succ_nodes = LabeledNodeList<Kind> {};

    if (current_start_node && options.goal_strategy->is_dynamic_goal_satisfied(current_start_node->get_state(), current_start_node->get_state()))
    {
        auto result = detail::make_empty_result(*current_start_node);
        event_handler->on_solved(*result.plan);
        event_handler->on_end_search(result.status);
        return result;
    }

    if (auto serialized_subgoal_strategy = std::dynamic_pointer_cast<SerializedGoalStrategy<Kind>>(options.subgoal_strategy))
        serialized_subgoal_strategy->clear();

    for (auto subsearch_index = uint_t { 0 }; subsearch_index < options.max_num_subsearches; ++subsearch_index)
    {
        auto local_solver = solver;
        local_solver.options.start_node = current_start_node;
        local_solver.options.goal_strategy = options.subgoal_strategy;

        event_handler->on_start_subsearch(subsearch_index);

        auto sub_result = local_solver.solve();

        if (!combined_start_node && sub_result.plan)
        {
            combined_start_node = sub_result.plan->get_start_node();
        }

        if (local_solver.options.event_handler)
            event_handler->add_subsearch_statistics(local_solver.options.event_handler->get_search_statistics(), local_solver.options.event_handler->get_statistics());

        event_handler->on_end_subsearch(subsearch_index, sub_result.status);

        if (sub_result.status != SearchStatus::SOLVED)
        {
            event_handler->on_end_search(sub_result.status);
            return sub_result;
        }

        if (!sub_result.plan || !sub_result.goal_node)
        {
            auto result = SearchResult<Kind> {};
            result.status = SearchStatus::FAILED;
            event_handler->on_end_search(result.status);
            return result;
        }

        auto current_node = combined_labeled_succ_nodes.empty() ? *combined_start_node : combined_labeled_succ_nodes.back().node;
        if (sub_result.plan->get_length() == 0)
        {
            auto result = SearchResult<Kind> {};
            result.status =
                (combined_start_node && options.goal_strategy->is_dynamic_goal_satisfied(combined_start_node->get_state(), current_node.get_state())) ?
                    SearchStatus::SOLVED :
                    SearchStatus::EXHAUSTED;
            if (combined_start_node)
            {
                result.plan = Plan<Kind>(*combined_start_node, std::move(combined_labeled_succ_nodes));
                result.goal_node = current_node;
            }

            if (result.plan && result.status == SearchStatus::SOLVED)
                event_handler->on_solved(*result.plan);
            event_handler->on_end_search(result.status);
            return result;
        }

        detail::append_plan(*sub_result.plan, current_node, combined_labeled_succ_nodes);
        current_start_node = Node<Kind>(sub_result.goal_node->get_state(), 0);

        if (combined_start_node && options.goal_strategy->is_dynamic_goal_satisfied(combined_start_node->get_state(), sub_result.goal_node->get_state()))
        {
            auto result = SearchResult<Kind> {};
            result.status = SearchStatus::SOLVED;
            result.plan = Plan<Kind>(*combined_start_node, std::move(combined_labeled_succ_nodes));
            result.goal_node = result.plan->get_labeled_succ_nodes().back().node;

            event_handler->on_solved(*result.plan);
            event_handler->on_end_search(result.status);
            return result;
        }
    }

    auto result = SearchResult<Kind> {};
    result.status = SearchStatus::EXHAUSTED;
    if (combined_start_node)
    {
        result.plan = Plan<Kind>(*combined_start_node, std::move(combined_labeled_succ_nodes));
        result.goal_node = result.plan->get_length() > 0 ? result.plan->get_labeled_succ_nodes().back().node : result.plan->get_start_node();
    }
    event_handler->on_end_search(result.status);
    return result;
}

template<TaskKind Kind, SerializedSolverConcept<Kind> Subsolver>
struct Solver
{
    using EventHandlerType = EventHandler<Kind, Subsolver>;

    Subsolver subsolver;
    Options<Kind, Subsolver> options;

    SearchResult<Kind> solve() { return find_solution<Kind>(subsolver, options); }
};

}  // namespace tyr::planning::serialized

#endif
