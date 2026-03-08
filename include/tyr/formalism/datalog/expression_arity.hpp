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

#ifndef TYR_FORMALISM_DATALOG_EXPRESSION_ARITY_HPP_
#define TYR_FORMALISM_DATALOG_EXPRESSION_ARITY_HPP_

#include "tyr/common/equal_to.hpp"
#include "tyr/common/hash.hpp"
#include "tyr/formalism/datalog/declarations.hpp"
#include "tyr/formalism/datalog/repository.hpp"
#include "tyr/formalism/datalog/views.hpp"

#include <numeric>

namespace tyr::formalism::datalog
{

/**
 * collect_parameters
 */

void collect_parameters(float_t element, UnorderedSet<ParameterIndex>& result);

void collect_parameters(View<Data<Term>, Repository> element, UnorderedSet<ParameterIndex>& result);

template<FactKind T>
void collect_parameters(View<Index<Atom<T>>, Repository> element, UnorderedSet<ParameterIndex>& result);

template<FactKind T>
void collect_parameters(View<Index<Literal<T>>, Repository> element, UnorderedSet<ParameterIndex>& result);

template<FactKind T>
void collect_parameters(View<Index<FunctionTerm<T>>, Repository> element, UnorderedSet<ParameterIndex>& result);

void collect_parameters(View<Data<FunctionExpression>, Repository> element, UnorderedSet<ParameterIndex>& result);

template<ArithmeticOpKind O>
void collect_parameters(View<Index<UnaryOperator<O, Data<FunctionExpression>>>, Repository> element, UnorderedSet<ParameterIndex>& result);

template<OpKind O>
void collect_parameters(View<Index<BinaryOperator<O, Data<FunctionExpression>>>, Repository> element, UnorderedSet<ParameterIndex>& result);

template<ArithmeticOpKind O>
void collect_parameters(View<Index<MultiOperator<O, Data<FunctionExpression>>>, Repository> element, UnorderedSet<ParameterIndex>& result);

void collect_parameters(View<Data<ArithmeticOperator<Data<FunctionExpression>>>, Repository> element, UnorderedSet<ParameterIndex>& result);

void collect_parameters(View<Data<BooleanOperator<Data<FunctionExpression>>>, Repository> element, UnorderedSet<ParameterIndex>& result);

auto collect_parameters(View<Data<BooleanOperator<Data<FunctionExpression>>>, Repository> element);

/**
 * Implementations
 */

inline void collect_parameters(float_t element, UnorderedSet<ParameterIndex>& result) {}

inline void collect_parameters(View<Data<Term>, Repository> element, UnorderedSet<ParameterIndex>& result)
{
    visit(
        [&](auto&& arg)
        {
            using Alternative = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<Alternative, ParameterIndex>)
                result.insert(arg);
            else if constexpr (std::is_same_v<Alternative, View<Index<Object>, Repository>>) {}
            else
                static_assert(dependent_false<Alternative>::value, "Missing case");
        },
        element.get_variant());
}

template<FactKind T>
inline void collect_parameters(View<Index<Atom<T>>, Repository> element, UnorderedSet<ParameterIndex>& result)
{
    for (const auto term : element.get_terms())
        collect_parameters(term, result);
}

template<FactKind T>
inline void collect_parameters(View<Index<Literal<T>>, Repository> element, UnorderedSet<ParameterIndex>& result)
{
    collect_parameters(element.get_atom(), result);
}

template<FactKind T>
inline void collect_parameters(View<Index<FunctionTerm<T>>, Repository> element, UnorderedSet<ParameterIndex>& result)
{
    for (const auto term : element.get_terms())
        collect_parameters(term, result);
}

inline void collect_parameters(View<Data<FunctionExpression>, Repository> element, UnorderedSet<ParameterIndex>& result)
{
    visit([&](auto&& arg) { collect_parameters(arg, result); }, element.get_variant());
}

template<ArithmeticOpKind O>
inline void collect_parameters(View<Index<UnaryOperator<O, Data<FunctionExpression>>>, Repository> element, UnorderedSet<ParameterIndex>& result)
{
    collect_parameters(element.get_arg(), result);
}

template<OpKind O>
inline void collect_parameters(View<Index<BinaryOperator<O, Data<FunctionExpression>>>, Repository> element, UnorderedSet<ParameterIndex>& result)
{
    collect_parameters(element.get_lhs(), result);
    collect_parameters(element.get_rhs(), result);
}

template<ArithmeticOpKind O>
inline void collect_parameters(View<Index<MultiOperator<O, Data<FunctionExpression>>>, Repository> element, UnorderedSet<ParameterIndex>& result)
{
    for (const auto arg : element.get_args())
        collect_parameters(arg, result);
}

inline void collect_parameters(View<Data<ArithmeticOperator<Data<FunctionExpression>>>, Repository> element, UnorderedSet<ParameterIndex>& result)
{
    visit([&](auto&& arg) { collect_parameters(arg, result); }, element.get_variant());
}

inline void collect_parameters(View<Data<BooleanOperator<Data<FunctionExpression>>>, Repository> element, UnorderedSet<ParameterIndex>& result)
{
    visit([&](auto&& arg) { collect_parameters(arg, result); }, element.get_variant());
}

template<FactKind T>
inline auto collect_parameters(View<Index<Atom<T>>, Repository> element)
{
    auto result = UnorderedSet<ParameterIndex> {};
    collect_parameters(element, result);
    return result;
}

template<FactKind T>
inline auto collect_parameters(View<Index<Literal<T>>, Repository> element)
{
    auto result = UnorderedSet<ParameterIndex> {};
    collect_parameters(element, result);
    return result;
}

template<FactKind T>
inline auto collect_parameters(View<Index<FunctionTerm<T>>, Repository> element)
{
    auto result = UnorderedSet<ParameterIndex> {};
    collect_parameters(element, result);
    return result;
}

inline auto collect_parameters(View<Data<BooleanOperator<Data<FunctionExpression>>>, Repository> element)
{
    auto result = UnorderedSet<ParameterIndex> {};
    visit([&](auto&& arg) { collect_parameters(arg, result); }, element.get_variant());
    return result;
}

/**
 * max_fterm_arity
 */

inline size_t max_fterm_arity(float_t element);

template<FactKind T>
size_t max_fterm_arity(View<Index<FunctionTerm<T>>, Repository> element);

size_t max_fterm_arity(View<Data<FunctionExpression>, Repository> element);

template<ArithmeticOpKind O>
size_t max_fterm_arity(View<Index<UnaryOperator<O, Data<FunctionExpression>>>, Repository> element);

template<OpKind O>
size_t max_fterm_arity(View<Index<BinaryOperator<O, Data<FunctionExpression>>>, Repository> element);

template<ArithmeticOpKind O>
size_t max_fterm_arity(View<Index<MultiOperator<O, Data<FunctionExpression>>>, Repository> element);

size_t max_fterm_arity(View<Data<ArithmeticOperator<Data<FunctionExpression>>>, Repository> element);

size_t max_fterm_arity(View<Data<BooleanOperator<Data<FunctionExpression>>>, Repository> element);

/**
 * Implementations
 */

inline size_t max_fterm_arity(float_t element) { return 0; }

template<FactKind T>
inline size_t max_fterm_arity(View<Index<FunctionTerm<T>>, Repository> element)
{
    return max_fterm_arity(element.get_function().get_arity());
}

inline size_t max_fterm_arity(View<Data<FunctionExpression>, Repository> element)
{
    return visit([&](auto&& arg) { return max_fterm_arity(arg); }, element.get_variant());
}

template<ArithmeticOpKind O>
inline size_t max_fterm_arity(View<Index<UnaryOperator<O, Data<FunctionExpression>>>, Repository> element)
{
    return max_fterm_arity(element.get_arg());
}

template<OpKind O>
inline size_t max_fterm_arity(View<Index<BinaryOperator<O, Data<FunctionExpression>>>, Repository> element)
{
    return std::max(max_fterm_arity(element.get_lhs()), max_fterm_arity(element.get_rhs()));
}

template<ArithmeticOpKind O>
inline size_t max_fterm_arity(View<Index<MultiOperator<O, Data<FunctionExpression>>>, Repository> element)
{
    const auto child_fexprs = element.get_args();

    return std::accumulate(std::next(child_fexprs.begin()),  // Start from the second expression
                           child_fexprs.end(),
                           max_fterm_arity(child_fexprs.front()),
                           [&](const auto& value, const auto& child_expr) { return std::max(value, max_fterm_arity(child_expr)); });
}

inline size_t max_fterm_arity(View<Data<ArithmeticOperator<Data<FunctionExpression>>>, Repository> element)
{
    return visit([&](auto&& arg) { return max_fterm_arity(arg); }, element.get_variant());
}

inline size_t max_fterm_arity(View<Data<BooleanOperator<Data<FunctionExpression>>>, Repository> element)
{
    return visit([&](auto&& arg) { return max_fterm_arity(arg); }, element.get_variant());
}

/**
 * kpkc_arity
 */

template<FactKind T>
inline size_t kpkc_arity(View<Index<Literal<T>>, Repository> element)
{
    return element.get_atom().get_predicate().get_arity();
}

template<FactKind T>
inline size_t kpkc_arity(View<Index<FunctionTerm<T>>, Repository> element)
{
    return element.get_function().get_arity();
}

inline size_t kpkc_arity(View<Data<BooleanOperator<Data<FunctionExpression>>>, Repository> element)
{
    return std::max(max_fterm_arity(element), collect_parameters(element).size());
}

/**
 * parameter_arity
 */

template<FactKind T>
inline size_t parameter_arity(View<Index<Literal<T>>, Repository> element)
{
    return collect_parameters(element).size();
}

template<FactKind T>
inline size_t parameter_arity(View<Index<FunctionTerm<T>>, Repository> element)
{
    return collect_parameters(element).size();
}

inline size_t parameter_arity(View<Data<BooleanOperator<Data<FunctionExpression>>>, Repository> element) { return collect_parameters(element).size(); }

}

#endif