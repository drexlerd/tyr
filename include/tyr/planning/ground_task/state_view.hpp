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

#ifndef TYR_PLANNING_GROUND_TASK_STATE_VIEW_HPP_
#define TYR_PLANNING_GROUND_TASK_STATE_VIEW_HPP_

#include "tyr/formalism/planning/declarations.hpp"
#include "tyr/formalism/planning/repository.hpp"
#include "tyr/formalism/planning/views.hpp"
#include "tyr/common/shared_object_pool.hpp"
#include "tyr/planning/declarations.hpp"
#include "tyr/planning/ground_task/state_iterators.hpp"
#include "tyr/planning/state_iterators.hpp"
#include "tyr/planning/state_view.hpp"
#include "tyr/planning/task.hpp"

#include <boost/dynamic_bitset.hpp>

namespace tyr
{
template<>
struct View<Index<planning::State<planning::GroundTag>>, std::shared_ptr<planning::StateRepository<planning::GroundTag>>>
{
public:
    using TaskType = planning::Task<planning::GroundTag>;

    View(std::shared_ptr<planning::StateRepository<planning::GroundTag>> owner,
         SharedObjectPoolPtr<planning::UnpackedState<planning::GroundTag>> unpacked) noexcept;
    View(const View&);
    View(View&&) noexcept;
    View& operator=(const View&);
    View& operator=(View&&) noexcept;
    ~View();

    Index<planning::State<planning::GroundTag>> get_index() const;

    /**
     * IndexableStateConcept
     */

    bool test(Index<formalism::planning::GroundAtom<formalism::StaticTag>> index) const;
    float_t get(Index<formalism::planning::GroundFunctionTerm<formalism::StaticTag>> index) const;
    formalism::planning::FDRValue get(Index<formalism::planning::FDRVariable<formalism::FluentTag>> index) const;
    float_t get(Index<formalism::planning::GroundFunctionTerm<formalism::FluentTag>> index) const;
    bool test(Index<formalism::planning::GroundAtom<formalism::DerivedTag>> index) const;

    /**
     * IndexableViewStateConcept
     */

    bool test(formalism::planning::GroundAtomView<formalism::StaticTag> index) const;
    float_t get(formalism::planning::GroundFunctionTermView<formalism::StaticTag> index) const;
    formalism::planning::FDRValue get(formalism::planning::FDRVariableView<formalism::FluentTag> index) const;
    float_t get(formalism::planning::GroundFunctionTermView<formalism::FluentTag> index) const;
    bool test(formalism::planning::GroundAtomView<formalism::DerivedTag> index) const;

    /**
     * IterableStateConcept
     */

    planning::AtomRange<formalism::StaticTag> get_static_atoms() const noexcept;
    planning::FDRFactRange<planning::GroundTag, formalism::FluentTag> get_fluent_facts() const noexcept;
    planning::AtomRange<formalism::DerivedTag> get_derived_atoms() const noexcept;
    planning::FunctionTermValueRange<formalism::StaticTag> get_static_fterm_values() const noexcept;
    planning::FunctionTermValueRange<formalism::FluentTag> get_fluent_fterm_values() const noexcept;

    /**
     * IterableStateViewConcept
     */

    auto get_static_atoms_view() const noexcept;
    auto get_fluent_facts_view() const noexcept;
    auto get_derived_atoms_view() const noexcept;
    auto get_static_fterm_values_view() const noexcept;
    auto get_fluent_fterm_values_view() const noexcept;

    /**
     * Getters
     */

    const std::shared_ptr<formalism::planning::Repository>& get_repository() const noexcept;
    const std::shared_ptr<planning::StateRepository<planning::GroundTag>>& get_state_repository() const noexcept;
    const planning::UnpackedState<planning::GroundTag>& get_unpacked_state() const noexcept;

    template<formalism::FactKind T>
    const boost::dynamic_bitset<>& get_atoms() const noexcept;

    const std::vector<uint_t>& get_fluent_values() const noexcept;

    template<formalism::FactKind T>
    const std::vector<float_t>& get_numeric_variables() const noexcept;

private:
    std::shared_ptr<planning::StateRepository<planning::GroundTag>> m_state_repository;
    SharedObjectPoolPtr<planning::UnpackedState<planning::GroundTag>> m_unpacked;
};

using GroundStateView = View<Index<planning::State<planning::GroundTag>>, std::shared_ptr<planning::StateRepository<planning::GroundTag>>>;

inline auto GroundStateView::get_static_atoms_view() const noexcept
{
    return get_static_atoms() | std::views::transform([context = this->get_repository()](auto id) { return make_view(id, *context); });
}
inline auto GroundStateView::get_fluent_facts_view() const noexcept
{
    return get_fluent_facts() | std::views::transform([context = this->get_repository()](auto id) { return make_view(id, *context); });
}
inline auto GroundStateView::get_derived_atoms_view() const noexcept
{
    return get_derived_atoms() | std::views::transform([context = this->get_repository()](auto id) { return make_view(id, *context); });
}
inline auto GroundStateView::get_static_fterm_values_view() const noexcept
{
    return get_static_fterm_values()
           | std::views::transform([context = this->get_repository()](auto&& pair) { return std::make_pair(make_view(pair.first, *context), pair.second); });
}
inline auto GroundStateView::get_fluent_fterm_values_view() const noexcept
{
    return get_fluent_fterm_values()
           | std::views::transform([context = this->get_repository()](auto&& pair) { return std::make_pair(make_view(pair.first, *context), pair.second); });
}
}

#endif
