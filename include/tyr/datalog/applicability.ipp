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

#include "tyr/common/macros.hpp"
#include "tyr/common/vector.hpp"
#include "tyr/datalog/fact_sets.hpp"
#include "tyr/formalism/arithmetic_operator_utils.hpp"
#include "tyr/formalism/boolean_operator_utils.hpp"
#include "tyr/formalism/datalog/builder.hpp"
#include "tyr/formalism/datalog/declarations.hpp"
#include "tyr/formalism/datalog/grounder.hpp"
#include "tyr/formalism/datalog/repository.hpp"
#include "tyr/formalism/datalog/views.hpp"
#include "tyr/formalism/declarations.hpp"

#include <algorithm>
#include <concepts>
#include <iterator>
#include <limits>
#include <numeric>
#include <stdexcept>
#include <type_traits>
#include <vector>

namespace tyr::datalog
{

/**
 * evaluate
 */

TYR_INLINE_IMPL float_t evaluate(float_t element, const FactSets& fact_sets) { return element; }

template<formalism::ArithmeticOpKind O>
float_t evaluate(formalism::datalog::GroundUnaryOperatorView<O> element, const FactSets& fact_sets)
{
    return formalism::apply(O {}, evaluate(element.get_arg(), fact_sets));
}

template<formalism::ArithmeticOpKind O>
float_t evaluate(formalism::datalog::GroundBinaryOperatorView<O> element, const FactSets& fact_sets)
{
    return formalism::apply(O {}, evaluate(element.get_lhs(), fact_sets), evaluate(element.get_rhs(), fact_sets));
}

template<formalism::BooleanOpKind O>
bool evaluate(formalism::datalog::GroundBinaryOperatorView<O> element, const FactSets& fact_sets)
{
    return formalism::apply(O {}, evaluate(element.get_lhs(), fact_sets), evaluate(element.get_rhs(), fact_sets));
}

template<formalism::ArithmeticOpKind O>
float_t evaluate(formalism::datalog::GroundMultiOperatorView<O> element, const FactSets& fact_sets)
{
    const auto child_fexprs = element.get_args();

    return std::accumulate(std::next(child_fexprs.begin()),  // Start from the second expression
                           child_fexprs.end(),
                           evaluate(child_fexprs.front(), fact_sets),
                           [&](const auto& value, const auto& child_expr) { return formalism::apply(O {}, value, evaluate(child_expr, fact_sets)); });
}

template<formalism::FactKind T>
float_t evaluate(formalism::datalog::GroundFunctionTermView<T> element, const FactSets& fact_sets)
{
    return fact_sets.template get<T>().function[element];
}

TYR_INLINE_IMPL float_t evaluate(formalism::datalog::GroundFunctionExpressionView element, const FactSets& fact_sets)
{
    return visit([&](auto&& arg) { return evaluate(arg, fact_sets); }, element.get_variant());
}

TYR_INLINE_IMPL float_t evaluate(formalism::datalog::GroundArithmeticOperatorView element, const FactSets& fact_sets)
{
    return visit([&](auto&& arg) { return evaluate(arg, fact_sets); }, element.get_variant());
}

TYR_INLINE_IMPL bool evaluate(formalism::datalog::GroundBooleanOperatorView element, const FactSets& fact_sets)
{
    return visit([&](auto&& arg) { return evaluate(arg, fact_sets); }, element.get_variant());
}

/**
 * is_applicable
 */

template<formalism::FactKind T>
bool is_applicable(formalism::datalog::GroundLiteralView<T> element, const FactSets& fact_sets)
{
    return fact_sets.template get<T>().predicate.contains(element.get_atom().get_row()) == element.get_polarity();
}

template<formalism::FactKind T>
bool is_applicable(formalism::datalog::GroundLiteralListView<T> elements, const FactSets& fact_sets)
{
    return std::all_of(elements.begin(), elements.end(), [&](auto&& arg) { return is_applicable(arg, fact_sets); });
}

TYR_INLINE_IMPL bool is_applicable(formalism::datalog::GroundBooleanOperatorListView elements, const FactSets& fact_sets)
{
    return std::all_of(elements.begin(), elements.end(), [&](auto&& arg) { return evaluate(arg, fact_sets); });
}

// GroundConjunctiveCondition

TYR_INLINE_IMPL bool is_applicable(formalism::datalog::GroundConjunctiveConditionView element, const FactSets& fact_sets)
{
    return is_applicable(element.template get_literals<formalism::StaticTag>(), fact_sets)     //
           && is_applicable(element.template get_literals<formalism::FluentTag>(), fact_sets)  //
           && is_applicable(element.get_numeric_constraints(), fact_sets);
}

// GroundRule

TYR_INLINE_IMPL bool is_applicable(formalism::datalog::GroundRuleView element, const FactSets& fact_sets)
{
    return is_applicable(element.get_body(), fact_sets);
}

/**
 * is_valid_binding
 */

TYR_INLINE_IMPL float_t is_valid_binding(float_t element, const FactSets&, formalism::datalog::GrounderContext&) { return element; }

template<formalism::ArithmeticOpKind O>
float_t is_valid_binding(formalism::datalog::LiftedUnaryOperatorView<O> element, const FactSets& fact_sets, formalism::datalog::GrounderContext& context)
{
    return formalism::apply(O {}, is_valid_binding(element.get_arg(), fact_sets, context));
}

template<formalism::ArithmeticOpKind O>
float_t is_valid_binding(formalism::datalog::LiftedBinaryOperatorView<O> element, const FactSets& fact_sets, formalism::datalog::GrounderContext& context)
{
    return formalism::apply(O {}, is_valid_binding(element.get_lhs(), fact_sets, context), is_valid_binding(element.get_rhs(), fact_sets, context));
}

template<formalism::BooleanOpKind O>
bool is_valid_binding(formalism::datalog::LiftedBinaryOperatorView<O> element, const FactSets& fact_sets, formalism::datalog::GrounderContext& context)
{
    return formalism::apply(O {}, is_valid_binding(element.get_lhs(), fact_sets, context), is_valid_binding(element.get_rhs(), fact_sets, context));
}

template<formalism::ArithmeticOpKind O>
float_t is_valid_binding(formalism::datalog::LiftedMultiOperatorView<O> element, const FactSets& fact_sets, formalism::datalog::GrounderContext& context)
{
    const auto child_fexprs = element.get_args();

    return std::accumulate(std::next(child_fexprs.begin()),  // Start from the second expression
                           child_fexprs.end(),
                           is_valid_binding(child_fexprs.front(), fact_sets, context),
                           [&](const auto& value, const auto& child_expr)
                           { return formalism::apply(O {}, value, is_valid_binding(child_expr, fact_sets, context)); });
}

template<formalism::FactKind T>
float_t is_valid_binding(formalism::datalog::FunctionTermView<T> element, const FactSets& fact_sets, formalism::datalog::GrounderContext& context)
{
    auto binding_or_nullopt = try_ground_binding(element, context);
    if (!binding_or_nullopt)
        return std::numeric_limits<float_t>::quiet_NaN();  // Indicate invalid binding with NaN

    return fact_sets.template get<T>().function[*binding_or_nullopt];
}

TYR_INLINE_IMPL float_t is_valid_binding(formalism::datalog::FunctionExpressionView element,
                                         const FactSets& fact_sets,
                                         formalism::datalog::GrounderContext& context)
{
    return visit([&](auto&& arg) { return is_valid_binding(arg, fact_sets, context); }, element.get_variant());
}

TYR_INLINE_IMPL float_t is_valid_binding(formalism::datalog::LiftedArithmeticOperatorView element,
                                         const FactSets& fact_sets,
                                         formalism::datalog::GrounderContext& context)
{
    return visit([&](auto&& arg) { return is_valid_binding(arg, fact_sets, context); }, element.get_variant());
}

TYR_INLINE_IMPL bool
is_valid_binding(formalism::datalog::LiftedBooleanOperatorView element, const FactSets& fact_sets, formalism::datalog::GrounderContext& context)
{
    return visit([&](auto&& arg) { return is_valid_binding(arg, fact_sets, context); }, element.get_variant());
}

template<formalism::FactKind T>
bool is_valid_binding(formalism::datalog::LiteralView<T> element, const FactSets& fact_sets, formalism::datalog::GrounderContext& context)
{
    auto binding_or_nullopt = try_ground_binding(element.get_atom(), context);
    if (!binding_or_nullopt)
        return element.get_polarity() == false;

    return fact_sets.template get<T>().predicate.contains(*binding_or_nullopt) == element.get_polarity();
}

template<formalism::FactKind T>
bool is_valid_binding(formalism::datalog::LiteralListView<T> elements, const FactSets& fact_sets, formalism::datalog::GrounderContext& context)
{
    return std::all_of(elements.begin(), elements.end(), [&](auto&& arg) { return is_valid_binding(arg, fact_sets, context); });
}

TYR_INLINE_IMPL bool
is_valid_binding(formalism::datalog::LiftedBooleanOperatorListView elements, const FactSets& fact_sets, formalism::datalog::GrounderContext& context)
{
    return std::all_of(elements.begin(), elements.end(), [&](auto&& arg) { return is_valid_binding(arg, fact_sets, context); });
}

TYR_INLINE_IMPL bool
is_valid_binding(formalism::datalog::ConjunctiveConditionView element, const FactSets& fact_sets, formalism::datalog::GrounderContext& context)
{
    return is_valid_binding(element.template get_literals<formalism::StaticTag>(), fact_sets, context)     //
           && is_valid_binding(element.template get_literals<formalism::FluentTag>(), fact_sets, context)  //
           && is_valid_binding(element.get_numeric_constraints(), fact_sets, context);
}

}
