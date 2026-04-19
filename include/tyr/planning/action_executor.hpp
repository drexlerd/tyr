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

#ifndef TYR_PLANNING_ACTION_EXECUTOR_HPP_
#define TYR_PLANNING_ACTION_EXECUTOR_HPP_

#include "tyr/common/declarations.hpp"
#include "tyr/common/itertools.hpp"
#include "tyr/common/types.hpp"
#include "tyr/formalism/planning/builder.hpp"
#include "tyr/formalism/planning/declarations.hpp"
#include "tyr/formalism/planning/repository.hpp"
#include "tyr/planning/declarations.hpp"

namespace tyr::planning
{

class ActionExecutor
{
public:
    ActionExecutor() = default;

    // Ground action API (interning)

    template<TaskKind Kind>
    bool is_applicable(formalism::planning::GroundActionView action, const StateContext<Kind>& state);

    template<TaskKind Kind>
    Node<Kind> apply_action(const StateContext<Kind>& state_context, formalism::planning::GroundActionView action, StateRepository<Kind>& state_repository);

    // Action binding API (interning)

    bool is_applicable(formalism::planning::ActionBindingView binding, const StateContext<LiftedTag>& state);

    Node<LiftedTag>
    apply_action(const StateContext<LiftedTag>& state_context, formalism::planning::ActionBindingView binding, StateRepository<LiftedTag>& state_repository);

    // Action binding API (no interning)

    bool is_applicable(const Data<formalism::RelationBinding<formalism::planning::Action>>& binding, const StateContext<LiftedTag>& state);

    Node<LiftedTag> apply_action(const StateContext<LiftedTag>& state_context,
                                 const Data<formalism::RelationBinding<formalism::planning::Action>>& binding,
                                 StateRepository<LiftedTag>& state_repository);

private:
    DataList<formalism::planning::FDRFact<formalism::FluentTag>> m_del_effects;
    DataList<formalism::planning::FDRFact<formalism::FluentTag>> m_add_effects;
    formalism::planning::EffectFamilyList m_effect_families;
    itertools::cartesian_set::Workspace<Index<formalism::Object>> m_cartesian_workspace;
    IndexList<formalism::Object> m_binding;
    formalism::planning::Builder m_builder;
};
}

#endif
