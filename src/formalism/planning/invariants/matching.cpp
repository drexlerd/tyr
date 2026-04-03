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

#include "matching.hpp"

namespace tyr::formalism::planning::invariant
{
namespace
{

bool is_effect_local_parameter(ParameterIndex parameter, size_t num_action_variables) { return static_cast<uint_t>(parameter) >= num_action_variables; }

ParameterIndex to_effect_local_parameter(ParameterIndex parameter, size_t num_action_variables)
{
    return ParameterIndex(static_cast<uint_t>(parameter) - num_action_variables);
}

template<typename Substitution, typename Matcher>
std::optional<Substitution> match_atom(const TempAtom& pattern, const TempAtom& element, Substitution substitution, Matcher&& matcher)
{
    if (pattern.predicate != element.predicate)
        return std::nullopt;

    if (pattern.terms.size() != element.terms.size())
        return std::nullopt;

    for (uint_t i = 0; i < pattern.terms.size(); ++i)
    {
        if (!matcher(pattern.terms[i], element.terms[i], substitution))
            return std::nullopt;
    }

    return substitution;
}

bool match_cover_term(const Invariant& inv, const Data<Term>& lhs, const Data<Term>& rhs, InvariantSubstitution& substitution)
{
    return std::visit(
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
}

bool match_ground_term(const Data<Term>& lhs, const Data<Term>& rhs, InvariantSubstitution& substitution)
{
    return std::visit(
        [&](auto&& lhs_arg) -> bool
        {
            using Lhs = std::decay_t<decltype(lhs_arg)>;

            return std::visit(
                [&](auto&& rhs_arg) -> bool
                {
                    using Rhs = std::decay_t<decltype(rhs_arg)>;

                    if constexpr (std::is_same_v<Lhs, ParameterIndex>)
                    {
                        return substitution.assign_or_check(lhs_arg, rhs);
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
}

bool match_action_alignment_term(const Invariant& inv,
                                 const Data<Term>& lhs,
                                 const Data<Term>& rhs,
                                 size_t num_action_variables,
                                 ActionSubstitution& substitution)
{
    return std::visit(
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
                            return true;

                        if constexpr (std::is_same_v<Rhs, ParameterIndex>)
                        {
                            if (static_cast<uint_t>(rhs_arg) >= num_action_variables)
                                return false;

                            return substitution.assign_or_check(rhs_arg, Data<Term>(lhs_arg));
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

                            return substitution.assign_or_check(rhs_arg, Data<Term>(lhs_arg));
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
}

bool match_effect_cover_term(const Invariant& inv,
                             const Data<Term>& lhs,
                             const Data<Term>& rhs,
                             size_t num_action_variables,
                             std::vector<std::optional<Data<Term>>>& counted_bindings,
                             EffectSubstitution& sigma)
{
    return std::visit(
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
                            if constexpr (std::is_same_v<Rhs, ParameterIndex>)
                            {
                                if (is_effect_local_parameter(rhs_arg, num_action_variables))
                                {
                                    const auto local = to_effect_local_parameter(rhs_arg, num_action_variables);
                                    return sigma.assign_or_check(local, Data<Term>(lhs_arg));
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
                            const auto counted_index = lhs_index - inv.num_rigid_variables;
                            auto& binding = counted_bindings[counted_index];

                            if constexpr (std::is_same_v<Rhs, ParameterIndex>)
                            {
                                if (is_effect_local_parameter(rhs_arg, num_action_variables))
                                {
                                    const auto local = to_effect_local_parameter(rhs_arg, num_action_variables);

                                    if (binding.has_value())
                                        return sigma.assign_or_check(local, *binding);

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
                                return sigma.assign_or_check(local, Data<Term>(lhs_arg));
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
}

std::optional<InvariantSubstitution> match_cover_against_atom(const Invariant& inv, const TempAtom& pattern, const TempAtom& element)
{
    return match_atom(pattern,
                      element,
                      InvariantSubstitution(inv.num_rigid_variables + inv.num_counted_variables),
                      [&](const Data<Term>& lhs, const Data<Term>& rhs, InvariantSubstitution& substitution) -> bool
                      { return match_cover_term(inv, lhs, rhs, substitution); });
}

std::optional<EffectSubstitution>
match_effect_cover_against_atom(const Invariant& inv, const TempAtom& pattern, const TempAtom& element, size_t num_action_variables)
{
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

    EffectSubstitution sigma_eff;
    if (has_local)
        sigma_eff.resize(max_local_index + 1);

    auto counted_bindings = std::vector<std::optional<Data<Term>>>(inv.num_counted_variables, std::nullopt);

    return match_atom(pattern,
                      element,
                      std::move(sigma_eff),
                      [&](const Data<Term>& lhs, const Data<Term>& rhs, EffectSubstitution& sigma) -> bool
                      { return match_effect_cover_term(inv, lhs, rhs, num_action_variables, counted_bindings, sigma); });
}

}  // namespace

bool covers(const Invariant& inv, const TempAtom& element)
{
    for (const auto& atom : inv.atoms)
    {
        if (match_cover_against_atom(inv, atom, element).has_value())
            return true;
    }

    return false;
}

std::optional<InvariantSubstitution> match_invariant_against_ground_atom(const Invariant& inv, const TempAtom& pattern, const TempAtom& ground_atom)
{
    return match_atom(pattern,
                      ground_atom,
                      InvariantSubstitution(inv.num_rigid_variables + inv.num_counted_variables),
                      [&](const Data<Term>& lhs, const Data<Term>& rhs, InvariantSubstitution& sigma) -> bool { return match_ground_term(lhs, rhs, sigma); });
}

std::vector<ActionAlignment> enumerate_action_alignments(const Invariant& inv, const TempAtom& element, size_t num_action_variables)
{
    auto result = std::vector<ActionAlignment> {};

    for (const auto& pattern : inv.atoms)
    {
        auto sigma = match_atom(pattern,
                                element,
                                ActionSubstitution(num_action_variables),
                                [&](const Data<Term>& lhs, const Data<Term>& rhs, ActionSubstitution& substitution) -> bool
                                { return match_action_alignment_term(inv, lhs, rhs, num_action_variables, substitution); });

        if (sigma.has_value())
            result.push_back(ActionAlignment { .pattern = pattern, .sigma = std::move(*sigma) });
    }

    return result;
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

}  // namespace tyr::formalism::planning::invariant