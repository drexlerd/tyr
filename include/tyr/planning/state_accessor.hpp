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

#ifndef TYR_PLANNING_STATE_ACCESSOR_HPP_
#define TYR_PLANNING_STATE_ACCESSOR_HPP_

#include "tyr/common/semantics.hpp"
#include "tyr/formalism/planning/grounder_decl.hpp"
#include "tyr/planning/applicability_decl.hpp"
#include "tyr/planning/declarations.hpp"
#include "tyr/planning/ground_task/unpacked_state.hpp"
#include "tyr/planning/lifted_task/unpacked_state.hpp"

namespace tyr::planning
{
template<TaskKind Kind, SemanticTag S>
struct StateAccessor
{
    const StateContext<Kind>& context;

    template<formalism::FactKind T>
    bool test(formalism::planning::GroundAtomView<T> atom) const
    {
        return context.unpacked_state.test(atom.get_index());
    }

    bool test(formalism::planning::FDRFactView<formalism::FluentTag> fact) const
    {
        assert(!fact.get_value().is_none());

        return context.unpacked_state.get(fact.get_variable().get_index()) == fact.get_value();
    }

    template<formalism::FactKind T>
    float_t get(formalism::planning::GroundFunctionTermView<T> fterm) const
    {
        return context.unpacked_state.get(fterm.get_index());
    }
};

template<TaskKind Kind>
struct StateAccessor<Kind, CareSemanticTag>
{
    const StateContext<Kind>& context;
    const formalism::planning::CareSet& care_set;

    template<formalism::FactKind T>
    bool test(formalism::planning::GroundAtomView<T> atom) const
    {
        return context.unpacked_state.test(atom.get_index());
    }

    bool test(formalism::planning::FDRFactView<formalism::FluentTag> fact) const
    {
        assert(!fact.get_value().is_none());

        if (!care_set.predicate_bindings.contains(fact.get_atom().value().get_row()))
            return true;

        return context.unpacked_state.get(fact.get_variable().get_index()) == fact.get_value();
    }

    template<formalism::FactKind T>
    float_t get(formalism::planning::GroundFunctionTermView<T> fterm) const
    {
        return context.unpacked_state.get(fterm.get_index());
    }
};

}

#endif
