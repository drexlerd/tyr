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

#ifndef TYR_FORMALISM_DATALOG_VARIABLE_DEPENDENCY_GRAPH_DETAILS_HPP_
#define TYR_FORMALISM_DATALOG_VARIABLE_DEPENDENCY_GRAPH_DETAILS_HPP_

#include "tyr/common/config.hpp"
#include "tyr/formalism/declarations.hpp"

#include <boost/dynamic_bitset.hpp>

namespace tyr::formalism::datalog::details
{

template<FactKind T, PolarityKind P, typename Dep>
decltype(auto) select_literal_dependency(Dep&& dep) noexcept
{
    if constexpr (std::is_same_v<T, StaticTag>)
    {
        if constexpr (std::is_same_v<P, PositiveTag>)
            return (std::forward<Dep>(dep).static_positive_literal);
        else if constexpr (std::is_same_v<P, NegativeTag>)
            return (std::forward<Dep>(dep).static_negative_literal);
        else
            static_assert(dependent_false<P>::value, "Missing case");
    }
    else if constexpr (std::is_same_v<T, FluentTag>)
    {
        if constexpr (std::is_same_v<P, PositiveTag>)
            return (std::forward<Dep>(dep).fluent_positive_literal);
        else if constexpr (std::is_same_v<P, NegativeTag>)
            return (std::forward<Dep>(dep).fluent_negative_literal);
        else
            static_assert(dependent_false<P>::value, "Missing case");
    }
    else
    {
        static_assert(dependent_false<T>::value, "Missing case");
    }
}

template<typename Dep>
decltype(auto) select_numeric_dependency(Dep&& dep) noexcept
{
    return (std::forward<Dep>(dep).numeric_constraint);
}

struct UnaryDependencies
{
    explicit UnaryDependencies(uint_t k_) :
        k(k_),
        static_positive_literal(k_),
        static_negative_literal(k_),
        fluent_positive_literal(k_),
        fluent_negative_literal(k_),
        numeric_constraint(k_)
    {
    }

    /**
     * get_
     */

    template<FactKind T, PolarityKind P>
    const auto& get_literal_dependency() const noexcept
    {
        return details::select_literal_dependency<T, P>(*this);
    }

    const auto& get_numeric_dependency() const noexcept { return details::select_numeric_dependency(*this); }

    /**
     * has_
     */

    template<FactKind T, PolarityKind P>
    bool has_literal_dependency(uint_t p) const noexcept
    {
        assert(p < k);
        return get_literal_dependency<T, P>().test(p);
    }

    bool has_numeric_dependency(uint_t p) const noexcept
    {
        assert(p < k);
        return numeric_constraint.test(p);
    }

    bool has_dependency(uint_t p) const noexcept
    {
        return has_literal_dependency<StaticTag, PositiveTag>(p) || has_literal_dependency<StaticTag, NegativeTag>(p)
               || has_literal_dependency<FluentTag, PositiveTag>(p) || has_literal_dependency<FluentTag, NegativeTag>(p) || has_numeric_dependency(p);
    }

    template<FactKind T>
    bool has_dependency(uint_t p) const noexcept
    {
        return has_literal_dependency<T, PositiveTag>(p) || has_literal_dependency<T, NegativeTag>(p) || has_numeric_dependency(p);
    }

    uint_t k;

    boost::dynamic_bitset<> static_positive_literal;
    boost::dynamic_bitset<> static_negative_literal;
    boost::dynamic_bitset<> fluent_positive_literal;
    boost::dynamic_bitset<> fluent_negative_literal;
    boost::dynamic_bitset<> numeric_constraint;
};

struct BinaryDependencies
{
    explicit BinaryDependencies(uint_t k_) :
        k(k_),
        static_positive_literal(k_ * k_),
        static_negative_literal(k_ * k_),
        fluent_positive_literal(k_ * k_),
        fluent_negative_literal(k_ * k_),
        numeric_constraint(k_ * k_)
    {
    }

    uint_t get_index(uint_t pi, uint_t pj) const noexcept
    {
        assert(pi < k && pj < k);
        return pi * k + pj;
    }

    /**
     * get_
     */

    template<FactKind T, PolarityKind P>
    const auto& get_literal_dependency() const noexcept
    {
        return details::select_literal_dependency<T, P>(*this);
    }

    const auto& get_numeric_dependency() const noexcept { return details::select_numeric_dependency(*this); }

    /**
     * has_
     */

    template<FactKind T, PolarityKind P>
    bool has_literal_dependency(uint_t pi, uint_t pj) const noexcept
    {
        return get_literal_dependency<T, P>().test(get_index(pi, pj));
    }

    bool has_numeric_dependency(uint_t pi, uint_t pj) const noexcept { return numeric_constraint.test(get_index(pi, pj)); }

    bool has_dependency(uint_t pi, uint_t pj) const noexcept
    {
        return has_literal_dependency<StaticTag, PositiveTag>(pi, pj) || has_literal_dependency<StaticTag, NegativeTag>(pi, pj)
               || has_literal_dependency<FluentTag, PositiveTag>(pi, pj) || has_literal_dependency<FluentTag, NegativeTag>(pi, pj)
               || has_numeric_dependency(pi, pj);
    }

    template<FactKind T>
    bool has_dependency(uint_t pi, uint_t pj) const noexcept
    {
        return has_literal_dependency<T, PositiveTag>(pi, pj) || has_literal_dependency<T, NegativeTag>(pi, pj) || has_numeric_dependency(pi, pj);
    }

    uint_t k;

    boost::dynamic_bitset<> static_positive_literal;
    boost::dynamic_bitset<> static_negative_literal;
    boost::dynamic_bitset<> fluent_positive_literal;
    boost::dynamic_bitset<> fluent_negative_literal;
    boost::dynamic_bitset<> numeric_constraint;
};
}

#endif
