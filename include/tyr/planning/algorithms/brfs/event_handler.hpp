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

#ifndef TYR_PLANNING_ALGORITHMS_BRFS_EVENT_HANDLER_HPP_
#define TYR_PLANNING_ALGORITHMS_BRFS_EVENT_HANDLER_HPP_

#include "tyr/formalism/planning/ground_action_view.hpp"
#include "tyr/planning/algorithms/statistics.hpp"
#include "tyr/planning/algorithms/utils.hpp"
#include "tyr/planning/declarations.hpp"

#include <chrono>
#include <concepts>
#include <cstdint>

namespace tyr::planning::brfs
{

template<TaskKind Kind>
class EventHandler
{
public:
    using StatisticsType = tyr::planning::Statistics;

    virtual ~EventHandler() = default;

    virtual void on_expand_node(const Node<Kind>& node) = 0;

    virtual void on_expand_goal_node(const Node<Kind>& node) = 0;

    virtual void on_generate_node(const Node<Kind>& source_node, const LabeledNode<Kind>& labeled_succ_node) = 0;

    virtual void on_prune_node(const Node<Kind>& node) = 0;

    virtual void on_prune_node(const Node<Kind>& source_node, const LabeledNode<Kind>& labeled_succ_node) = 0;

    virtual void on_start_search(const Node<Kind>& node) = 0;

    virtual void on_finish_layer(uint_t layer) = 0;

    virtual void on_end_search(tyr::planning::SearchStatus status) = 0;

    virtual void on_solved(const Plan<Kind>& plan) = 0;

    virtual const tyr::planning::Statistics& get_search_statistics() const = 0;
    virtual const tyr::planning::Statistics& get_statistics() const = 0;
};

template<typename Derived, TaskKind Kind>
class EventHandlerBase : public EventHandler<Kind>
{
protected:
    tyr::planning::Statistics m_statistics;
    tyr::planning::ProgressStatistics m_progress_statistics;
    size_t m_verbosity;

private:
    EventHandlerBase() = default;
    friend Derived;

    constexpr const auto& self() const { return static_cast<const Derived&>(*this); }
    constexpr auto& self() { return static_cast<Derived&>(*this); }

    bool verbosity(size_t level) const { return m_verbosity >= level; }

public:
    explicit EventHandlerBase(size_t verbosity = 0) : m_statistics(), m_verbosity(verbosity) {}

    void on_expand_node(const Node<Kind>& node) override
    {
        m_statistics.increment_num_expanded();

        if (verbosity(2))
            self().on_expand_node_impl(node);
    }

    void on_expand_goal_node(const Node<Kind>& node) override
    {
        if (verbosity(2))
            self().on_expand_goal_node_impl(node);
    }

    void on_generate_node(const Node<Kind>& source_node, const LabeledNode<Kind>& labeled_succ_node) override
    {
        static_cast<void>(source_node);
        m_statistics.increment_num_generated();

        if (verbosity(2))
            self().on_generate_node_impl(labeled_succ_node);
    }

    void on_prune_node(const Node<Kind>& node) override
    {
        m_statistics.increment_num_pruned();

        if (verbosity(2))
            self().on_prune_node_impl(node);
    }

    void on_prune_node(const Node<Kind>& source_node, const LabeledNode<Kind>& labeled_succ_node) override
    {
        static_cast<void>(source_node);
        m_statistics.increment_num_pruned();

        if (verbosity(2))
            self().on_prune_node_impl(labeled_succ_node.node);
    }

    void on_start_search(const Node<Kind>& node) override
    {
        m_statistics = tyr::planning::Statistics();
        m_progress_statistics = tyr::planning::ProgressStatistics();

        m_statistics.set_search_start_time_point(std::chrono::high_resolution_clock::now());

        if (verbosity(1))
            self().on_start_search_impl(node);
    }

    void on_finish_layer(uint_t layer) override
    {
        m_progress_statistics.add_snap_shot(m_statistics);

        if (verbosity(1))
            self().on_finish_layer_impl(layer, m_statistics.get_num_expanded(), m_statistics.get_num_generated());
    }

    void on_end_search(tyr::planning::SearchStatus status) override
    {
        m_statistics.set_search_end_time_point(std::chrono::high_resolution_clock::now());

        if (verbosity(1))
            self().on_end_search_impl(status);
    }

    void on_solved(const Plan<Kind>& plan) override
    {
        if (verbosity(1))
            self().on_solved_impl(plan);
    }

    const tyr::planning::Statistics& get_search_statistics() const override { return m_statistics; }
    const tyr::planning::Statistics& get_statistics() const override { return m_statistics; }
    const tyr::planning::ProgressStatistics& get_progress_statistics() const { return m_progress_statistics; }
};

template<TaskKind Kind>
class DefaultEventHandler : public EventHandlerBase<DefaultEventHandler<Kind>, Kind>
{
private:
    friend class EventHandlerBase<DefaultEventHandler<Kind>, Kind>;

    void on_expand_node_impl(const Node<Kind>& node) const;

    void on_expand_goal_node_impl(const Node<Kind>& node) const;

    void on_generate_node_impl(const LabeledNode<Kind>& labeled_succ_node) const;

    void on_prune_node_impl(const Node<Kind>& node) const;

    void on_start_search_impl(const Node<Kind>& node) const;

    void on_finish_layer_impl(uint_t layer, uint64_t num_expanded_states, uint64_t num_generated_states) const;

    void on_end_search_impl(tyr::planning::SearchStatus status) const;

    void on_solved_impl(const Plan<Kind>& plan) const;

public:
    DefaultEventHandler(size_t verbosity = 0);

    static DefaultEventHandlerPtr<Kind> create(size_t verbosity = 0);
};

}

#endif
