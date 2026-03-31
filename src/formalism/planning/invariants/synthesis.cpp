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

#include "tyr/formalism/planning/invariants/synthesis.hpp"

#include "tyr/common/comparators.hpp"
#include "tyr/common/declarations.hpp"
#include "tyr/common/equal_to.hpp"
#include "tyr/common/hash.hpp"
#include "tyr/formalism/planning/expression_arity.hpp"
#include "tyr/formalism/planning/grounder.hpp"
#include "tyr/formalism/planning/invariants/formatter.hpp"
#include "tyr/formalism/planning/invariants/invariant.hpp"
#include "tyr/formalism/planning/planning_task.hpp"
#include "tyr/formalism/planning/repository.hpp"

#include <algorithm>
#include <map>
#include <optional>
#include <unordered_set>
#include <variant>
#include <vector>

namespace tyr::formalism::planning::invariant
{
namespace
{

/**
 * Common
 */

std::optional<InvariantSubstitution> match_cover_against_atom(const Invariant& inv, const TempAtom& pattern, const TempAtom& element)
{
    if (pattern.predicate != element.predicate)
        return std::nullopt;

    if (pattern.terms.size() != element.terms.size())
        return std::nullopt;

    InvariantSubstitution substitution(inv.num_rigid_variables + inv.num_counted_variables);

    for (uint_t i = 0; i < pattern.terms.size(); ++i)
    {
        const auto& lhs = pattern.terms[i];
        const auto& rhs = element.terms[i];

        const bool ok = std::visit(
            [&](auto&& lhs_arg) -> bool
            {
                using Lhs = std::decay_t<decltype(lhs_arg)>;

                return std::visit(
                    [&](auto&& rhs_arg) -> bool
                    {
                        using Rhs = std::decay_t<decltype(rhs_arg)>;

                        if constexpr (std::is_same_v<Lhs, ParameterIndex>)
                        {
                            const bool is_counted = (static_cast<uint_t>(lhs_arg) >= inv.num_rigid_variables);

                            if (is_counted)
                                return substitution.assign_or_check(lhs_arg, rhs);

                            if constexpr (std::is_same_v<Rhs, ParameterIndex>)
                                return lhs_arg == rhs_arg;
                            else
                                return false;
                        }
                        else if constexpr (std::is_same_v<Lhs, Index<Object>>)
                        {
                            if constexpr (std::is_same_v<Rhs, Index<Object>>)
                                return lhs_arg == rhs_arg;
                            else
                                return false;
                        }
                        else
                        {
                            static_assert(dependent_false<Lhs>::value, "Missing case");
                        }
                    },
                    rhs.value);
            },
            lhs.value);

        if (!ok)
            return std::nullopt;
    }

    return substitution;
}

// Fig 7
bool covers(const Invariant& inv, const TempAtom& element)
{
    for (const auto& atom : inv.atoms)
    {
        if (match_cover_against_atom(inv, atom, element).has_value())
            return true;
    }

    return false;
}

/**
 * Prove invariant
 */

std::vector<ActionSubstitution> enumerate_action_alignments(const Invariant& inv, const TempAtom& element, size_t num_action_variables)
{
    auto result = std::vector<ActionSubstitution> {};

    for (const auto& pattern : inv.atoms)
    {
        if (pattern.predicate != element.predicate)
            continue;

        if (pattern.terms.size() != element.terms.size())
            continue;

        ActionSubstitution sigma(num_action_variables);
        bool mismatch = false;

        for (uint_t i = 0; i < pattern.terms.size(); ++i)
        {
            const auto& lhs = pattern.terms[i];  // invariant pattern term
            const auto& rhs = element.terms[i];  // action/effect term

            const bool ok = std::visit(
                [&](auto&& lhs_arg) -> bool
                {
                    using Lhs = std::decay_t<decltype(lhs_arg)>;

                    return std::visit(
                        [&](auto&& rhs_arg) -> bool
                        {
                            using Rhs = std::decay_t<decltype(rhs_arg)>;

                            if constexpr (std::is_same_v<Lhs, ParameterIndex>)
                            {
                                const bool is_counted = (static_cast<uint_t>(lhs_arg) >= inv.num_rigid_variables);

                                if (is_counted)
                                {
                                    // Counted invariant variables do not constrain
                                    // the action-side alignment.
                                    return true;
                                }

                                // Rigid invariant variables must match exactly.
                                if constexpr (std::is_same_v<Rhs, ParameterIndex>)
                                {
                                    return sigma.assign_or_check(rhs_arg, Data<Term>(lhs_arg));
                                }
                                else
                                {
                                    return false;
                                }
                            }
                            else if constexpr (std::is_same_v<Lhs, Index<Object>>)
                            {
                                if constexpr (std::is_same_v<Rhs, ParameterIndex>)
                                {
                                    return sigma.assign_or_check(rhs_arg, Data<Term>(lhs_arg));
                                }
                                else if constexpr (std::is_same_v<Rhs, Index<Object>>)
                                {
                                    return lhs_arg == rhs_arg;
                                }
                                else
                                {
                                    return false;
                                }
                            }
                            else
                            {
                                static_assert(dependent_false<Lhs>::value, "Missing case");
                            }
                        },
                        rhs.value);
                },
                lhs.value);

            if (!ok)
            {
                mismatch = true;
                break;
            }
        }

        if (!mismatch)
            result.push_back(std::move(sigma));
    }

    return result;
}

bool is_contradictory(const TempLiteral& lhs, const TempLiteral& rhs) { return lhs.atom == rhs.atom && lhs.polarity != rhs.polarity; }

bool satisfiable(const TempLiteralList& lits)
{
    for (size_t i = 0; i < lits.size(); ++i)
    {
        for (size_t j = i + 1; j < lits.size(); ++j)
        {
            if (is_contradictory(lits[i], lits[j]))
                return false;
        }
    }

    return true;
}

bool is_operator_too_heavy(const TempAction& op, const Invariant& inv)
{
    // Approximation:
    // - no explicit duplication/alpha-renaming of quantified effect-local vars yet
    // - action alignments are used for operator renaming
    // - effect-local renamings are currently identity only

    for (const auto& eff_lhs : op.effects)
    {
        for (const auto& eff_rhs : op.effects)
        {
            for (const auto& atom_lhs_raw : eff_lhs.add_effects)
            {
                if (!inv.predicates.contains(atom_lhs_raw.predicate))
                    continue;

                for (const auto& sigma_op : enumerate_action_alignments(inv, atom_lhs_raw, op.num_variables))
                {
                    const auto atom_lhs = apply_substitution(atom_lhs_raw, sigma_op);

                    for (const auto& atom_rhs_raw : eff_rhs.add_effects)
                    {
                        if (!inv.predicates.contains(atom_rhs_raw.predicate))
                            continue;

                        const auto atom_rhs = apply_substitution(atom_rhs_raw, sigma_op);

                        if (atom_lhs == atom_rhs)
                            continue;

                        if (!covers(inv, atom_rhs))
                            continue;

                        auto lhs = apply_substitution(op.precondition, sigma_op);

                        auto cond_lhs = apply_substitution(eff_lhs.condition, sigma_op);
                        lhs.insert(lhs.end(), cond_lhs.begin(), cond_lhs.end());

                        auto cond_rhs = apply_substitution(eff_rhs.condition, sigma_op);
                        lhs.insert(lhs.end(), cond_rhs.begin(), cond_rhs.end());

                        lhs.push_back(TempLiteral { .atom = atom_lhs, .polarity = false });
                        lhs.push_back(TempLiteral { .atom = atom_rhs, .polarity = false });

                        if (satisfiable(lhs))
                            return true;
                    }
                }
            }
        }
    }

    return false;
}

bool entails(const TempLiteralList& lhs, const TempLiteralList& rhs)
{
    for (const auto& rhs_lit : rhs)
    {
        const auto it = std::find_if(lhs.begin(), lhs.end(), [&](const auto& lhs_lit) { return lhs_lit == rhs_lit; });

        if (it == lhs.end())
            return false;
    }

    return true;
}

std::vector<EffectSubstitution>
enumerate_effect_renamings(const TempEffect& effect, const TempAtom& element, const Invariant& inv, const ActionSubstitution& sigma_op)
{
    auto result = std::vector<EffectSubstitution> {};

    // First approximation: identity-only renaming of effect-local variables.
    // This is consistent once effect-local variables live in the shifted space
    // [num_action_variables, num_action_variables + num_effect_variables).
    EffectSubstitution sigma_eff(effect.num_effect_variables);

    // Optional pruning: only keep the identity renaming if the renamed atom
    // is at least coverable by the invariant.
    const auto renamed = apply_substitution(apply_substitution(element, sigma_op), sigma_eff, effect.num_action_variables);

    if (covers(inv, renamed))
        result.push_back(std::move(sigma_eff));

    return result;
}

bool is_add_effect_unbalanced(const TempAction& op, const TempEffect& effect, const TempAtom& add_atom, const Invariant& inv)
{
    // Enumerate action-space renamings that make the chosen add atom align with the invariant.
    for (const auto& sigma_op : enumerate_action_alignments(inv, add_atom, op.num_variables))
    {
        const auto e_atom = apply_substitution(add_atom, sigma_op);

        // lhs = o'.precond ∧ e.cond ∧ ¬e.atom
        auto lhs = apply_substitution(op.precondition, sigma_op);
        auto e_cond = apply_substitution(effect.condition, sigma_op);
        lhs.insert(lhs.end(), e_cond.begin(), e_cond.end());
        lhs.push_back(TempLiteral { .atom = e_atom, .polarity = false });

        // Try every delete effect e' of o'
        for (const auto& del_eff : op.effects)
        {
            for (const auto& eprime_raw : del_eff.del_effects)
            {
                for (const auto& sigma_eff : enumerate_effect_renamings(del_eff, eprime_raw, inv, sigma_op))
                {
                    const auto eprime = apply_substitution(apply_substitution(eprime_raw, sigma_op), sigma_eff, del_eff.num_action_variables);

                    if (e_atom == eprime)
                        continue;

                    if (!covers(inv, eprime))
                        continue;

                    auto rhs = apply_substitution(del_eff.condition, sigma_op);
                    rhs = apply_substitution(rhs, sigma_eff, del_eff.num_action_variables);
                    rhs.push_back(TempLiteral { .atom = eprime, .polarity = true });

                    if (entails(lhs, rhs))
                        return false;  // e' balances e
                }
            }
        }
    }

    return true;  // no balancing delete effect found
}

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

ProofResult prove_invariant(const Invariant& inv, const TempActionList& ops)
{
    for (size_t op_index = 0; op_index < ops.size(); ++op_index)
    {
        const auto& op = ops[op_index];

        if (is_operator_too_heavy(op, inv))
            return { ProofStatus::TooHeavy, Threat { op_index, 0, 0 } };  // { Reject the candidate. }

        for (size_t effect_index = 0; effect_index < op.effects.size(); ++effect_index)
        {
            const auto& conj_eff = op.effects[effect_index];

            for (size_t add_index = 0; add_index < conj_eff.add_effects.size(); ++add_index)
            {
                const auto& atom = conj_eff.add_effects[add_index];
                {
                    if (is_add_effect_unbalanced(op, conj_eff, atom, inv))
                        return { ProofStatus::UnbalancedAddEffect, Threat { op_index, effect_index, add_index } };  // { Reject the candidate. }
                }
            }
        }
    }

    return { ProofStatus::Proven, std::nullopt };  // { Accept the candidate. }
}

/**
 * Refine
 */

bool uses_parameter(const Data<Term>& term, ParameterIndex parameter)
{
    return std::visit(
        [&](auto&& arg) -> bool
        {
            using T = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<T, ParameterIndex>)
                return arg == parameter;
            else if constexpr (std::is_same_v<T, Index<Object>>)
                return false;
            else
                static_assert(dependent_false<T>::value, "Missing case");
        },
        term.value);
}

bool uses_parameter(const TempAtom& atom, ParameterIndex parameter)
{
    return std::find_if(atom.terms.begin(), atom.terms.end(), [&](const auto& term) { return uses_parameter(term, parameter); }) != atom.terms.end();
}

bool uses_parameter(const Invariant& inv, ParameterIndex parameter)
{
    return std::find_if(inv.atoms.begin(), inv.atoms.end(), [&](const auto& atom) { return uses_parameter(atom, parameter); }) != inv.atoms.end();
}

void remove_covered_atoms(Invariant& inv)
{
    TempAtomList kept;
    kept.reserve(inv.atoms.size());

    for (size_t i = 0; i < inv.atoms.size(); ++i)
    {
        bool covered_by_other = false;

        for (size_t j = 0; j < inv.atoms.size(); ++j)
        {
            if (i == j)
                continue;

            Invariant singleton {};
            singleton.num_rigid_variables = inv.num_rigid_variables;
            singleton.num_counted_variables = inv.num_counted_variables;
            singleton.atoms.push_back(inv.atoms[j]);
            singleton.predicates.insert(inv.atoms[j].predicate);

            if (covers(singleton, inv.atoms[i]))
            {
                covered_by_other = true;
                break;
            }
        }

        if (!covered_by_other)
            kept.push_back(inv.atoms[i]);
    }

    inv.atoms = std::move(kept);

    inv.predicates.clear();
    for (const auto& atom : inv.atoms)
        inv.predicates.insert(atom.predicate);
}

void remove_unused_parameters(Invariant& inv)
{
    const size_t total = inv.num_rigid_variables + inv.num_counted_variables;
    std::vector<bool> used(total, false);

    for (size_t p = 0; p < total; ++p)
        used[p] = uses_parameter(inv, ParameterIndex(p));

    std::vector<std::optional<ParameterIndex>> remap(total, std::nullopt);
    size_t next = 0;
    size_t new_num_rigid = 0;

    for (size_t p = 0; p < total; ++p)
    {
        if (!used[p])
            continue;

        remap[p] = ParameterIndex(next);
        if (p < inv.num_rigid_variables)
            ++new_num_rigid;
        ++next;
    }

    for (auto& atom : inv.atoms)
    {
        for (auto& term : atom.terms)
        {
            term = std::visit(
                [&](auto&& arg) -> Data<Term>
                {
                    using T = std::decay_t<decltype(arg)>;

                    if constexpr (std::is_same_v<T, ParameterIndex>)
                        return Data<Term>(*remap[static_cast<uint_t>(arg)]);
                    else if constexpr (std::is_same_v<T, Index<Object>>)
                        return term;
                    else
                        static_assert(dependent_false<T>::value, "Missing case");
                },
                term.value);
        }
    }

    inv.num_rigid_variables = new_num_rigid;
    inv.num_counted_variables = next - new_num_rigid;
}

TempAtom make_refinement_atom(PredicateView<FluentTag> predicate, size_t num_rigid_variables, std::optional<size_t> counted_position)
{
    const auto arity = static_cast<size_t>(predicate.get_arity());
    auto terms = std::vector<Data<Term>> {};
    terms.reserve(arity);

    const bool has_counted = counted_position.has_value();
    const auto counted_index = ParameterIndex(num_rigid_variables);

    for (size_t i = 0; i < arity; ++i)
    {
        if (has_counted && i == *counted_position)
        {
            terms.emplace_back(Data<Term>(counted_index));
        }
        else
        {
            size_t rigid_slot = i;
            if (has_counted && i > *counted_position)
                rigid_slot -= 1;

            terms.emplace_back(Data<Term>(ParameterIndex(rigid_slot)));
        }
    }

    return TempAtom {
        .predicate = predicate,
        .terms = std::move(terms),
    };
}

InvariantList refine_candidate(const Invariant& inv, const Threat& threat, const TempActionList& ops)
{
    InvariantList result;

    const auto& op = ops[threat.op_index];
    const auto& effect = op.effects[threat.effect_index];
    const auto& add_atom = effect.add_effects[threat.add_index];

    // Enumerate candidate refinement atoms.
    // For now, use predicates from the threatening operator.
    UnorderedSet<PredicateView<FluentTag>> candidate_predicates;

    for (const auto& eff : op.effects)
    {
        for (const auto& atom : eff.add_effects)
            candidate_predicates.insert(atom.predicate);
        for (const auto& atom : eff.del_effects)
            candidate_predicates.insert(atom.predicate);
    }

    for (const auto predicate : candidate_predicates)
    {
        const auto arity = static_cast<size_t>(predicate.get_arity());

        // Case 1: no extra counted variable
        if (arity == inv.num_rigid_variables)
        {
            auto phi_prime = make_refinement_atom(predicate, inv.num_rigid_variables, std::nullopt);

            if (!covers(inv, phi_prime))
            {
                auto refined = inv;
                refined.atoms.push_back(std::move(phi_prime));
                refined.predicates.insert(predicate);

                remove_covered_atoms(refined);
                remove_unused_parameters(refined);

                if (!is_add_effect_unbalanced(op, effect, add_atom, refined))
                    result.push_back(std::move(refined));
            }
        }

        // Case 2: exactly one extra counted variable
        if (arity == inv.num_rigid_variables + 1)
        {
            for (size_t counted_position = 0; counted_position < arity; ++counted_position)
            {
                auto phi_prime = make_refinement_atom(predicate, inv.num_rigid_variables, counted_position);

                if (covers(inv, phi_prime))
                    continue;

                auto refined = inv;
                refined.atoms.push_back(std::move(phi_prime));
                refined.predicates.insert(predicate);

                refined.num_counted_variables = std::max<size_t>(refined.num_counted_variables, 1);

                remove_covered_atoms(refined);
                remove_unused_parameters(refined);

                if (!is_add_effect_unbalanced(op, effect, add_atom, refined))
                    result.push_back(std::move(refined));
            }
        }
    }

    return result;
}

/**
 * Initial Candidates
 */

TempAtom make_initial_atom(PredicateView<FluentTag> predicate, size_t counted_position)
{
    const auto arity = static_cast<size_t>(predicate.get_arity());
    auto terms = std::vector<Data<Term>> {};
    terms.reserve(arity);

    if (counted_position == arity)
    {
        for (size_t i = 0; i < arity; ++i)
            terms.emplace_back(Data<Term>(ParameterIndex(i)));
    }
    else
    {
        const auto counted_index = ParameterIndex(arity - 1);

        for (size_t i = 0; i < arity; ++i)
        {
            if (i == counted_position)
            {
                terms.emplace_back(Data<Term>(counted_index));
            }
            else
            {
                const auto rigid_index = (i < counted_position) ? ParameterIndex(i) : ParameterIndex(i - 1);
                terms.emplace_back(Data<Term>(rigid_index));
            }
        }
    }

    return TempAtom {
        .predicate = predicate,
        .terms = std::move(terms),
    };
}

InvariantList make_initial_candidates(PredicateListView<FluentTag> predicates)
{
    auto result = InvariantList {};

    for (const auto predicate : predicates)
    {
        const auto arity = static_cast<size_t>(predicate.get_arity());

        {
            Invariant inv {};
            inv.num_rigid_variables = arity;
            inv.num_counted_variables = 0;
            inv.atoms.push_back(make_initial_atom(predicate, arity));
            inv.predicates.insert(predicate);
            result.push_back(std::move(inv));
        }

        for (size_t counted_position = 0; counted_position < arity; ++counted_position)
        {
            Invariant inv {};
            inv.num_rigid_variables = arity - 1;
            inv.num_counted_variables = 1;
            inv.atoms.push_back(make_initial_atom(predicate, counted_position));
            inv.predicates.insert(predicate);
            result.push_back(std::move(inv));
        }
    }

    return result;
}
}

/**
 * Main loop
 */

InvariantList synthesize_invariants(TaskView task)
{
    auto ops = make_temp_actions(task.get_domain().get_actions());

    auto queue = make_initial_candidates(task.get_domain().get_predicates<FluentTag>());

    std::cout << "[Total] Initial candidates: " << queue.size() << std::endl;
    print(std::cout, queue);
    std::cout << std::endl;

    auto accepted = InvariantList {};

    auto seen = UnorderedSet<Invariant> {};

    while (!queue.empty())
    {
        auto candidate = std::move(queue.back());
        queue.pop_back();

        if (!seen.insert(candidate).second)
            continue;

        auto result = prove_invariant(candidate, ops);

        if (result.status == ProofStatus::Proven)
        {
            accepted.push_back(std::move(candidate));
        }
        else if (result.status == ProofStatus::UnbalancedAddEffect)
        {
            auto refined = refine_candidate(candidate, *result.threat, ops);
            queue.insert(queue.end(), std::make_move_iterator(refined.begin()), std::make_move_iterator(refined.end()));
        }
    }

    return accepted;
}
}
