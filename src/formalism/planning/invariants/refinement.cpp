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

#include "refinement.hpp"

#include "matching.hpp"
#include "normalization.hpp"
#include "proof.hpp"

#include <algorithm>
#include <optional>
#include <vector>

namespace tyr::formalism::planning::invariant
{
namespace
{

TempAtom make_initial_atom(PredicateView<FluentTag> predicate, size_t counted_position)
{
    const auto arity = static_cast<size_t>(predicate.get_arity());

    std::vector<Data<Term>> terms;
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

bool uses_predicate(const Invariant& inv, PredicateView<FluentTag> predicate)
{
    return std::ranges::any_of(inv.atoms, [&](const auto& atom) { return atom.predicate == predicate; });
}

const TempAtom* find_matching_part(const Invariant& inv, const TempAtom& add_atom)
{
    for (const auto& part : inv.atoms)
    {
        if (part.predicate == add_atom.predicate)
            return &part;
    }
    return nullptr;
}

using PositionMapping = std::vector<std::pair<size_t, std::optional<ParameterIndex>>>;

size_t part_arity(const TempAtom& part, size_t num_rigid_variables)
{
    bool has_counted = false;

    for (const auto& term : part.terms)
    {
        std::visit(
            [&](auto&& arg)
            {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, ParameterIndex>)
                {
                    if (static_cast<uint_t>(arg) >= num_rigid_variables)
                        has_counted = true;
                }
            },
            term.value);
    }

    return has_counted ? part.terms.size() - 1 : part.terms.size();
}

std::map<ParameterIndex, Data<Term>> get_parameters_from_part(const TempAtom& part, const TempAtom& literal, size_t num_rigid_variables)
{
    std::map<ParameterIndex, Data<Term>> result;

    for (size_t pos = 0; pos < part.terms.size(); ++pos)
    {
        std::visit(
            [&](auto&& arg)
            {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, ParameterIndex>)
                {
                    if (static_cast<uint_t>(arg) < num_rigid_variables)
                        result.emplace(arg, literal.terms[pos]);
                }
            },
            part.terms[pos].value);
    }

    return result;
}

bool optional_parameter_less(const std::optional<ParameterIndex>& a, const std::optional<ParameterIndex>& b)
{
    if (!a.has_value() && !b.has_value())
        return false;
    if (!a.has_value())
        return false;
    if (!b.has_value())
        return true;
    return *a < *b;
}

void instantiate_factored_mapping_rec(const std::vector<std::pair<std::vector<size_t>, std::vector<std::optional<ParameterIndex>>>>& groups,
                                      size_t group_index,
                                      PositionMapping& current,
                                      std::vector<PositionMapping>& out)
{
    if (group_index == groups.size())
    {
        out.push_back(current);
        return;
    }

    const auto& [positions, images] = groups[group_index];

    auto perm = images;
    std::sort(perm.begin(), perm.end(), optional_parameter_less);

    do
    {
        for (size_t i = 0; i < positions.size(); ++i)
            current.push_back({ positions[i], perm[i] });

        instantiate_factored_mapping_rec(groups, group_index + 1, current, out);

        for (size_t i = 0; i < positions.size(); ++i)
            current.pop_back();
    } while (std::next_permutation(perm.begin(), perm.end(), optional_parameter_less));
}

std::vector<PositionMapping> instantiate_factored_mapping(const std::vector<std::pair<std::vector<size_t>, std::vector<std::optional<ParameterIndex>>>>& groups)
{
    std::vector<PositionMapping> result;
    PositionMapping current;
    instantiate_factored_mapping_rec(groups, 0, current, result);
    return result;
}

std::vector<PositionMapping> possible_mappings(const TempAtom& part, const TempAtom& own_literal, const TempAtom& other_literal, size_t num_rigid_variables)
{
    size_t allowed_omissions = other_literal.terms.size() - part_arity(part, num_rigid_variables);
    if (allowed_omissions > 1)
        return {};

    const auto own_parameters = get_parameters_from_part(part, own_literal, num_rigid_variables);

    std::map<Data<Term>, std::vector<ParameterIndex>> ownarg_to_invariant_parameters;
    for (const auto& [inv_param, term] : own_parameters)
        ownarg_to_invariant_parameters[term].push_back(inv_param);

    std::map<Data<Term>, std::vector<size_t>> other_arg_to_positions;
    for (size_t pos = 0; pos < other_literal.terms.size(); ++pos)
        other_arg_to_positions[other_literal.terms[pos]].push_back(pos);

    std::vector<std::pair<std::vector<size_t>, std::vector<std::optional<ParameterIndex>>>> factored_mapping;

    for (const auto& [term, other_positions] : other_arg_to_positions)
    {
        auto inv_params = ownarg_to_invariant_parameters[term];
        const int len_diff = static_cast<int>(inv_params.size()) - static_cast<int>(other_positions.size());

        if (len_diff >= 1 || len_diff <= -2 || (len_diff == -1 && !allowed_omissions))
            return {};

        std::vector<std::optional<ParameterIndex>> images;
        images.reserve(inv_params.size() + (len_diff != 0 ? 1 : 0));

        for (const auto& p : inv_params)
            images.push_back(p);

        if (len_diff != 0)
        {
            images.push_back(std::nullopt);
            allowed_omissions = 0;
        }

        factored_mapping.push_back({ other_positions, std::move(images) });
    }

    return instantiate_factored_mapping(factored_mapping);
}

std::vector<TempAtom> possible_matches(const TempAtom& part, const TempAtom& own_literal, const TempAtom& other_literal, size_t num_rigid_variables)
{
    std::vector<TempAtom> result;

    for (const auto& mapping : possible_mappings(part, own_literal, other_literal, num_rigid_variables))
    {
        std::vector<Data<Term>> args(other_literal.terms.size(), Data<Term>(ParameterIndex(num_rigid_variables)));

        for (const auto& [other_pos, inv_var] : mapping)
            args[other_pos] = Data<Term>(inv_var.value_or(ParameterIndex(num_rigid_variables)));

        result.push_back(TempAtom {
            .predicate = other_literal.predicate,
            .terms = std::move(args),
        });
    }

    return result;
}

void try_refinement(InvariantList& result,
                    const Invariant& inv,
                    const TempAction& op,
                    const TempEffect& effect,
                    const TempAtom& add_atom,
                    TempAtom phi_prime,
                    size_t num_counted_variables)
{
    if (covers(inv, phi_prime))
        return;

    auto atoms = inv.atoms;
    atoms.push_back(phi_prime);

    auto refined = Invariant(inv.num_rigid_variables, num_counted_variables, std::move(atoms));
    normalize_invariant(refined);

    if (!is_add_effect_unbalanced(op, effect, add_atom, refined))
        result.push_back(std::move(refined));
}

bool atom_uses_counted(const TempAtom& atom, size_t num_rigid_variables)
{
    return std::ranges::any_of(atom.terms,
                               [&](const auto& term)
                               {
                                   return std::visit(
                                       [&](auto&& arg) -> bool
                                       {
                                           using T = std::decay_t<decltype(arg)>;
                                           if constexpr (std::is_same_v<T, ParameterIndex>)
                                               return static_cast<uint_t>(arg) >= num_rigid_variables;
                                           return false;
                                       },
                                       term.value);
                               });
}

}  // namespace

InvariantList refine_candidate(const Invariant& inv, const Threat& threat, const TempActionList& ops, PredicateListView<FluentTag>)
{
    InvariantList result;

    const auto& op = ops[threat.op_index];
    const auto& effect = op.effects[threat.effect_index];
    const auto& add_atom = effect.add_effects[threat.add_index];

    const TempAtom* part = find_matching_part(inv, add_atom);
    if (part == nullptr)
        return result;

    for (const auto& del_eff : op.effects)
    {
        for (const auto& del_atom : del_eff.del_effects)
        {
            if (uses_predicate(inv, del_atom.predicate))
                continue;

            for (auto phi_prime : possible_matches(*part, add_atom, del_atom, inv.num_rigid_variables))
            {
                const size_t new_num_counted_variables =
                    std::max(inv.num_counted_variables, atom_uses_counted(phi_prime, inv.num_rigid_variables) ? size_t(1) : size_t(0));

                try_refinement(result, inv, op, effect, add_atom, std::move(phi_prime), new_num_counted_variables);
            }
        }
    }

    return result;
}

InvariantList make_initial_candidates(PredicateListView<FluentTag> predicates)
{
    InvariantList result;

    for (const auto predicate : predicates)
    {
        const auto arity = static_cast<size_t>(predicate.get_arity());

        result.emplace_back(arity, 0, TempAtomList { make_initial_atom(predicate, arity) });

        for (size_t counted_position = 0; counted_position < arity; ++counted_position)
            result.emplace_back(arity - 1, 1, TempAtomList { make_initial_atom(predicate, counted_position) });
    }

    return result;
}

}  // namespace tyr::formalism::planning::invariant