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

#include "tyr/planning/ground_task.hpp"
#include "tyr/planning/ground_task/state_repository.hpp"
#include "tyr/planning/ground_task/state_view.hpp"
#include "tyr/planning/ground_task/unpacked_state.hpp"

#include <cassert>
#include <limits>

namespace f = tyr::formalism;
namespace fp = tyr::formalism::planning;

namespace tyr::planning
{

Index<State<GroundTag>> UnpackedState<GroundTag>::get_index() const { return m_index; }

void UnpackedState<GroundTag>::set(Index<State<GroundTag>> index) { m_index = index; }

fp::FDRValue UnpackedState<GroundTag>::get(Index<fp::FDRVariable<f::FluentTag>> index) const
{
    assert(uint_t(index) < m_fact_storage.values.size());
    return fp::FDRValue(m_fact_storage.values[uint_t(index)]);
}

void UnpackedState<GroundTag>::set(Data<fp::FDRFact<f::FluentTag>> fact)
{
    assert(uint_t(fact.variable) < m_fact_storage.values.size());
    m_fact_storage.values[uint_t(fact.variable)] = uint_t(fact.value);
}

float_t UnpackedState<GroundTag>::get(Index<fp::GroundFunctionTerm<f::FluentTag>> index) const
{
    return tyr::get(uint_t(index), m_numeric_storage.values, std::numeric_limits<float_t>::quiet_NaN());
}

void UnpackedState<GroundTag>::set(Index<fp::GroundFunctionTerm<f::FluentTag>> index, float_t value)
{
    tyr::set(uint_t(index), value, m_numeric_storage.values, std::numeric_limits<float_t>::quiet_NaN());
}

bool UnpackedState<GroundTag>::test(Index<fp::GroundAtom<f::DerivedTag>> index) const
{
    assert(uint_t(index) < m_atom_storage.indices.size());
    return m_atom_storage.indices.test(uint_t(index));
}

void UnpackedState<GroundTag>::set(Index<fp::GroundAtom<f::DerivedTag>> index)
{
    assert(uint_t(index) < m_atom_storage.indices.size());
    m_atom_storage.indices.set(uint_t(index));
}

void UnpackedState<GroundTag>::clear()
{
    clear_unextended_part();
    clear_extended_part();
}

void UnpackedState<GroundTag>::clear_unextended_part()
{
    m_fact_storage.values.clear();
    m_numeric_storage.values.clear();
}

void UnpackedState<GroundTag>::clear_extended_part() { m_atom_storage.indices.clear(); }

void UnpackedState<GroundTag>::assign_unextended_part(const UnpackedState<GroundTag>& other)
{
    m_fact_storage = other.m_fact_storage;
    m_numeric_storage = other.m_numeric_storage;
}

void UnpackedState<GroundTag>::resize_fluent_facts(size_t num_fluent_facts) { m_fact_storage.values.resize(num_fluent_facts, 0); }

void UnpackedState<GroundTag>::resize_derived_atoms(size_t num_derived_atoms) { m_atom_storage.indices.resize(num_derived_atoms, false); }

NumericUnpackedStorage<GroundTag>& UnpackedState<GroundTag>::get_numeric_variables() noexcept { return m_numeric_storage; }

const NumericUnpackedStorage<GroundTag>& UnpackedState<GroundTag>::get_numeric_variables() const noexcept { return m_numeric_storage; }

template<f::FactKind T>
GroundUnpackedAtomStorage<T>& UnpackedState<GroundTag>::get_atoms() noexcept
{
    if constexpr (std::same_as<T, f::FluentTag>)
        return m_fact_storage;
    else if constexpr (std::same_as<T, f::DerivedTag>)
        return m_atom_storage;
}

template<f::FactKind T>
const GroundUnpackedAtomStorage<T>& UnpackedState<GroundTag>::get_atoms() const noexcept
{
    if constexpr (std::same_as<T, f::FluentTag>)
        return m_fact_storage;
    else if constexpr (std::same_as<T, f::DerivedTag>)
        return m_atom_storage;
}

template FactUnpackedStorage<GroundTag>& UnpackedState<GroundTag>::get_atoms<f::FluentTag>() noexcept;
template AtomUnpackedStorage<GroundTag>& UnpackedState<GroundTag>::get_atoms<f::DerivedTag>() noexcept;
template const FactUnpackedStorage<GroundTag>& UnpackedState<GroundTag>::get_atoms<f::FluentTag>() const noexcept;
template const AtomUnpackedStorage<GroundTag>& UnpackedState<GroundTag>::get_atoms<f::DerivedTag>() const noexcept;

}

namespace tyr
{

GroundStateView::View(std::shared_ptr<planning::StateRepository<planning::GroundTag>> owner,
                      SharedObjectPoolPtr<planning::UnpackedState<planning::GroundTag>> unpacked) noexcept :
    m_state_repository(std::move(owner)),
    m_unpacked(std::move(unpacked))
{
}

GroundStateView::~View() = default;

GroundStateView::View(const View&) = default;

GroundStateView::View(View&&) noexcept = default;

GroundStateView& GroundStateView::operator=(const View&) = default;

GroundStateView& GroundStateView::operator=(View&&) noexcept = default;

Index<planning::State<planning::GroundTag>> GroundStateView::get_index() const { return m_unpacked->get_index(); }

formalism::planning::FDRValue GroundStateView::get(Index<formalism::planning::FDRVariable<formalism::FluentTag>> index) const
{
    return m_unpacked->get(index);
}

float_t GroundStateView::get(Index<formalism::planning::GroundFunctionTerm<formalism::FluentTag>> index) const { return m_unpacked->get(index); }

bool GroundStateView::test(Index<formalism::planning::GroundAtom<formalism::DerivedTag>> index) const { return m_unpacked->test(index); }

const std::shared_ptr<planning::StateRepository<planning::GroundTag>>& GroundStateView::get_state_repository() const noexcept
{
    return m_state_repository;
}

const planning::UnpackedState<planning::GroundTag>& GroundStateView::get_unpacked_state() const noexcept { return *m_unpacked; }

const std::vector<uint_t>& GroundStateView::get_fluent_values() const noexcept { return m_unpacked->get_atoms<formalism::FluentTag>().values; }

bool GroundStateView::test(formalism::planning::GroundAtomView<formalism::StaticTag> view) const { return test(view.get_index()); }

float_t GroundStateView::get(formalism::planning::GroundFunctionTermView<formalism::StaticTag> view) const { return get(view.get_index()); }

formalism::planning::FDRValue GroundStateView::get(formalism::planning::FDRVariableView<formalism::FluentTag> view) const
{
    return get(view.get_index());
}

float_t GroundStateView::get(formalism::planning::GroundFunctionTermView<formalism::FluentTag> view) const { return get(view.get_index()); }

bool GroundStateView::test(formalism::planning::GroundAtomView<formalism::DerivedTag> view) const { return test(view.get_index()); }

bool GroundStateView::test(Index<fp::GroundAtom<f::StaticTag>> index) const { return m_state_repository->get_task()->test(index); }

float_t GroundStateView::get(Index<fp::GroundFunctionTerm<f::StaticTag>> index) const { return m_state_repository->get_task()->get(index); }

template<f::FactKind T>
const boost::dynamic_bitset<>& GroundStateView::get_atoms() const noexcept
{
    if constexpr (std::is_same_v<T, f::StaticTag>)
        return m_state_repository->get_task()->get_static_atoms_bitset();
    else if constexpr (std::is_same_v<T, f::DerivedTag>)
        return m_unpacked->template get_atoms<f::DerivedTag>().indices;
    else
        static_assert(dependent_false<T>::value, "Missing case");
}

template const boost::dynamic_bitset<>& GroundStateView::get_atoms<f::StaticTag>() const noexcept;
template const boost::dynamic_bitset<>& GroundStateView::get_atoms<f::DerivedTag>() const noexcept;

template<f::FactKind T>
const std::vector<float_t>& GroundStateView::get_numeric_variables() const noexcept
{
    if constexpr (std::is_same_v<T, f::StaticTag>)
        return m_state_repository->get_task()->get_static_numeric_variables();
    else if constexpr (std::is_same_v<T, f::FluentTag>)
        return m_unpacked->get_numeric_variables().values;
    else
        static_assert(dependent_false<T>::value, "Missing case");
}

template const std::vector<float_t>& GroundStateView::get_numeric_variables<f::StaticTag>() const noexcept;
template const std::vector<float_t>& GroundStateView::get_numeric_variables<f::FluentTag>() const noexcept;

planning::AtomRange<formalism::StaticTag> GroundStateView::get_static_atoms() const noexcept
{
    return planning::AtomRange<formalism::StaticTag>(m_state_repository->get_task()->get_static_atoms_bitset());
}

planning::FDRFactRange<planning::GroundTag, formalism::FluentTag> GroundStateView::get_fluent_facts() const noexcept
{
    return planning::FDRFactRange<planning::GroundTag, formalism::FluentTag>(get_fluent_values());
}

planning::AtomRange<formalism::DerivedTag> GroundStateView::get_derived_atoms() const noexcept
{
    return planning::AtomRange<formalism::DerivedTag>(get_atoms<formalism::DerivedTag>());
}

planning::FunctionTermValueRange<formalism::StaticTag> GroundStateView::get_static_fterm_values() const noexcept
{
    return planning::FunctionTermValueRange<formalism::StaticTag>(m_state_repository->get_task()->get_static_numeric_variables());
}

planning::FunctionTermValueRange<formalism::FluentTag> GroundStateView::get_fluent_fterm_values() const noexcept
{
    return planning::FunctionTermValueRange<formalism::FluentTag>(get_numeric_variables<formalism::FluentTag>());
}

const std::shared_ptr<formalism::planning::Repository>& GroundStateView::get_repository() const noexcept
{
    return m_state_repository->get_task()->get_repository();
}

static_assert(planning::IterableStateConcept<GroundStateView>);
static_assert(planning::IterableViewStateConcept<GroundStateView>);
static_assert(planning::IndexableStateConcept<GroundStateView, planning::GroundTag>);
static_assert(planning::IndexableViewStateConcept<GroundStateView, planning::GroundTag>);
}
