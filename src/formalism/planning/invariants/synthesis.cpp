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
                                {
                                    // Counted invariant variables do not constrain
                                    // the action-parameter alignment.
                                    return true;
                                }

                                // Rigid invariant variables can only align with
                                // action parameters, not effect-local variables.
                                if constexpr (std::is_same_v<Rhs, ParameterIndex>)
                                {
                                    if (static_cast<uint_t>(rhs_arg) >= num_action_variables)
                                        return false;

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
                                    if (static_cast<uint_t>(rhs_arg) >= num_action_variables)
                                        return false;

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

bool is_effect_local_parameter(ParameterIndex parameter, size_t num_action_variables) { return static_cast<uint_t>(parameter) >= num_action_variables; }

ParameterIndex to_effect_local_parameter(ParameterIndex parameter, size_t num_action_variables)
{
    return ParameterIndex(static_cast<uint_t>(parameter) - num_action_variables);
}

uint_t get_effect_local_index(ParameterIndex parameter, size_t num_action_variables) { return static_cast<uint_t>(parameter) - num_action_variables; }

Data<Term> alpha_rename_effect_term(const Data<Term>& term, size_t num_action_variables, size_t fresh_base)
{
    return std::visit(
        [&](auto&& arg) -> Data<Term>
        {
            using T = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<T, ParameterIndex>)
            {
                if (!is_effect_local_parameter(arg, num_action_variables))
                    return term;

                const auto local_index = get_effect_local_index(arg, num_action_variables);
                return Data<Term>(ParameterIndex(fresh_base + local_index));
            }
            else if constexpr (std::is_same_v<T, Index<Object>>)
            {
                return term;
            }
            else
            {
                static_assert(dependent_false<T>::value, "Missing case");
            }
        },
        term.value);
}

TempAtom alpha_rename_effect_atom(const TempAtom& atom, size_t num_action_variables, size_t fresh_base)
{
    auto terms = std::vector<Data<Term>> {};
    terms.reserve(atom.terms.size());

    for (const auto& term : atom.terms)
        terms.push_back(alpha_rename_effect_term(term, num_action_variables, fresh_base));

    return TempAtom {
        .predicate = atom.predicate,
        .terms = std::move(terms),
    };
}

TempLiteral alpha_rename_effect_literal(const TempLiteral& lit, size_t num_action_variables, size_t fresh_base)
{
    return TempLiteral {
        .atom = alpha_rename_effect_atom(lit.atom, num_action_variables, fresh_base),
        .polarity = lit.polarity,
    };
}

TempLiteralList alpha_rename_effect_condition(const TempLiteralList& lits, size_t num_action_variables, size_t fresh_base)
{
    auto result = TempLiteralList {};
    result.reserve(lits.size());

    for (const auto& lit : lits)
        result.push_back(alpha_rename_effect_literal(lit, num_action_variables, fresh_base));

    return result;
}

bool is_operator_too_heavy(const TempAction& op, const Invariant& inv)
{
    size_t max_num_effect_variables = 0;
    for (const auto& eff : op.effects)
        max_num_effect_variables = std::max(max_num_effect_variables, eff.num_effect_variables);

    const size_t fresh_base_lhs = op.num_variables;
    const size_t fresh_base_rhs = op.num_variables + max_num_effect_variables + 1;

    for (const auto& eff_lhs_raw : op.effects)
    {
        const auto eff_lhs_cond = alpha_rename_effect_condition(eff_lhs_raw.condition, op.num_variables, fresh_base_lhs);

        for (const auto& eff_rhs_raw : op.effects)
        {
            const auto eff_rhs_cond = alpha_rename_effect_condition(eff_rhs_raw.condition, op.num_variables, fresh_base_rhs);

            for (const auto& atom_lhs_raw : eff_lhs_raw.add_effects)
            {
                if (!inv.predicates.contains(atom_lhs_raw.predicate))
                    continue;

                const auto atom_lhs_alpha = alpha_rename_effect_atom(atom_lhs_raw, op.num_variables, fresh_base_lhs);

                for (const auto& sigma_op : enumerate_action_alignments(inv, atom_lhs_alpha, op.num_variables))
                {
                    const auto atom_lhs = apply_substitution(atom_lhs_alpha, sigma_op);

                    if (!covers(inv, atom_lhs))
                        continue;

                    for (const auto& atom_rhs_raw : eff_rhs_raw.add_effects)
                    {
                        if (!inv.predicates.contains(atom_rhs_raw.predicate))
                            continue;

                        const auto atom_rhs_alpha = alpha_rename_effect_atom(atom_rhs_raw, op.num_variables, fresh_base_rhs);

                        const auto atom_rhs = apply_substitution(atom_rhs_alpha, sigma_op);

                        if (atom_lhs == atom_rhs)
                            continue;

                        if (!covers(inv, atom_rhs))
                            continue;

                        auto lhs = apply_substitution(op.precondition, sigma_op);

                        auto cond_lhs = apply_substitution(eff_lhs_cond, sigma_op);
                        lhs.insert(lhs.end(), cond_lhs.begin(), cond_lhs.end());

                        auto cond_rhs = apply_substitution(eff_rhs_cond, sigma_op);
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

std::optional<EffectSubstitution>
match_effect_cover_against_atom(const Invariant& inv, const TempAtom& pattern, const TempAtom& element, size_t num_action_variables)
{
    if (pattern.predicate != element.predicate)
        return std::nullopt;

    if (pattern.terms.size() != element.terms.size())
        return std::nullopt;

    EffectSubstitution sigma_eff;

    uint_t max_local_index = 0;
    bool has_local = false;

    for (const auto& term : element.terms)
    {
        std::visit(
            [&](auto&& arg)
            {
                using T = std::decay_t<decltype(arg)>;

                if constexpr (std::is_same_v<T, ParameterIndex>)
                {
                    if (is_effect_local_parameter(arg, num_action_variables))
                    {
                        has_local = true;
                        max_local_index = std::max(max_local_index, static_cast<uint_t>(to_effect_local_parameter(arg, num_action_variables)));
                    }
                }
            },
            term.value);
    }

    if (has_local)
        sigma_eff.resize(max_local_index + 1);

    std::vector<std::optional<Data<Term>>> counted_bindings(inv.num_counted_variables, std::nullopt);

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
                            const auto lhs_index = static_cast<uint_t>(lhs_arg);
                            const bool lhs_is_counted = lhs_index >= inv.num_rigid_variables;

                            if (!lhs_is_counted)
                            {
                                // Rigid invariant variable: must match exactly.
                                if constexpr (std::is_same_v<Rhs, ParameterIndex>)
                                {
                                    if (is_effect_local_parameter(rhs_arg, num_action_variables))
                                    {
                                        const auto local = to_effect_local_parameter(rhs_arg, num_action_variables);
                                        return sigma_eff.assign_or_check(local, Data<Term>(lhs_arg));
                                    }
                                    return lhs_arg == rhs_arg;
                                }
                                else if constexpr (std::is_same_v<Rhs, Index<Object>>)
                                {
                                    return false;
                                }
                                else
                                {
                                    static_assert(dependent_false<Rhs>::value, "Missing case");
                                }
                            }
                            else
                            {
                                // Counted invariant variable: any term is allowed, but repeated
                                // occurrences must stay consistent whenever we can determine that.
                                const auto counted_index = lhs_index - inv.num_rigid_variables;
                                auto& binding = counted_bindings[counted_index];

                                if constexpr (std::is_same_v<Rhs, ParameterIndex>)
                                {
                                    if (is_effect_local_parameter(rhs_arg, num_action_variables))
                                    {
                                        const auto local = to_effect_local_parameter(rhs_arg, num_action_variables);

                                        if (binding.has_value())
                                            return sigma_eff.assign_or_check(local, *binding);

                                        // Leave unconstrained for now.
                                        return true;
                                    }

                                    if (!binding.has_value())
                                    {
                                        binding = Data<Term>(rhs_arg);
                                        return true;
                                    }

                                    return *binding == Data<Term>(rhs_arg);
                                }
                                else if constexpr (std::is_same_v<Rhs, Index<Object>>)
                                {
                                    if (!binding.has_value())
                                    {
                                        binding = Data<Term>(rhs_arg);
                                        return true;
                                    }

                                    return *binding == Data<Term>(rhs_arg);
                                }
                                else
                                {
                                    static_assert(dependent_false<Rhs>::value, "Missing case");
                                }
                            }
                        }
                        else if constexpr (std::is_same_v<Lhs, Index<Object>>)
                        {
                            if constexpr (std::is_same_v<Rhs, ParameterIndex>)
                            {
                                if (is_effect_local_parameter(rhs_arg, num_action_variables))
                                {
                                    const auto local = to_effect_local_parameter(rhs_arg, num_action_variables);
                                    return sigma_eff.assign_or_check(local, Data<Term>(lhs_arg));
                                }

                                return false;
                            }
                            else if constexpr (std::is_same_v<Rhs, Index<Object>>)
                            {
                                return lhs_arg == rhs_arg;
                            }
                            else
                            {
                                static_assert(dependent_false<Rhs>::value, "Missing case");
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
            return std::nullopt;
    }

    return sigma_eff;
}

std::vector<EffectSubstitution>
enumerate_effect_renamings(const TempEffect& effect, const TempAtom& element, const Invariant& inv, const ActionSubstitution& sigma_op)
{
    auto result = std::vector<EffectSubstitution> {};

    const auto partially_renamed = apply_substitution(element, sigma_op);

    for (const auto& pattern : inv.atoms)
    {
        auto sigma_eff = match_effect_cover_against_atom(inv, pattern, partially_renamed, effect.num_action_variables);

        if (!sigma_eff.has_value())
            continue;

        const auto fully_renamed = apply_substitution(partially_renamed, *sigma_eff, effect.num_action_variables);

        if (covers(inv, fully_renamed))
            result.push_back(std::move(*sigma_eff));
    }

    return result;
}

bool is_add_effect_unbalanced(const TempAction& op, const TempEffect& effect, const TempAtom& add_atom, const Invariant& inv)
{
    for (const auto& sigma_op : enumerate_action_alignments(inv, add_atom, op.num_variables))
    {
        const auto e_atom = apply_substitution(add_atom, sigma_op);

        auto lhs = apply_substitution(op.precondition, sigma_op);
        auto e_cond = apply_substitution(effect.condition, sigma_op);
        lhs.insert(lhs.end(), e_cond.begin(), e_cond.end());
        lhs.push_back(TempLiteral { .atom = e_atom, .polarity = false });

        for (const auto& del_eff : op.effects)
        {
            for (const auto& eprime_raw : del_eff.del_effects)
            {
                const auto eprime_partial = apply_substitution(eprime_raw, sigma_op);

                for (const auto& sigma_eff : enumerate_effect_renamings(del_eff, eprime_raw, inv, sigma_op))
                {
                    const auto eprime = apply_substitution(eprime_partial, sigma_eff, del_eff.num_action_variables);

                    if (e_atom == eprime)
                        continue;

                    if (!covers(inv, eprime))
                        continue;

                    auto rhs = apply_substitution(del_eff.condition, sigma_op);
                    rhs = apply_substitution(rhs, sigma_eff, del_eff.num_action_variables);
                    rhs.push_back(TempLiteral { .atom = eprime, .polarity = true });

                    if (entails(lhs, rhs))
                        return false;
                }
            }
        }
    }

    return true;
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

                if (!inv.predicates.contains(atom.predicate))
                    continue;

                if (is_add_effect_unbalanced(op, conj_eff, atom, inv))
                {
                    std::cout << atom << std::endl;
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

            const auto singleton = Invariant(inv.num_rigid_variables, inv.num_counted_variables, TempAtomList { inv.atoms[j] });

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

void normalize_invariant(Invariant& inv)
{
    remove_covered_atoms(inv);
    remove_unused_parameters(inv);
    inv = Invariant(inv.num_rigid_variables, inv.num_counted_variables, std::move(inv.atoms));
}

bool has_consistent_rigid_role_pattern(const Invariant& inv)
{
    if (inv.atoms.empty())
        return true;

    // For each rigid variable p, remember in which predicate/argument positions
    // it occurs. For the invariant families from the paper, inconsistent swaps
    // like in(V1,V0) versus in(V0,V1) should be rejected.
    for (size_t rigid = 0; rigid < inv.num_rigid_variables; ++rigid)
    {
        std::map<PredicateView<FluentTag>, std::vector<size_t>> positions_by_predicate;

        for (const auto& atom : inv.atoms)
        {
            std::vector<size_t> positions;

            for (size_t i = 0; i < atom.terms.size(); ++i)
            {
                const auto& term = atom.terms[i];
                std::visit(
                    [&](auto&& arg)
                    {
                        using T = std::decay_t<decltype(arg)>;
                        if constexpr (std::is_same_v<T, ParameterIndex>)
                        {
                            if (static_cast<uint_t>(arg) == rigid)
                                positions.push_back(i);
                        }
                    },
                    term.value);
            }

            auto it = positions_by_predicate.find(atom.predicate);
            if (it == positions_by_predicate.end())
            {
                positions_by_predicate.emplace(atom.predicate, std::move(positions));
            }
            else
            {
                if (it->second != positions)
                    return false;
            }
        }
    }

    return true;
}

bool is_well_shaped_invariant(const Invariant& inv) { return has_consistent_rigid_role_pattern(inv); }

InvariantList refine_candidate(const Invariant& inv, const Threat& threat, const TempActionList& ops, PredicateListView<FluentTag> predicates)
{
    InvariantList result;

    const auto& op = ops[threat.op_index];
    const auto& effect = op.effects[threat.effect_index];
    const auto& add_atom = effect.add_effects[threat.add_index];

    auto candidate_predicates = PredicateViewList<FluentTag> {};
    candidate_predicates.reserve(predicates.size());

    for (const auto predicate : predicates)
        candidate_predicates.push_back(predicate);

    for (const auto predicate : candidate_predicates)
    {
        const auto arity = static_cast<size_t>(predicate.get_arity());

        if (arity == inv.num_rigid_variables)
        {
            auto phi_prime = make_refinement_atom(predicate, inv.num_rigid_variables, std::nullopt);

            if (!covers(inv, phi_prime))
            {
                auto atoms = inv.atoms;
                atoms.push_back(std::move(phi_prime));

                auto refined = Invariant(inv.num_rigid_variables, inv.num_counted_variables, std::move(atoms));

                normalize_invariant(refined);

                if (!is_well_shaped_invariant(refined))
                    continue;

                if (!is_add_effect_unbalanced(op, effect, add_atom, refined))
                {
                    result.push_back(std::move(refined));
                }
            }
        }

        if (arity == inv.num_rigid_variables + 1)
        {
            for (size_t counted_position = 0; counted_position < arity; ++counted_position)
            {
                auto phi_prime = make_refinement_atom(predicate, inv.num_rigid_variables, counted_position);

                if (covers(inv, phi_prime))
                    continue;

                auto atoms = inv.atoms;
                atoms.push_back(std::move(phi_prime));

                auto refined = Invariant(inv.num_rigid_variables, std::max<size_t>(inv.num_counted_variables, 1), std::move(atoms));

                normalize_invariant(refined);

                if (!is_well_shaped_invariant(refined))
                    continue;

                if (!is_add_effect_unbalanced(op, effect, add_atom, refined))
                {
                    result.push_back(std::move(refined));
                }
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

        result.emplace_back(arity, 0, TempAtomList { make_initial_atom(predicate, arity) });

        for (size_t counted_position = 0; counted_position < arity; ++counted_position)
            result.emplace_back(arity - 1, 1, TempAtomList { make_initial_atom(predicate, counted_position) });
    }

    return result;
}

std::optional<InvariantSubstitution> match_invariant_against_ground_atom(const Invariant& inv, const TempAtom& pattern, const TempAtom& ground_atom)
{
    if (pattern.predicate != ground_atom.predicate)
        return std::nullopt;

    if (pattern.terms.size() != ground_atom.terms.size())
        return std::nullopt;

    InvariantSubstitution sigma(inv.num_rigid_variables + inv.num_counted_variables);

    for (uint_t i = 0; i < pattern.terms.size(); ++i)
    {
        const auto& lhs = pattern.terms[i];
        const auto& rhs = ground_atom.terms[i];

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
                            // In the initial state, both rigid and counted variables
                            // may bind to concrete objects, but repeated occurrences
                            // must stay consistent.
                            return sigma.assign_or_check(lhs_arg, rhs);
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

    return sigma;
}

bool covered_atoms_conflict_in_initial_state(const Invariant& inv, const TempAtom& lhs, const TempAtom& rhs)
{
    for (const auto& pattern_lhs : inv.atoms)
    {
        auto sigma_lhs = match_invariant_against_ground_atom(inv, pattern_lhs, lhs);
        if (!sigma_lhs.has_value())
            continue;

        for (const auto& pattern_rhs : inv.atoms)
        {
            auto sigma_rhs = match_invariant_against_ground_atom(inv, pattern_rhs, rhs);
            if (!sigma_rhs.has_value())
                continue;

            bool compatible = true;

            for (size_t p = 0; p < inv.num_rigid_variables; ++p)
            {
                const auto& lhs_binding = sigma_lhs->get(ParameterIndex(p));
                const auto& rhs_binding = sigma_rhs->get(ParameterIndex(p));

                if (lhs_binding.has_value() && rhs_binding.has_value() && *lhs_binding != *rhs_binding)
                {
                    compatible = false;
                    break;
                }
            }

            if (compatible)
                return true;
        }
    }

    return false;
}

inline TempAtom make_temp_ground_atom(GroundAtomView<FluentTag> element)
{
    auto terms = std::vector<Data<Term>> {};
    const auto objects = element.get_row().get_objects();
    terms.reserve(objects.size());

    for (const auto object : objects)
        terms.emplace_back(Data<Term>(object.get_index()));

    return TempAtom {
        .predicate = element.get_predicate(),
        .terms = std::move(terms),
    };
}

bool holds_in_initial_state(const Invariant& inv, GroundAtomListView<FluentTag> initial_atoms)
{
    TempAtomList covered_atoms;
    covered_atoms.reserve(initial_atoms.size());

    for (const auto atom_view : initial_atoms)
    {
        auto atom = make_temp_ground_atom(atom_view);

        for (const auto& pattern : inv.atoms)
        {
            if (match_invariant_against_ground_atom(inv, pattern, atom).has_value())
            {
                covered_atoms.push_back(std::move(atom));
                break;
            }
        }
    }

    for (size_t i = 0; i < covered_atoms.size(); ++i)
    {
        for (size_t j = i + 1; j < covered_atoms.size(); ++j)
        {
            if (covered_atoms_conflict_in_initial_state(inv, covered_atoms[i], covered_atoms[j]))
                return false;
        }
    }

    return true;
}

}

/**
 * Main loop
 */

InvariantList synthesize_invariants(TaskView task)
{
    auto ops = make_temp_actions(task.get_domain().get_actions());
    auto queue = make_initial_candidates(task.get_domain().get_predicates<FluentTag>());

    auto accepted = InvariantList {};
    auto seen = UnorderedSet<Invariant> {};

    while (!queue.empty())
    {
        auto candidate = std::move(queue.back());
        queue.pop_back();

        candidate = Invariant(candidate.num_rigid_variables, candidate.num_counted_variables, std::move(candidate.atoms));

        normalize_invariant(candidate);

        if (!is_well_shaped_invariant(candidate))
            continue;

        if (!seen.insert(candidate).second)
            continue;

        if (!holds_in_initial_state(candidate, task.get_atoms<FluentTag>()))
            continue;

        auto result = prove_invariant(candidate, ops);

        if (result.status == ProofStatus::Proven)
        {
            accepted.push_back(std::move(candidate));
        }
        else if (result.status == ProofStatus::UnbalancedAddEffect)
        {
            auto refined = refine_candidate(candidate, *result.threat, ops, task.get_domain().get_predicates<FluentTag>());

            queue.insert(queue.end(), std::make_move_iterator(refined.begin()), std::make_move_iterator(refined.end()));
        }
    }

    return accepted;
}
}
