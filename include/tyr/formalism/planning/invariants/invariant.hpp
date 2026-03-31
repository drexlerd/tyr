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

#ifndef TYR_FORMALISM_PLANNING_INVARIANTS_INVARIANT_HPP_
#define TYR_FORMALISM_PLANNING_INVARIANTS_INVARIANT_HPP_

#include "tyr/common/comparators.hpp"
#include "tyr/common/declarations.hpp"
#include "tyr/common/equal_to.hpp"
#include "tyr/common/hash.hpp"
#include "tyr/formalism/planning/expression_arity.hpp"
#include "tyr/formalism/planning/grounder.hpp"
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
struct TempAtom
{
    PredicateView<FluentTag> predicate;
    std::vector<Data<Term>> terms;

    auto identifying_members() const noexcept { return std::tie(predicate, terms); }

    friend bool operator==(const TempAtom& lhs, const TempAtom& rhs) { return lhs.identifying_members() == rhs.identifying_members(); }

    friend bool operator<(const TempAtom& lhs, const TempAtom& rhs) { return lhs.identifying_members() < rhs.identifying_members(); }
};

using TempAtomList = std::vector<TempAtom>;

struct TempLiteral
{
    TempAtom atom;
    bool polarity;

    auto identifying_members() const noexcept { return std::tie(atom, polarity); }

    friend bool operator==(const TempLiteral& lhs, const TempLiteral& rhs) { return lhs.identifying_members() == rhs.identifying_members(); }

    friend bool operator<(const TempLiteral& lhs, const TempLiteral& rhs) { return lhs.identifying_members() < rhs.identifying_members(); }
};

using TempLiteralList = std::vector<TempLiteral>;

struct TempEffect
{
    size_t num_action_variables;
    size_t num_effect_variables;
    TempLiteralList condition;
    TempAtomList add_effects;
    TempAtomList del_effects;
};

using TempEffectList = std::vector<TempEffect>;

struct TempAction
{
    size_t num_variables;
    TempLiteralList precondition;
    TempEffectList effects;
};

using TempActionList = std::vector<TempAction>;

struct Invariant
{
    size_t num_rigid_variables = 0;
    size_t num_counted_variables = 0;
    TempAtomList atoms;
    UnorderedSet<PredicateView<FluentTag>> predicates;

    Invariant() = default;

    Invariant(size_t num_rigid_variables_, size_t num_counted_variables_, TempAtomList atoms_) :
        num_rigid_variables(num_rigid_variables_),
        num_counted_variables(num_counted_variables_),
        atoms(std::move(atoms_)),
        predicates()
    {
        canonicalize();
    }

    void canonicalize()
    {
        std::sort(atoms.begin(), atoms.end());
        atoms.erase(std::unique(atoms.begin(), atoms.end()), atoms.end());

        predicates.clear();
        for (const auto& atom : atoms)
            predicates.insert(atom.predicate);
    }

    auto identifying_members() const noexcept { return std::tie(num_rigid_variables, num_counted_variables, atoms); }

    friend bool operator==(const Invariant& lhs, const Invariant& rhs) { return lhs.identifying_members() == rhs.identifying_members(); }

    friend bool operator<(const Invariant& lhs, const Invariant& rhs) { return lhs.identifying_members() < rhs.identifying_members(); }
};

using InvariantList = std::vector<Invariant>;

struct ActionTag
{
};
struct EffectTag
{
};
struct InvariantTag
{
};

template<typename Tag>
struct Substitution
{
    std::vector<std::optional<Data<Term>>> data;

    Substitution() = default;
    explicit Substitution(size_t size) : data(size) {}

    void reset() noexcept { std::fill(data.begin(), data.end(), std::nullopt); }

    void resize(size_t size)
    {
        data.resize(size);
        reset();
    }

    size_t size() const noexcept { return data.size(); }

    bool contains(ParameterIndex parameter) const noexcept
    {
        return static_cast<uint_t>(parameter) < data.size() && data[static_cast<uint_t>(parameter)].has_value();
    }

    const std::optional<Data<Term>>& get(ParameterIndex parameter) const noexcept { return data[static_cast<uint_t>(parameter)]; }

    std::optional<Data<Term>>& get(ParameterIndex parameter) noexcept { return data[static_cast<uint_t>(parameter)]; }

    bool assign(ParameterIndex parameter, const Data<Term>& term)
    {
        auto& slot = data[static_cast<uint_t>(parameter)];
        if (slot.has_value())
            return false;
        slot = term;
        return true;
    }

    bool assign_or_check(ParameterIndex parameter, const Data<Term>& term)
    {
        auto& slot = data[static_cast<uint_t>(parameter)];
        if (!slot.has_value())
        {
            slot = term;
            return true;
        }
        return *slot == term;
    }
};

using ActionSubstitution = Substitution<ActionTag>;
using EffectSubstitution = Substitution<EffectTag>;
using InvariantSubstitution = Substitution<InvariantTag>;

template<typename Tag>
inline Data<Term> apply_substitution(const Data<Term>& term, const Substitution<Tag>& sigma)
{
    return std::visit(
        [&](auto&& arg) -> Data<Term>
        {
            using T = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<T, ParameterIndex>)
            {
                if (sigma.contains(arg))
                    return *sigma.get(arg);
                return term;
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

inline Data<Term> apply_substitution(const Data<Term>& term, const EffectSubstitution& sigma, size_t num_action_variables)
{
    return std::visit(
        [&](auto&& arg) -> Data<Term>
        {
            using T = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<T, ParameterIndex>)
            {
                const auto index = static_cast<uint_t>(arg);

                if (index < num_action_variables)
                    return term;

                const auto local_index = ParameterIndex(index - num_action_variables);
                if (sigma.contains(local_index))
                    return *sigma.get(local_index);

                return term;
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

template<typename Tag>
inline TempAtom apply_substitution(const TempAtom& atom, const Substitution<Tag>& sigma)
{
    auto terms = std::vector<Data<Term>> {};
    terms.reserve(atom.terms.size());

    for (const auto& term : atom.terms)
        terms.push_back(apply_substitution(term, sigma));

    return TempAtom {
        .predicate = atom.predicate,
        .terms = std::move(terms),
    };
}

inline TempAtom apply_substitution(const TempAtom& atom, const EffectSubstitution& sigma, size_t num_action_variables)
{
    auto terms = std::vector<Data<Term>> {};
    terms.reserve(atom.terms.size());

    for (const auto& term : atom.terms)
        terms.push_back(apply_substitution(term, sigma, num_action_variables));

    return TempAtom {
        .predicate = atom.predicate,
        .terms = std::move(terms),
    };
}

template<typename Tag>
inline TempLiteral apply_substitution(const TempLiteral& lit, const Substitution<Tag>& sigma)
{
    return TempLiteral {
        .atom = apply_substitution(lit.atom, sigma),
        .polarity = lit.polarity,
    };
}

inline TempLiteral apply_substitution(const TempLiteral& lit, const EffectSubstitution& sigma, size_t num_action_variables)
{
    return TempLiteral {
        .atom = apply_substitution(lit.atom, sigma, num_action_variables),
        .polarity = lit.polarity,
    };
}

template<typename Tag>
inline TempLiteralList apply_substitution(const TempLiteralList& lits, const Substitution<Tag>& sigma)
{
    auto result = TempLiteralList {};
    result.reserve(lits.size());

    for (const auto& lit : lits)
        result.push_back(apply_substitution(lit, sigma));

    return result;
}

inline TempLiteralList apply_substitution(const TempLiteralList& lits, const EffectSubstitution& sigma, size_t num_action_variables)
{
    auto result = TempLiteralList {};
    result.reserve(lits.size());

    for (const auto& lit : lits)
        result.push_back(apply_substitution(lit, sigma, num_action_variables));

    return result;
}

inline Data<Term> make_temp_term(TermView element) { return element.get_data(); }

inline TempAtom make_temp_atom(AtomView<FluentTag> element)
{
    auto terms = std::vector<Data<Term>> {};
    terms.reserve(element.get_terms().size());

    for (const auto term : element.get_terms())
        terms.push_back(make_temp_term(term));

    return TempAtom {
        .predicate = element.get_predicate(),
        .terms = std::move(terms),
    };
}

inline TempLiteral make_temp_literal(LiteralView<FluentTag> element)
{
    return TempLiteral {
        .atom = make_temp_atom(element.get_atom()),
        .polarity = element.get_polarity(),
    };
}

inline Data<Term> make_temp_effect_term(TermView term, size_t /*num_action_variables*/) { return term.get_data(); }

inline TempAtom make_temp_effect_atom(AtomView<FluentTag> element, size_t num_action_variables)
{
    auto terms = std::vector<Data<Term>> {};
    terms.reserve(element.get_terms().size());

    for (const auto term : element.get_terms())
        terms.push_back(make_temp_effect_term(term, num_action_variables));

    return TempAtom {
        .predicate = element.get_predicate(),
        .terms = std::move(terms),
    };
}

inline TempLiteral make_temp_effect_literal(LiteralView<FluentTag> element, size_t num_action_variables)
{
    return TempLiteral {
        .atom = make_temp_effect_atom(element.get_atom(), num_action_variables),
        .polarity = element.get_polarity(),
    };
}

inline TempEffect make_temp_effect(ConditionalEffectView element, size_t action_arity)
{
    TempEffect result {};

    result.num_action_variables = action_arity;
    result.num_effect_variables = element.get_arity();

    for (const auto lit : element.get_condition().get_literals<FluentTag>())
        result.condition.push_back(make_temp_effect_literal(lit, action_arity));

    for (const auto lit : element.get_effect().get_literals())
    {
        if (lit.get_polarity())
            result.add_effects.push_back(make_temp_effect_atom(lit.get_atom(), action_arity));
        else
            result.del_effects.push_back(make_temp_effect_atom(lit.get_atom(), action_arity));
    }

    return result;
}

inline TempAction make_temp_action(ActionView op)
{
    TempAction result {};

    result.num_variables = op.get_arity();

    for (const auto lit : op.get_condition().get_literals<FluentTag>())
        result.precondition.push_back(make_temp_literal(lit));

    result.effects.reserve(op.get_effects().size());
    for (const auto cond_eff : op.get_effects())
        result.effects.push_back(make_temp_effect(cond_eff, op.get_arity()));

    return result;
}

inline TempActionList make_temp_actions(ActionListView ops)
{
    auto result = TempActionList {};
    result.reserve(ops.size());

    for (const auto op : ops)
        result.push_back(make_temp_action(op));

    return result;
}

}

#endif
