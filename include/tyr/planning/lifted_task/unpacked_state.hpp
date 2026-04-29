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

#ifndef TYR_PLANNING_LIFTED_TASK_UNPACKED_STATE_HPP_
#define TYR_PLANNING_LIFTED_TASK_UNPACKED_STATE_HPP_

#include "tyr/common/config.hpp"
#include "tyr/common/dynamic_bitset.hpp"
#include "tyr/common/vector.hpp"
#include "tyr/formalism/planning/declarations.hpp"
#include "tyr/formalism/planning/repository.hpp"
#include "tyr/formalism/planning/views.hpp"
#include "tyr/planning/declarations.hpp"
#include "tyr/planning/lifted_task/state_iterators.hpp"
#include "tyr/planning/lifted_task/state_storage.hpp"
#include "tyr/planning/state_index.hpp"
#include "tyr/planning/state_storage.hpp"
#include "tyr/planning/state_iterators.hpp"
#include "tyr/planning/unpacked_state.hpp"

#include <boost/dynamic_bitset.hpp>
#include <vector>

namespace tyr::planning
{
template<formalism::FactKind T>
struct LiftedUnpackedAtomStorageType;

template<>
struct LiftedUnpackedAtomStorageType<formalism::FluentTag>
{
    using type = planning::FactUnpackedStorage<LiftedTag>;
};

template<>
struct LiftedUnpackedAtomStorageType<formalism::DerivedTag>
{
    using type = planning::AtomUnpackedStorage<LiftedTag>;
};

template<formalism::FactKind T>
using LiftedUnpackedAtomStorage = typename LiftedUnpackedAtomStorageType<T>::type;

template<>
class UnpackedState<LiftedTag>
{
public:
    using TaskType = Task<LiftedTag>;

    UnpackedState() = default;

    Index<State<LiftedTag>> get_index() const;
    void set(Index<State<LiftedTag>> index);

    void clear();
    void clear_unextended_part();
    void clear_extended_part();
    void assign_unextended_part(const UnpackedState<LiftedTag>& other);

    /**
     * UnpackedStateConcept
     */

    formalism::planning::FDRValue get(Index<formalism::planning::FDRVariable<formalism::FluentTag>> index) const;
    void set(Data<formalism::planning::FDRFact<formalism::FluentTag>> fact);
    float_t get(Index<formalism::planning::GroundFunctionTerm<formalism::FluentTag>> index) const;
    void set(Index<formalism::planning::GroundFunctionTerm<formalism::FluentTag>> index, float_t value);
    bool test(Index<formalism::planning::GroundAtom<formalism::DerivedTag>> index) const;
    void set(Index<formalism::planning::GroundAtom<formalism::DerivedTag>> index);

    formalism::planning::FDRValue get(formalism::planning::FDRVariableView<formalism::FluentTag> view) const;
    void set(formalism::planning::FDRFactView<formalism::FluentTag> view);
    float_t get(formalism::planning::GroundFunctionTermView<formalism::FluentTag> view) const;
    void set(formalism::planning::GroundFunctionTermView<formalism::FluentTag> view, float_t value);
    bool test(formalism::planning::GroundAtomView<formalism::DerivedTag> view) const;
    void set(formalism::planning::GroundAtomView<formalism::DerivedTag> view);

    FDRFactRange<LiftedTag, formalism::FluentTag> get_fluent_facts() const noexcept;
    AtomRange<formalism::DerivedTag> get_derived_atoms() const noexcept;
    FunctionTermValueRange<formalism::FluentTag> get_fluent_fterm_values() const noexcept;

    auto get_fluent_facts_view(const formalism::planning::Repository& repository) const noexcept;
    auto get_derived_atoms_view(const formalism::planning::Repository& repository) const noexcept;
    auto get_fluent_fterm_values_view(const formalism::planning::Repository& repository) const noexcept;

    template<formalism::FactKind T>
    LiftedUnpackedAtomStorage<T>& get_atoms() noexcept;
    template<formalism::FactKind T>
    const LiftedUnpackedAtomStorage<T>& get_atoms() const noexcept;

    planning::NumericUnpackedStorage<LiftedTag>& get_numeric_variables() noexcept;
    const planning::NumericUnpackedStorage<LiftedTag>& get_numeric_variables() const noexcept;

private:
    Index<State<LiftedTag>> m_index;

    planning::FactUnpackedStorage<LiftedTag> m_fact_storage;
    planning::AtomUnpackedStorage<LiftedTag> m_atom_storage;
    planning::NumericUnpackedStorage<LiftedTag> m_numeric_storage;
};

static_assert(UnpackedStateConcept<UnpackedState<LiftedTag>, LiftedTag>);

inline auto UnpackedState<LiftedTag>::get_fluent_facts_view(const formalism::planning::Repository& repository_) const noexcept
{
    return get_fluent_facts() | std::views::transform([repository = &repository_](auto id) { return make_view(id, *repository); });
}

inline auto UnpackedState<LiftedTag>::get_derived_atoms_view(const formalism::planning::Repository& repository_) const noexcept
{
    return get_derived_atoms() | std::views::transform([repository = &repository_](auto id) { return make_view(id, *repository); });
}

inline auto UnpackedState<LiftedTag>::get_fluent_fterm_values_view(const formalism::planning::Repository& repository_) const noexcept
{
    return get_fluent_fterm_values()
           | std::views::transform([repository = &repository_](auto&& pair) { return std::make_pair(make_view(pair.first, *repository), pair.second); });
}

}

#endif
