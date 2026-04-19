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

#ifndef TYR_PLANNING_APPLICABILITY_LIFTED_DECL_HPP_
#define TYR_PLANNING_APPLICABILITY_LIFTED_DECL_HPP_

#include "tyr/common/semantics.hpp"
#include "tyr/formalism/planning/fdr_context.hpp"
#include "tyr/formalism/planning/grounder_decl.hpp"
#include "tyr/planning/declarations.hpp"
#include "tyr/planning/state_accessor.hpp"

namespace tyr::planning
{
template<TaskKind Kind, SemanticTag S>
struct LiftedApplicabilityContext
{
    const StateAccessor<Kind, S>& state;
    formalism::planning::GrounderContext& grounder;
    const formalism::planning::FDRContext& fdr;
};
}

#endif
