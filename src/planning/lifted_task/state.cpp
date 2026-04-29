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

#include "tyr/planning/lifted_task.hpp"
#include "tyr/planning/lifted_task/state_repository.hpp"
#include "tyr/planning/lifted_task/state_view.hpp"
#include "tyr/planning/lifted_task/unpacked_state.hpp"

#include <cassert>
#include <limits>

namespace tyr::planning
{

Index<State<LiftedTag>> UnpackedState<LiftedTag>::get_index() const { return m_index; }

void UnpackedState<LiftedTag>::set(Index<State<LiftedTag>> index) { m_index = index; }

formalism::planning::FDRValue UnpackedState<LiftedTag>::get(Index<formalism::planning::FDRVariable<formalism::FluentTag>> index) const
{
    return formalism::planning::FDRValue(tyr::test(uint_t(index), m_fact_storage.indices));
}

void UnpackedState<LiftedTag>::set(Data<formalism::planning::FDRFact<formalism::FluentTag>> fact)
{
    assert(uint_t(fact.value) < 2);  // can only handle binary using bitsets
    tyr::set(uint_t(fact.variable), bool(uint_t(fact.value)), m_fact_storage.indices);
}

float_t UnpackedState<LiftedTag>::get(Index<formalism::planning::GroundFunctionTerm<formalism::FluentTag>> index) const
{
    return tyr::get(uint_t(index), m_numeric_storage.values, std::numeric_limits<float_t>::quiet_NaN());
}

void UnpackedState<LiftedTag>::set(Index<formalism::planning::GroundFunctionTerm<formalism::FluentTag>> index, float_t value)
{
    tyr::set(uint_t(index), value, m_numeric_storage.values, std::numeric_limits<float_t>::quiet_NaN());
}

bool UnpackedState<LiftedTag>::test(Index<formalism::planning::GroundAtom<formalism::DerivedTag>> index) const
{
    return tyr::test(uint_t(index), m_atom_storage.indices);
}

void UnpackedState<LiftedTag>::set(Index<formalism::planning::GroundAtom<formalism::DerivedTag>> index)
{
    tyr::set(uint_t(index), true, m_atom_storage.indices);
}

formalism::planning::FDRValue UnpackedState<LiftedTag>::get(formalism::planning::FDRVariableView<formalism::FluentTag> view) const
{
    return get(view.get_index());
}

void UnpackedState<LiftedTag>::set(formalism::planning::FDRFactView<formalism::FluentTag> view) { set(view.get_data()); }

float_t UnpackedState<LiftedTag>::get(formalism::planning::GroundFunctionTermView<formalism::FluentTag> view) const { return get(view.get_index()); }

void UnpackedState<LiftedTag>::set(formalism::planning::GroundFunctionTermView<formalism::FluentTag> view, float_t value)
{
    set(view.get_index(), value);
}

bool UnpackedState<LiftedTag>::test(formalism::planning::GroundAtomView<formalism::DerivedTag> view) const { return test(view.get_index()); }

void UnpackedState<LiftedTag>::set(formalism::planning::GroundAtomView<formalism::DerivedTag> view) { set(view.get_index()); }

void UnpackedState<LiftedTag>::clear()
{
    clear_unextended_part();
    clear_extended_part();
}

void UnpackedState<LiftedTag>::clear_unextended_part()
{
    m_fact_storage.indices.clear();
    m_numeric_storage.values.clear();
}

void UnpackedState<LiftedTag>::clear_extended_part() { m_atom_storage.indices.clear(); }

void UnpackedState<LiftedTag>::assign_unextended_part(const UnpackedState<LiftedTag>& other)
{
    m_fact_storage = other.m_fact_storage;
    m_numeric_storage = other.m_numeric_storage;
}

FDRFactRange<LiftedTag, formalism::FluentTag> UnpackedState<LiftedTag>::get_fluent_facts() const noexcept
{
    return FDRFactRange<LiftedTag, formalism::FluentTag>(m_fact_storage.indices);
}

AtomRange<formalism::DerivedTag> UnpackedState<LiftedTag>::get_derived_atoms() const noexcept
{
    return AtomRange<formalism::DerivedTag>(m_atom_storage.indices);
}

FunctionTermValueRange<formalism::FluentTag> UnpackedState<LiftedTag>::get_fluent_fterm_values() const noexcept
{
    return FunctionTermValueRange<formalism::FluentTag>(m_numeric_storage.values);
}

NumericUnpackedStorage<LiftedTag>& UnpackedState<LiftedTag>::get_numeric_variables() noexcept { return m_numeric_storage; }

const NumericUnpackedStorage<LiftedTag>& UnpackedState<LiftedTag>::get_numeric_variables() const noexcept { return m_numeric_storage; }

template<formalism::FactKind T>
LiftedUnpackedAtomStorage<T>& UnpackedState<LiftedTag>::get_atoms() noexcept
{
    if constexpr (std::same_as<T, formalism::FluentTag>)
        return m_fact_storage;
    else if constexpr (std::same_as<T, formalism::DerivedTag>)
        return m_atom_storage;
}

template<formalism::FactKind T>
const LiftedUnpackedAtomStorage<T>& UnpackedState<LiftedTag>::get_atoms() const noexcept
{
    if constexpr (std::same_as<T, formalism::FluentTag>)
        return m_fact_storage;
    else if constexpr (std::same_as<T, formalism::DerivedTag>)
        return m_atom_storage;
}

template FactUnpackedStorage<LiftedTag>& UnpackedState<LiftedTag>::get_atoms<formalism::FluentTag>() noexcept;
template AtomUnpackedStorage<LiftedTag>& UnpackedState<LiftedTag>::get_atoms<formalism::DerivedTag>() noexcept;
template const FactUnpackedStorage<LiftedTag>& UnpackedState<LiftedTag>::get_atoms<formalism::FluentTag>() const noexcept;
template const AtomUnpackedStorage<LiftedTag>& UnpackedState<LiftedTag>::get_atoms<formalism::DerivedTag>() const noexcept;

}

namespace tyr
{

LiftedStateView::View(std::shared_ptr<planning::StateRepository<planning::LiftedTag>> owner,
                      SharedObjectPoolPtr<planning::UnpackedState<planning::LiftedTag>> unpacked) noexcept :
    m_state_repository(std::move(owner)),
    m_unpacked(std::move(unpacked))
{
}

LiftedStateView::~View() = default;

LiftedStateView::View(const View&) = default;

LiftedStateView::View(View&&) noexcept = default;

LiftedStateView& LiftedStateView::operator=(const View&) = default;

LiftedStateView& LiftedStateView::operator=(View&&) noexcept = default;

Index<planning::State<planning::LiftedTag>> LiftedStateView::get_index() const { return m_unpacked->get_index(); }

formalism::planning::FDRValue LiftedStateView::get(Index<formalism::planning::FDRVariable<formalism::FluentTag>> index) const
{
    return m_unpacked->get(index);
}

float_t LiftedStateView::get(Index<formalism::planning::GroundFunctionTerm<formalism::FluentTag>> index) const { return m_unpacked->get(index); }

bool LiftedStateView::test(Index<formalism::planning::GroundAtom<formalism::DerivedTag>> index) const { return m_unpacked->test(index); }

const std::shared_ptr<planning::StateRepository<planning::LiftedTag>>& LiftedStateView::get_state_repository() const noexcept
{
    return m_state_repository;
}

const planning::UnpackedState<planning::LiftedTag>& LiftedStateView::get_unpacked_state() const noexcept { return *m_unpacked; }

bool LiftedStateView::test(formalism::planning::GroundAtomView<formalism::StaticTag> view) const { return test(view.get_index()); }

float_t LiftedStateView::get(formalism::planning::GroundFunctionTermView<formalism::StaticTag> view) const { return get(view.get_index()); }

formalism::planning::FDRValue LiftedStateView::get(formalism::planning::FDRVariableView<formalism::FluentTag> view) const
{
    return get(view.get_index());
}

float_t LiftedStateView::get(formalism::planning::GroundFunctionTermView<formalism::FluentTag> view) const { return get(view.get_index()); }

bool LiftedStateView::test(formalism::planning::GroundAtomView<formalism::DerivedTag> view) const { return test(view.get_index()); }

bool LiftedStateView::test(Index<formalism::planning::GroundAtom<formalism::StaticTag>> index) const { return m_state_repository->get_task()->test(index); }

float_t LiftedStateView::get(Index<formalism::planning::GroundFunctionTerm<formalism::StaticTag>> index) const
{
    return m_state_repository->get_task()->get(index);
}

template<formalism::FactKind T>
const boost::dynamic_bitset<>& LiftedStateView::get_atoms() const noexcept
{
    if constexpr (std::is_same_v<T, formalism::StaticTag>)
        return m_state_repository->get_task()->get_static_atoms_bitset();
    else if constexpr (std::is_same_v<T, formalism::FluentTag> || std::is_same_v<T, formalism::DerivedTag>)
        return m_unpacked->template get_atoms<T>().indices;
    else
        static_assert(dependent_false<T>::value, "Missing case");
}

template const boost::dynamic_bitset<>& LiftedStateView::get_atoms<formalism::StaticTag>() const noexcept;
template const boost::dynamic_bitset<>& LiftedStateView::get_atoms<formalism::FluentTag>() const noexcept;
template const boost::dynamic_bitset<>& LiftedStateView::get_atoms<formalism::DerivedTag>() const noexcept;

template<formalism::FactKind T>
const std::vector<float_t>& LiftedStateView::get_numeric_variables() const noexcept
{
    if constexpr (std::is_same_v<T, formalism::StaticTag>)
        return m_state_repository->get_task()->get_static_numeric_variables();
    else if constexpr (std::is_same_v<T, formalism::FluentTag>)
        return m_unpacked->get_numeric_variables().values;
    else
        static_assert(dependent_false<T>::value, "Missing case");
}

template const std::vector<float_t>& LiftedStateView::get_numeric_variables<formalism::StaticTag>() const noexcept;
template const std::vector<float_t>& LiftedStateView::get_numeric_variables<formalism::FluentTag>() const noexcept;

planning::AtomRange<formalism::StaticTag> LiftedStateView::get_static_atoms() const noexcept
{
    return planning::AtomRange<formalism::StaticTag>(m_state_repository->get_task()->get_static_atoms_bitset());
}

planning::FDRFactRange<planning::LiftedTag, formalism::FluentTag> LiftedStateView::get_fluent_facts() const noexcept
{
    return planning::FDRFactRange<planning::LiftedTag, formalism::FluentTag>(get_atoms<formalism::FluentTag>());
}

planning::AtomRange<formalism::DerivedTag> LiftedStateView::get_derived_atoms() const noexcept
{
    return planning::AtomRange<formalism::DerivedTag>(get_atoms<formalism::DerivedTag>());
}

planning::FunctionTermValueRange<formalism::StaticTag> LiftedStateView::get_static_fterm_values() const noexcept
{
    return planning::FunctionTermValueRange<formalism::StaticTag>(m_state_repository->get_task()->get_static_numeric_variables());
}

planning::FunctionTermValueRange<formalism::FluentTag> LiftedStateView::get_fluent_fterm_values() const noexcept
{
    return planning::FunctionTermValueRange<formalism::FluentTag>(get_numeric_variables<formalism::FluentTag>());
}

const std::shared_ptr<formalism::planning::Repository>& LiftedStateView::get_repository() const noexcept
{
    return m_state_repository->get_task()->get_repository();
}

static_assert(planning::IterableStateConcept<LiftedStateView>);
static_assert(planning::IterableViewStateConcept<LiftedStateView>);
static_assert(planning::IndexableStateConcept<LiftedStateView, planning::LiftedTag>);
static_assert(planning::IndexableViewStateConcept<LiftedStateView, planning::LiftedTag>);

}
