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

#ifndef TYR_FORMALISM_PLANNING_INVARIANTS_MATCHING_HPP_
#define TYR_FORMALISM_PLANNING_INVARIANTS_MATCHING_HPP_

#include "tyr/formalism/planning/invariants/invariant.hpp"

namespace tyr::formalism::planning::invariant
{

struct ActionAlignment
{
    TempAtom pattern;
    ActionSubstitution sigma;

    friend bool operator==(const ActionAlignment&, const ActionAlignment&) = default;
};

bool covers(const Invariant& inv, const TempAtom& element);

std::optional<InvariantSubstitution> match_invariant_against_ground_atom(const Invariant& inv, const TempAtom& pattern, const TempAtom& ground_atom);

std::vector<ActionAlignment> enumerate_action_alignments(const Invariant& inv, const TempAtom& element, size_t num_action_variables);

std::vector<EffectSubstitution>
enumerate_effect_renamings(const TempEffect& effect, const TempAtom& element, const Invariant& inv, const ActionSubstitution& sigma_op);

}  // namespace tyr::formalism::planning::invariant

#endif