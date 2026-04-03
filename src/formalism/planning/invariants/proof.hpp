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

#ifndef TYR_FORMALISM_PLANNING_INVARIANTS_PROOF_HPP_
#define TYR_FORMALISM_PLANNING_INVARIANTS_PROOF_HPP_

#include "tyr/formalism/planning/invariants/invariant.hpp"

namespace tyr::formalism::planning::invariant
{

bool is_operator_too_heavy(const TempAction& op, const Invariant& inv);

bool is_add_effect_unbalanced(const TempAction& op, const TempEffect& effect, const TempAtom& add_atom, const Invariant& inv);

struct Threat
{
    size_t op_index;
    size_t effect_index;
    size_t add_index;
};

enum class ProofStatus
{
    Proven,
    TooHeavy,
    UnbalancedAddEffect
};

struct ProofResult
{
    ProofStatus status;
    std::optional<Threat> threat;
};

ProofResult prove_invariant(const Invariant& inv, const TempActionList& ops);
}

#endif