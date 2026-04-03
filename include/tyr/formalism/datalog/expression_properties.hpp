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

#ifndef TYR_FORMALISM_DATALOG_EXPRESSION_PROPERTIES_HPP_
#define TYR_FORMALISM_DATALOG_EXPRESSION_PROPERTIES_HPP_

#include "tyr/common/equal_to.hpp"
#include "tyr/common/hash.hpp"
#include "tyr/formalism/datalog/declarations.hpp"
#include "tyr/formalism/datalog/repository.hpp"
#include "tyr/formalism/datalog/views.hpp"

namespace tyr::formalism::datalog
{
/**
 * Forward declarations
 */

template<FactKind T>
void collect_fterms(float_t element, UnorderedSet<FunctionTermView<T>>& result);

template<FactKind T1, FactKind T2>
void collect_fterms(FunctionTermView<T1> element, UnorderedSet<FunctionTermView<T2>>& result);

template<FactKind T>
void collect_fterms(FunctionExpressionView element, UnorderedSet<FunctionTermView<T>>& result);

template<FactKind T, ArithmeticOpKind O>
void collect_fterms(LiftedUnaryOperatorView<O> element, UnorderedSet<FunctionTermView<T>>& result);

template<FactKind T, OpKind O>
void collect_fterms(LiftedBinaryOperatorView<O> element, UnorderedSet<FunctionTermView<T>>& result);

template<FactKind T, ArithmeticOpKind O>
void collect_fterms(LiftedMultiOperatorView<O> element, UnorderedSet<FunctionTermView<T>>& result);

template<FactKind T>
void collect_fterms(LiftedArithmeticOperatorView element, UnorderedSet<FunctionTermView<T>>& result);

template<FactKind T>
void collect_fterms(LiftedBooleanOperatorView element, UnorderedSet<FunctionTermView<T>>& result);

/**
 * Implementations
 */

template<FactKind T>
inline void collect_fterms(float_t element, UnorderedSet<FunctionTermView<T>>& result)
{
}

template<FactKind T1, FactKind T2>
inline void collect_fterms(FunctionTermView<T1> element, UnorderedSet<FunctionTermView<T2>>& result)
{
    if constexpr (std::same_as<T1, T2>)
        result.insert(element);
}

template<FactKind T>
inline void collect_fterms(FunctionExpressionView element, UnorderedSet<FunctionTermView<T>>& result)
{
    visit([&](auto&& arg) { collect_fterms(arg, result); }, element.get_variant());
}

template<FactKind T, ArithmeticOpKind O>
inline void collect_fterms(LiftedUnaryOperatorView<O> element, UnorderedSet<FunctionTermView<T>>& result)
{
    collect_fterms(element.get_arg(), result);
}

template<FactKind T, OpKind O>
inline void collect_fterms(LiftedBinaryOperatorView<O> element, UnorderedSet<FunctionTermView<T>>& result)
{
    collect_fterms(element.get_lhs(), result);
    collect_fterms(element.get_rhs(), result);
}

template<FactKind T, ArithmeticOpKind O>
inline void collect_fterms(LiftedMultiOperatorView<O> element, UnorderedSet<FunctionTermView<T>>& result)
{
    for (const auto& arg : element.get_args())
        collect_fterms(arg, result);
}

template<FactKind T>
inline void collect_fterms(LiftedArithmeticOperatorView element, UnorderedSet<FunctionTermView<T>>& result)
{
    visit([&](auto&& arg) { collect_fterms(arg, result); }, element.get_variant());
}

template<FactKind T>
inline void collect_fterms(LiftedBooleanOperatorView element, UnorderedSet<FunctionTermView<T>>& result)
{
    visit([&](auto&& arg) { collect_fterms(arg, result); }, element.get_variant());
}

template<FactKind T>
inline auto collect_fterms(LiftedBooleanOperatorView element)
{
    auto result = UnorderedSet<FunctionTermView<T>> {};
    visit([&](auto&& arg) { collect_fterms(arg, result); }, element.get_variant());
    auto result_vec = std::vector<FunctionTermView<T>>(result.begin(), result.end());
    std::sort(result_vec.begin(), result_vec.end());
    return result_vec;
}
}

#endif