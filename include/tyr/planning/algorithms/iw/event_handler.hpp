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

#ifndef TYR_PLANNING_ALGORITHMS_IW_EVENT_HANDLER_HPP_
#define TYR_PLANNING_ALGORITHMS_IW_EVENT_HANDLER_HPP_

#include "tyr/planning/algorithms/iw/statistics.hpp"
#include "tyr/planning/algorithms/utils.hpp"
#include "tyr/planning/declarations.hpp"

#include <memory>

namespace tyr::planning::iw
{

template<TaskKind Kind>
class EventHandler
{
public:
    virtual ~EventHandler() = default;
    virtual void on_start_search(uint_t max_arity) = 0;
    virtual void on_start_arity(uint_t arity) = 0;
    virtual void on_end_arity(uint_t arity, SearchStatus status) = 0;
    virtual void on_end_search() = 0;
    virtual void on_solved(uint_t arity) = 0;
};

template<TaskKind Kind>
class DefaultEventHandler : public EventHandler<Kind>
{
private:
    Statistics m_statistics;

public:
    void on_start_search(uint_t max_arity) override
    {
        static_cast<void>(max_arity);
        m_statistics.clear();
    }

    void on_start_arity(uint_t arity) override { static_cast<void>(arity); }

    void on_end_arity(uint_t arity, SearchStatus status) override
    {
        static_cast<void>(arity);
        static_cast<void>(status);
    }

    void on_end_search() override {}

    void on_solved(uint_t arity) override { m_statistics.set_solution_arity(arity); }

    const Statistics& get_statistics() const { return m_statistics; }

    static DefaultEventHandlerPtr<Kind> create() { return std::make_shared<DefaultEventHandler<Kind>>(); }
};

}

#endif
