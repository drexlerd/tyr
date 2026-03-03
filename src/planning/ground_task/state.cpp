/*
 * Copyright (C) 2025 Dominik Drexler
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

#include "tyr/planning/ground_task/state.hpp"

#include "tyr/planning/ground_task.hpp"
#include "tyr/planning/ground_task/state_repository.hpp"

namespace f = tyr::formalism;
namespace fp = tyr::formalism::planning;

namespace tyr::planning
{

bool State<GroundTask>::test(Index<fp::GroundAtom<f::StaticTag>> index) const { return m_state_repository->get_task()->test(index); }

float_t State<GroundTask>::get(Index<fp::GroundFunctionTerm<f::StaticTag>> index) const { return m_state_repository->get_task()->get(index); }

template<f::FactKind T>
const boost::dynamic_bitset<>& State<GroundTask>::get_atoms() const noexcept
{
    if constexpr (std::is_same_v<T, f::StaticTag>)
        return m_state_repository->get_task()->get_static_atoms_bitset();
    else if constexpr (std::is_same_v<T, f::DerivedTag>)
        return m_unpacked->get_derived_atoms();
    else
        static_assert(dependent_false<T>::value, "Missing case");
}

template const boost::dynamic_bitset<>& State<GroundTask>::get_atoms<f::StaticTag>() const noexcept;
template const boost::dynamic_bitset<>& State<GroundTask>::get_atoms<f::DerivedTag>() const noexcept;

template<f::FactKind T>
const std::vector<float_t>& State<GroundTask>::get_numeric_variables() const noexcept
{
    if constexpr (std::is_same_v<T, f::StaticTag>)
        return m_state_repository->get_task()->get_static_numeric_variables();
    else if constexpr (std::is_same_v<T, f::FluentTag>)
        return m_unpacked->get_numeric_variables();
    else
        static_assert(dependent_false<T>::value, "Missing case");
}

template const std::vector<float_t>& State<GroundTask>::get_numeric_variables<f::StaticTag>() const noexcept;
template const std::vector<float_t>& State<GroundTask>::get_numeric_variables<f::FluentTag>() const noexcept;

AtomRange<formalism::StaticTag> State<GroundTask>::get_static_atoms() const noexcept
{
    return AtomRange<formalism::StaticTag>(m_state_repository->get_task()->get_static_atoms_bitset());
}

FDRFactRange<GroundTask, formalism::FluentTag> State<GroundTask>::get_fluent_facts() const noexcept
{
    return FDRFactRange<GroundTask, formalism::FluentTag>(get_fluent_values());
}

AtomRange<formalism::DerivedTag> State<GroundTask>::get_derived_atoms() const noexcept
{
    return AtomRange<formalism::DerivedTag>(get_atoms<formalism::DerivedTag>());
}

FunctionTermValueRange<formalism::StaticTag> State<GroundTask>::get_static_fterm_values() const noexcept
{
    return FunctionTermValueRange<formalism::StaticTag>(m_state_repository->get_task()->get_static_numeric_variables());
}

FunctionTermValueRange<formalism::FluentTag> State<GroundTask>::get_fluent_fterm_values() const noexcept
{
    return FunctionTermValueRange<formalism::FluentTag>(get_numeric_variables<formalism::FluentTag>());
}

const std::shared_ptr<formalism::planning::Repository>& State<GroundTask>::get_repository() const noexcept
{
    return m_state_repository->get_task()->get_repository();
}

static_assert(IterableStateConcept<State<GroundTask>>);
static_assert(IterableViewStateConcept<State<GroundTask>>);
static_assert(IndexableStateConcept<State<GroundTask>, GroundTask>);
static_assert(IndexableViewStateConcept<State<GroundTask>, GroundTask>);
}
