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

#ifndef TYR_PLANNING_LIFTED_TASK_STATE_VIEW_HPP_
#define TYR_PLANNING_LIFTED_TASK_STATE_VIEW_HPP_

#include "tyr/common/shared_object_pool.hpp"
#include "tyr/formalism/planning/declarations.hpp"
#include "tyr/formalism/planning/repository.hpp"
#include "tyr/formalism/planning/views.hpp"
#include "tyr/planning/declarations.hpp"
#include "tyr/planning/lifted_task/state_iterators.hpp"
#include "tyr/planning/state_iterators.hpp"
#include "tyr/planning/state_view.hpp"

#include <boost/dynamic_bitset.hpp>

namespace tyr
{
template<>
struct View<Index<planning::State<planning::LiftedTag>>, std::shared_ptr<planning::StateRepository<planning::LiftedTag>>>
{
public:
    using TaskType = planning::Task<planning::LiftedTag>;

    View(std::shared_ptr<planning::StateRepository<planning::LiftedTag>> owner,
         SharedObjectPoolPtr<planning::UnpackedState<planning::LiftedTag>> unpacked) noexcept;
    View(const View&);
    View(View&&) noexcept;
    View& operator=(const View&);
    View& operator=(View&&) noexcept;
    ~View();

    Index<planning::State<planning::LiftedTag>> get_index() const;

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

    bool test(formalism::planning::GroundAtomView<formalism::StaticTag> view) const;
    float_t get(formalism::planning::GroundFunctionTermView<formalism::StaticTag> view) const;
    formalism::planning::FDRValue get(formalism::planning::FDRVariableView<formalism::FluentTag> view) const;
    float_t get(formalism::planning::GroundFunctionTermView<formalism::FluentTag> view) const;
    bool test(formalism::planning::GroundAtomView<formalism::DerivedTag> view) const;

    /**
     * IterableStateConcept
     */

    planning::AtomRange<formalism::StaticTag> get_static_atoms() const noexcept;
    planning::FDRFactRange<planning::LiftedTag, formalism::FluentTag> get_fluent_facts() const noexcept;
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
    const std::shared_ptr<planning::StateRepository<planning::LiftedTag>>& get_state_repository() const noexcept;
    const planning::UnpackedState<planning::LiftedTag>& get_unpacked_state() const noexcept;

private:
    template<formalism::FactKind T>
    const boost::dynamic_bitset<>& get_atoms() const noexcept;

    template<formalism::FactKind T>
    const std::vector<float_t>& get_numeric_variables() const noexcept;

    std::shared_ptr<planning::StateRepository<planning::LiftedTag>> m_state_repository;
    SharedObjectPoolPtr<planning::UnpackedState<planning::LiftedTag>> m_unpacked;
};

using LiftedStateView = View<Index<planning::State<planning::LiftedTag>>, std::shared_ptr<planning::StateRepository<planning::LiftedTag>>>;

inline auto LiftedStateView::get_static_atoms_view() const noexcept
{
    return get_static_atoms() | std::views::transform([context = this->get_repository()](auto id) { return make_view(id, *context); });
}
inline auto LiftedStateView::get_fluent_facts_view() const noexcept
{
    return get_fluent_facts() | std::views::transform([context = this->get_repository()](auto id) { return make_view(id, *context); });
}
inline auto LiftedStateView::get_derived_atoms_view() const noexcept
{
    return get_derived_atoms() | std::views::transform([context = this->get_repository()](auto id) { return make_view(id, *context); });
}
inline auto LiftedStateView::get_static_fterm_values_view() const noexcept
{
    return get_static_fterm_values()
           | std::views::transform([context = this->get_repository()](auto&& pair) { return std::make_pair(make_view(pair.first, *context), pair.second); });
}
inline auto LiftedStateView::get_fluent_fterm_values_view() const noexcept
{
    return get_fluent_fterm_values()
           | std::views::transform([context = this->get_repository()](auto&& pair) { return std::make_pair(make_view(pair.first, *context), pair.second); });
}
}

#endif
