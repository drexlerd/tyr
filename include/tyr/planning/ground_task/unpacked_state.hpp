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

#ifndef TYR_PLANNING_GROUND_TASK_UNPACKED_STATE_HPP_
#define TYR_PLANNING_GROUND_TASK_UNPACKED_STATE_HPP_

#include "tyr/common/config.hpp"
#include "tyr/common/dynamic_bitset.hpp"
#include "tyr/common/vector.hpp"
#include "tyr/formalism/planning/fdr_fact_data.hpp"
#include "tyr/formalism/planning/fdr_variable_index.hpp"
#include "tyr/formalism/planning/ground_atom_index.hpp"
#include "tyr/formalism/planning/ground_function_term_index.hpp"
#include "tyr/planning/declarations.hpp"
#include "tyr/planning/ground_task/state_storage.hpp"
#include "tyr/planning/state_storage.hpp"
#include "tyr/planning/unpacked_state.hpp"

#include <boost/dynamic_bitset.hpp>
#include <cassert>
#include <limits>
#include <vector>

namespace tyr::planning
{
template<formalism::FactKind T>
struct GroundUnpackedAtomStorageType;

template<>
struct GroundUnpackedAtomStorageType<formalism::FluentTag>
{
    using type = planning::FactUnpackedStorage<GroundTag>;
};

template<>
struct GroundUnpackedAtomStorageType<formalism::DerivedTag>
{
    using type = planning::AtomUnpackedStorage<GroundTag>;
};

template<formalism::FactKind T>
using GroundUnpackedAtomStorage = typename GroundUnpackedAtomStorageType<T>::type;

template<>
class UnpackedState<GroundTag>
{
public:
    using TaskType = Task<GroundTag>;

    UnpackedState() = default;

    /**
     * UnpackedStateConcept
     */

    Index<State<GroundTag>> get_index() const;
    void set(Index<State<GroundTag>> index);

    formalism::planning::FDRValue get(Index<formalism::planning::FDRVariable<formalism::FluentTag>> index) const;
    void set(Data<formalism::planning::FDRFact<formalism::FluentTag>> fact);

    float_t get(Index<formalism::planning::GroundFunctionTerm<formalism::FluentTag>> index) const;
    void set(Index<formalism::planning::GroundFunctionTerm<formalism::FluentTag>> index, float_t value);

    bool test(Index<formalism::planning::GroundAtom<formalism::DerivedTag>> index) const;
    void set(Index<formalism::planning::GroundAtom<formalism::DerivedTag>> index);

    void clear();
    void clear_unextended_part();
    void clear_extended_part();
    void assign_unextended_part(const UnpackedState<GroundTag>& other);

    /**
     * For GroundTag
     */

    void resize_fluent_facts(size_t num_fluent_facts);
    void resize_derived_atoms(size_t num_derived_atoms);

    template<formalism::FactKind T>
    GroundUnpackedAtomStorage<T>& get_atoms() noexcept;
    template<formalism::FactKind T>
    const GroundUnpackedAtomStorage<T>& get_atoms() const noexcept;

    planning::NumericUnpackedStorage<GroundTag>& get_numeric_variables() noexcept;
    const planning::NumericUnpackedStorage<GroundTag>& get_numeric_variables() const noexcept;

private:
    Index<State<GroundTag>> m_index;

    planning::FactUnpackedStorage<GroundTag> m_fact_storage;
    planning::AtomUnpackedStorage<GroundTag> m_atom_storage;
    planning::NumericUnpackedStorage<GroundTag> m_numeric_storage;
};

static_assert(UnpackedStateConcept<UnpackedState<GroundTag>, GroundTag>);

}

#endif
