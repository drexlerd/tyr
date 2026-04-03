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

#include "proof.hpp"

#include "constraints.hpp"

#include <algorithm>
#include <map>
#include <optional>
#include <vector>

namespace tyr::formalism::planning::invariant
{
namespace
{
struct EffectLiteralRef
{
    const TempEffect* effect;
    const TempAtom* atom;
    bool negated;
};

bool is_effect_local_parameter(ParameterIndex parameter, size_t num_action_variables) { return static_cast<uint_t>(parameter) >= num_action_variables; }

uint_t get_effect_local_index(ParameterIndex parameter, size_t num_action_variables) { return static_cast<uint_t>(parameter) - num_action_variables; }

const TempAtom* find_part(const Invariant& inv, PredicateView<FluentTag> predicate)
{
    const auto it = std::find_if(inv.atoms.begin(), inv.atoms.end(), [&](const auto& atom) { return atom.predicate == predicate; });

    if (it == inv.atoms.end())
        return nullptr;

    return &(*it);
}

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
    std::vector<Data<Term>> terms;
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
    TempLiteralList result;
    result.reserve(lits.size());

    for (const auto& lit : lits)
        result.push_back(alpha_rename_effect_literal(lit, num_action_variables, fresh_base));

    return result;
}

EqualityConjunction make_cover_equality_conjunction(const TempAtom& pattern, const TempAtom& atom, const Invariant& inv)
{
    assert(pattern.predicate == atom.predicate);
    assert(pattern.terms.size() == atom.terms.size());

    std::vector<std::pair<ConstraintTerm, ConstraintTerm>> equalities;

    for (size_t pos = 0; pos < pattern.terms.size(); ++pos)
    {
        std::visit(
            [&](auto&& x)
            {
                using T = std::decay_t<decltype(x)>;

                if constexpr (std::is_same_v<T, ParameterIndex>)
                {
                    const auto idx = static_cast<uint_t>(x);
                    if (idx < inv.num_rigid_variables)
                        equalities.emplace_back(make_invariant_parameter_term(idx), make_constraint_term(atom.terms[pos]));
                }
            },
            pattern.terms[pos].value);
    }

    return EqualityConjunction(std::move(equalities));
}

std::vector<EffectLiteralRef> collect_relevant_add_effects(const TempAction& op, const Invariant& inv)
{
    std::vector<EffectLiteralRef> result;

    for (const auto& eff : op.effects)
    {
        for (const auto& atom : eff.add_effects)
        {
            if (inv.predicates.contains(atom.predicate))
                result.push_back(EffectLiteralRef { .effect = &eff, .atom = &atom, .negated = false });
        }
    }

    return result;
}

std::vector<EffectLiteralRef> collect_relevant_del_effects(const TempAction& op, const Invariant& inv)
{
    std::vector<EffectLiteralRef> result;

    for (const auto& eff : op.effects)
    {
        for (const auto& atom : eff.del_effects)
        {
            if (inv.predicates.contains(atom.predicate))
                result.push_back(EffectLiteralRef { .effect = &eff, .atom = &atom, .negated = true });
        }
    }

    return result;
}

std::map<PredicateView<FluentTag>, std::vector<TempLiteral>>
build_add_effect_produced_by_pred(const TempAction& op, const TempEffect& add_effect, const TempAtom& add_atom)
{
    std::map<PredicateView<FluentTag>, std::vector<TempLiteral>> produced_by_pred;

    for (const auto& lit : op.precondition)
        produced_by_pred[lit.atom.predicate].push_back(lit);

    for (const auto& lit : add_effect.condition)
        produced_by_pred[lit.atom.predicate].push_back(lit);

    produced_by_pred[add_atom.predicate].push_back(TempLiteral { .atom = add_atom, .polarity = false });

    return produced_by_pred;
}

ConstraintSystem make_param_system(const TempAction& op, const TempEffect& add_effect, const EqualityConjunction& add_cover)
{
    ConstraintSystem param_system;
    const auto& representative = add_cover.get_representative();

    std::vector<ParameterIndex> params;
    params.reserve(op.num_variables + add_effect.num_effect_variables);

    for (size_t i = 0; i < op.num_variables; ++i)
        params.push_back(ParameterIndex(i));

    for (size_t i = 0; i < add_effect.num_effect_variables; ++i)
        params.push_back(ParameterIndex(add_effect.num_action_variables + i));

    for (const auto param : params)
    {
        const auto term = make_constraint_term(Data<Term>(param));
        const auto repr = representative.contains(term) ? representative.at(term) : term;

        if (std::holds_alternative<InvariantParameter>(repr) || std::holds_alternative<VariableTerm>(repr))
            param_system.add_not_constant(term);
    }

    for (size_t i = 0; i < params.size(); ++i)
    {
        for (size_t j = i + 1; j < params.size(); ++j)
        {
            const auto t1 = make_constraint_term(Data<Term>(params[i]));
            const auto t2 = make_constraint_term(Data<Term>(params[j]));
            const auto r1 = representative.contains(t1) ? representative.at(t1) : t1;
            const auto r2 = representative.contains(t2) ? representative.at(t2) : t2;

            if (r1 != r2)
                param_system.add_inequality_disjunction(InequalityDisjunction({ { t1, t2 } }));
        }
    }

    return param_system;
}

std::optional<ConstraintSystem> make_balance_system(const TempEffect& add_effect,
                                                    const TempAtom& add_atom,
                                                    const TempEffect& del_effect,
                                                    const TempAtom& del_atom,
                                                    const std::map<PredicateView<FluentTag>, std::vector<TempLiteral>>& produced_by_pred)
{
    ConstraintSystem system;

    TempLiteralList del_required = del_effect.condition;
    del_required.push_back(TempLiteral { .atom = del_atom, .polarity = true });

    for (const auto& lit : del_required)
    {
        std::vector<EqualityConjunction> possibilities;

        const auto it = produced_by_pred.find(lit.atom.predicate);
        if (it == produced_by_pred.end())
            return std::nullopt;

        for (const auto& match : it->second)
        {
            if (match.polarity != lit.polarity)
                continue;

            std::vector<std::pair<ConstraintTerm, ConstraintTerm>> eqs;
            eqs.reserve(lit.atom.terms.size());

            for (size_t i = 0; i < lit.atom.terms.size(); ++i)
                eqs.emplace_back(make_constraint_term(lit.atom.terms[i]), make_constraint_term(match.atom.terms[i]));

            possibilities.emplace_back(std::move(eqs));
        }

        if (possibilities.empty())
            return std::nullopt;

        system.add_equality_DNF(std::move(possibilities));
    }

    ensure_inequality(system, add_atom, del_atom);
    return system;
}

}  // namespace

bool is_operator_too_heavy(const TempAction& op, const Invariant& inv)
{
    const auto add_effects = collect_relevant_add_effects(op, inv);

    if (add_effects.size() <= 1)
        return false;

    size_t max_num_effect_variables = 0;
    for (const auto& eff : op.effects)
        max_num_effect_variables = std::max(max_num_effect_variables, eff.num_effect_variables);

    const size_t fresh_base_lhs = op.num_variables;
    const size_t fresh_base_rhs = op.num_variables + max_num_effect_variables + 1;

    for (size_t i = 0; i < add_effects.size(); ++i)
    {
        for (size_t j = i + 1; j < add_effects.size(); ++j)
        {
            const auto& eff1 = add_effects[i];
            const auto& eff2 = add_effects[j];

            const auto lhs_atom = alpha_rename_effect_atom(*eff1.atom, eff1.effect->num_action_variables, fresh_base_lhs);
            const auto rhs_atom = alpha_rename_effect_atom(*eff2.atom, eff2.effect->num_action_variables, fresh_base_rhs);

            const auto lhs_cond = alpha_rename_effect_condition(eff1.effect->condition, eff1.effect->num_action_variables, fresh_base_lhs);
            const auto rhs_cond = alpha_rename_effect_condition(eff2.effect->condition, eff2.effect->num_action_variables, fresh_base_rhs);

            const auto* lhs_pattern = find_part(inv, lhs_atom.predicate);
            const auto* rhs_pattern = find_part(inv, rhs_atom.predicate);

            if (lhs_pattern == nullptr || rhs_pattern == nullptr)
                continue;

            ConstraintSystem system;
            ensure_inequality(system, lhs_atom, rhs_atom);
            ensure_cover(system, *lhs_pattern, lhs_atom, inv);
            ensure_cover(system, *rhs_pattern, rhs_atom, inv);

            TempLiteralList conjunction = op.precondition;
            conjunction.insert(conjunction.end(), lhs_cond.begin(), lhs_cond.end());
            conjunction.insert(conjunction.end(), rhs_cond.begin(), rhs_cond.end());
            conjunction.push_back(TempLiteral { .atom = lhs_atom, .polarity = false });
            conjunction.push_back(TempLiteral { .atom = rhs_atom, .polarity = false });

            ensure_conjunction_sat(system, conjunction);

            if (system.is_solvable())
                return true;
        }
    }

    return false;
}

bool is_add_effect_unbalanced(const TempAction& op, const TempEffect& add_effect, const TempAtom& add_atom, const Invariant& inv)
{
    const auto* add_pattern = find_part(inv, add_atom.predicate);
    if (add_pattern == nullptr)
        return true;

    const auto add_cover = make_cover_equality_conjunction(*add_pattern, add_atom, inv);
    auto param_system = make_param_system(op, add_effect, add_cover);
    const auto produced_by_pred = build_add_effect_produced_by_pred(op, add_effect, add_atom);

    const auto del_effects = collect_relevant_del_effects(op, inv);

    for (const auto& del_ref : del_effects)
    {
        const auto* del_pattern = find_part(inv, del_ref.atom->predicate);
        if (del_pattern == nullptr)
            continue;

        auto balance_system = make_balance_system(add_effect, add_atom, *del_ref.effect, *del_ref.atom, produced_by_pred);
        if (!balance_system.has_value())
            continue;

        ConstraintSystem system;
        system.add_equality_conjunction(add_cover);
        ensure_cover(system, *del_pattern, *del_ref.atom, inv);
        system.extend(*balance_system);
        system.extend(param_system);

        if (system.is_solvable())
            return false;
    }

    return true;
}

ProofResult prove_invariant(const Invariant& inv, const TempActionList& ops)
{
    for (size_t op_index = 0; op_index < ops.size(); ++op_index)
    {
        const auto& op = ops[op_index];

        if (is_operator_too_heavy(op, inv))
            return { ProofStatus::TooHeavy, std::nullopt };

        for (size_t effect_index = 0; effect_index < op.effects.size(); ++effect_index)
        {
            const auto& eff = op.effects[effect_index];

            for (size_t add_index = 0; add_index < eff.add_effects.size(); ++add_index)
            {
                const auto& add_atom = eff.add_effects[add_index];

                if (!inv.predicates.contains(add_atom.predicate))
                    continue;

                if (is_add_effect_unbalanced(op, eff, add_atom, inv))
                    return { ProofStatus::UnbalancedAddEffect, Threat { op_index, effect_index, add_index } };
            }
        }
    }

    return { ProofStatus::Proven, std::nullopt };
}

}  // namespace tyr::formalism::planning::invariant