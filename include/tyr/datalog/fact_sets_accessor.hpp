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

#ifndef TYR_DATALOG_FACT_SETS_ACCESSOR_HPP_
#define TYR_DATALOG_FACT_SETS_ACCESSOR_HPP_

#include "tyr/common/closed_interval.hpp"
#include "tyr/datalog/fact_sets.hpp"
#include "tyr/datalog/fact_sets_accessor_concept.hpp"
#include "tyr/datalog/workspaces/facts.hpp"
#include "tyr/formalism/datalog/declarations.hpp"
#include "tyr/formalism/datalog/grounder_decl.hpp"
#include "tyr/formalism/datalog/repository.hpp"

namespace tyr::datalog
{

/**
 * Fact-set subpolicies
 */

template<formalism::FactKind T>
struct NoCarePredicateFactSetsAccessor
{
    const PredicateFactSets<T>& fact_sets;

    bool check(formalism::datalog::LiteralView<T> element, const formalism::datalog::GrounderContext& context) const;

    bool check(formalism::datalog::GroundLiteralView<T> element) const;
};

template<formalism::FactKind T>
struct NoCareFunctionFactSetsAccessor
{
    const FunctionFactSets<T>& fact_sets;

    float_t check(formalism::datalog::FunctionTermView<T> element, const formalism::datalog::GrounderContext& context) const;

    float_t check(formalism::datalog::GroundFunctionTermView<T> element) const;
};

template<formalism::FactKind T>
struct CarePredicateFactSetsAccessor
{
    const PredicateFactSets<T>& fact_sets;

    bool check(formalism::datalog::LiteralView<T> element, const formalism::datalog::GrounderContext& context) const
    {
        return NoCarePredicateFactSetsAccessor<T> { fact_sets }.check(element, context);
    }

    bool check(formalism::datalog::GroundLiteralView<T> element) const { return NoCarePredicateFactSetsAccessor<T> { fact_sets }.check(element); }
};

template<>
struct CarePredicateFactSetsAccessor<formalism::FluentTag>
{
    const PredicateFactSets<formalism::FluentTag>& fact_sets;
    const PredicateFactSets<formalism::FluentTag>& care_fact_sets;

    bool check(formalism::datalog::LiteralView<formalism::FluentTag> element, const formalism::datalog::GrounderContext& context) const;

    bool check(formalism::datalog::GroundLiteralView<formalism::FluentTag> element) const;
};

template<formalism::FactKind T>
using CareFunctionFactSetsAccessor = NoCareFunctionFactSetsAccessor<T>;

/**
 * Tagged wrappers
 */

template<formalism::FactKind T>
struct TaggedNoCareFactSetAccessor
{
    NoCarePredicateFactSetsAccessor<T> predicate;
    NoCareFunctionFactSetsAccessor<T> function;

    explicit TaggedNoCareFactSetAccessor(const TaggedFactSets<T>& fact_sets) : predicate { fact_sets.predicate }, function { fact_sets.function } {}
};

template<formalism::FactKind T>
struct TaggedCareFactSetAccessor;

template<>
struct TaggedCareFactSetAccessor<formalism::StaticTag>
{
    NoCarePredicateFactSetsAccessor<formalism::StaticTag> predicate;
    NoCareFunctionFactSetsAccessor<formalism::StaticTag> function;

    explicit TaggedCareFactSetAccessor(const TaggedFactSets<formalism::StaticTag>& fact_sets) :
        predicate { fact_sets.predicate },
        function { fact_sets.function }
    {
    }
};

template<>
struct TaggedCareFactSetAccessor<formalism::FluentTag>
{
    CarePredicateFactSetsAccessor<formalism::FluentTag> predicate;
    NoCareFunctionFactSetsAccessor<formalism::FluentTag> function;

    TaggedCareFactSetAccessor(const TaggedFactSets<formalism::FluentTag>& fact_sets, const PredicateFactSets<formalism::FluentTag>& care_fact_sets) :
        predicate { fact_sets.predicate, care_fact_sets },
        function { fact_sets.function }
    {
    }
};

/**
 * Combined wrappers
 */

struct NoCareFactSetAccessor
{
    TaggedNoCareFactSetAccessor<formalism::StaticTag> static_policy;
    TaggedNoCareFactSetAccessor<formalism::FluentTag> fluent_policy;

    NoCareFactSetAccessor(const ConstFactsWorkspace& cws, const FactsWorkspace& ws) : static_policy { cws.fact_sets }, fluent_policy { ws.fact_sets } {}

    template<formalism::FactKind T>
    const auto& get() const noexcept
    {
        if constexpr (std::is_same_v<T, formalism::StaticTag>)
            return static_policy;
        else
            return fluent_policy;
    }
};

struct CareFactSetAccessor
{
    TaggedCareFactSetAccessor<formalism::StaticTag> static_policy;
    TaggedCareFactSetAccessor<formalism::FluentTag> fluent_policy;

    CareFactSetAccessor(const ConstFactsWorkspace& cws, const FactsWorkspace& ws) :
        static_policy { cws.fact_sets },
        fluent_policy { ws.fact_sets, ws.care_fact_sets }
    {
    }

    template<formalism::FactKind T>
    const auto& get() const noexcept
    {
        if constexpr (std::is_same_v<T, formalism::StaticTag>)
            return static_policy;
        else
            return fluent_policy;
    }
};

/**
 * Implementations
 */

template<typename Relation, typename Terms>
auto find_relation_binding(Index<Relation> relation, Terms terms, const formalism::datalog::GrounderContext& context)
{
    auto binding_ptr = context.builder.get_builder<formalism::RelationBinding<Relation>>();
    auto& binding = *binding_ptr;
    binding.clear();

    binding.relation = relation;
    for (const auto term : terms)
    {
        visit(
            [&](auto&& arg)
            {
                using Alternative = std::decay_t<decltype(arg)>;

                if constexpr (std::is_same_v<Alternative, formalism::ParameterIndex>)
                    binding.objects.push_back(context.binding[uint_t(arg)]);
                else if constexpr (std::is_same_v<Alternative, formalism::datalog::ObjectView>)
                    binding.objects.push_back(arg.get_index());
                else
                    static_assert(dependent_false<Alternative>::value, "Missing case");
            },
            term.get_variant());
    }

    canonicalize(binding);
    return context.destination.find(binding);
}

/**
 * NoCarePredicateFactSetsAccessor
 */

template<formalism::FactKind T>
bool NoCarePredicateFactSetsAccessor<T>::check(formalism::datalog::LiteralView<T> element, const formalism::datalog::GrounderContext& context) const
{
    const auto binding_or_nullopt = find_relation_binding(element.get_atom().get_predicate().get_index(), element.get_atom().get_terms(), context);

    if (!binding_or_nullopt)
        return !element.get_polarity();

    return fact_sets.contains(*binding_or_nullopt) == element.get_polarity();
}

template<formalism::FactKind T>
bool NoCarePredicateFactSetsAccessor<T>::check(formalism::datalog::GroundLiteralView<T> element) const
{
    return fact_sets.contains(element.get_atom().get_row()) == element.get_polarity();
}

/**
 * NoCareFunctionFactSetsAccessor
 */

template<formalism::FactKind T>
float_t NoCareFunctionFactSetsAccessor<T>::check(formalism::datalog::FunctionTermView<T> element, const formalism::datalog::GrounderContext& context) const
{
    const auto binding_or_nullopt = find_relation_binding(element.get_function().get_index(), element.get_terms(), context);

    if (!binding_or_nullopt)
        return std::numeric_limits<float_t>::quiet_NaN();

    return fact_sets[*binding_or_nullopt];
}

template<formalism::FactKind T>
float_t NoCareFunctionFactSetsAccessor<T>::check(formalism::datalog::GroundFunctionTermView<T> element) const
{
    return fact_sets[element.get_row()];
}

/**
 * CarePredicateFactSetsAccessor<FluentTag>
 */

inline bool CarePredicateFactSetsAccessor<formalism::FluentTag>::check(formalism::datalog::LiteralView<formalism::FluentTag> element,
                                                                       const formalism::datalog::GrounderContext& context) const
{
    const auto binding_or_nullopt = find_relation_binding(element.get_atom().get_predicate().get_index(), element.get_atom().get_terms(), context);

    // Outside represented universe => outside care => don't care.
    if (!binding_or_nullopt)
        return true;

    if (!care_fact_sets.contains(*binding_or_nullopt))
        return true;

    return fact_sets.contains(*binding_or_nullopt) == element.get_polarity();
}

inline bool CarePredicateFactSetsAccessor<formalism::FluentTag>::check(formalism::datalog::GroundLiteralView<formalism::FluentTag> element) const
{
    const auto binding = element.get_atom().get_row();

    if (!care_fact_sets.contains(binding))
        return true;

    return fact_sets.contains(binding) == element.get_polarity();
}

}

#endif
