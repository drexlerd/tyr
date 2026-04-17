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
#include "tyr/datalog/fact_sets_accessor.hpp"
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

template<FactSetCareAccessorConcept P>
float_t evaluate(float_t element, const P& policy)
{
    return element;
}

template<formalism::ArithmeticOpKind O, FactSetCareAccessorConcept P>
float_t evaluate(formalism::datalog::GroundUnaryOperatorView<O> element, const P& policy)
{
    return formalism::apply(O {}, evaluate(element.get_arg(), policy));
}

template<formalism::ArithmeticOpKind O, FactSetCareAccessorConcept P>
float_t evaluate(formalism::datalog::GroundBinaryOperatorView<O> element, const P& policy)
{
    return formalism::apply(O {}, evaluate(element.get_lhs(), policy), evaluate(element.get_rhs(), policy));
}

template<formalism::BooleanOpKind O, FactSetCareAccessorConcept P>
bool evaluate(formalism::datalog::GroundBinaryOperatorView<O> element, const P& policy)
{
    return formalism::apply(O {}, evaluate(element.get_lhs(), policy), evaluate(element.get_rhs(), policy));
}

template<formalism::ArithmeticOpKind O, FactSetCareAccessorConcept P>
float_t evaluate(formalism::datalog::GroundMultiOperatorView<O> element, const P& policy)
{
    const auto child_fexprs = element.get_args();

    return std::accumulate(std::next(child_fexprs.begin()),  // Start from the second expression
                           child_fexprs.end(),
                           evaluate(child_fexprs.front(), policy),
                           [&](const auto& value, const auto& child_expr) { return formalism::apply(O {}, value, evaluate(child_expr, policy)); });
}

template<formalism::FactKind T, FactSetCareAccessorConcept P>
float_t evaluate(formalism::datalog::GroundFunctionTermView<T> element, const P& policy)
{
    return policy.template get<T>().function.check(element);
}

template<FactSetCareAccessorConcept P>
float_t evaluate(formalism::datalog::GroundFunctionExpressionView element, const P& policy)
{
    return visit([&](auto&& arg) { return evaluate(arg, policy); }, element.get_variant());
}

template<FactSetCareAccessorConcept P>
float_t evaluate(formalism::datalog::GroundArithmeticOperatorView element, const P& policy)
{
    return visit([&](auto&& arg) { return evaluate(arg, policy); }, element.get_variant());
}

template<FactSetCareAccessorConcept P>
bool evaluate(formalism::datalog::GroundBooleanOperatorView element, const P& policy)
{
    return visit([&](auto&& arg) { return evaluate(arg, policy); }, element.get_variant());
}

/**
 * is_applicable
 */

template<formalism::FactKind T, FactSetCareAccessorConcept P>
bool is_applicable(formalism::datalog::GroundLiteralView<T> element, const P& policy)
{
    return policy.template get<T>().predicate.check(element);
}

template<formalism::FactKind T, FactSetCareAccessorConcept P>
bool is_applicable(formalism::datalog::GroundLiteralListView<T> elements, const P& policy)
{
    return std::all_of(elements.begin(), elements.end(), [&](auto&& arg) { return is_applicable(arg, policy); });
}

template<FactSetCareAccessorConcept P>
bool is_applicable(formalism::datalog::GroundBooleanOperatorListView elements, const P& policy)
{
    return std::all_of(elements.begin(), elements.end(), [&](auto&& arg) { return evaluate(arg, policy); });
}

template<FactSetCareAccessorConcept P>
bool is_applicable(formalism::datalog::GroundConjunctiveConditionView element, const P& policy)
{
    return is_applicable(element.template get_literals<formalism::StaticTag>(), policy)     //
           && is_applicable(element.template get_literals<formalism::FluentTag>(), policy)  //
           && is_applicable(element.get_numeric_constraints(), policy);
}

template<FactSetCareAccessorConcept P>
bool is_applicable(formalism::datalog::GroundRuleView element, const P& policy)
{
    return is_applicable(element.get_body(), policy);
}

/**
 * evaluate
 */

template<FactSetCareAccessorConcept P>
float_t evaluate(float_t element, const P&, const formalism::datalog::GrounderContext&)
{
    return element;
}

template<formalism::ArithmeticOpKind O, FactSetCareAccessorConcept P>
float_t evaluate(formalism::datalog::LiftedUnaryOperatorView<O> element, const P& policy, const formalism::datalog::GrounderContext& context)
{
    return formalism::apply(O {}, evaluate(element.get_arg(), policy, context));
}

template<formalism::ArithmeticOpKind O, FactSetCareAccessorConcept P>
float_t evaluate(formalism::datalog::LiftedBinaryOperatorView<O> element, const P& policy, const formalism::datalog::GrounderContext& context)
{
    return formalism::apply(O {}, evaluate(element.get_lhs(), policy, context), evaluate(element.get_rhs(), policy, context));
}

template<formalism::BooleanOpKind O, FactSetCareAccessorConcept P>
bool evaluate(formalism::datalog::LiftedBinaryOperatorView<O> element, const P& policy, const formalism::datalog::GrounderContext& context)
{
    return formalism::apply(O {}, evaluate(element.get_lhs(), policy, context), evaluate(element.get_rhs(), policy, context));
}

template<formalism::ArithmeticOpKind O, FactSetCareAccessorConcept P>
float_t evaluate(formalism::datalog::LiftedMultiOperatorView<O> element, const P& policy, const formalism::datalog::GrounderContext& context)
{
    const auto child_fexprs = element.get_args();

    return std::accumulate(std::next(child_fexprs.begin()),  // Start from the second expression
                           child_fexprs.end(),
                           evaluate(child_fexprs.front(), policy, context),
                           [&](const auto& value, const auto& child_expr) { return formalism::apply(O {}, value, evaluate(child_expr, policy, context)); });
}

template<formalism::FactKind T, FactSetCareAccessorConcept P>
float_t evaluate(formalism::datalog::FunctionTermView<T> element, const P& policy, const formalism::datalog::GrounderContext& context)
{
    return policy.template get<T>().function.check(element, context);
}

template<FactSetCareAccessorConcept P>
float_t evaluate(formalism::datalog::FunctionExpressionView element, const P& policy, const formalism::datalog::GrounderContext& context)
{
    return visit([&](auto&& arg) { return evaluate(arg, policy, context); }, element.get_variant());
}

template<FactSetCareAccessorConcept P>
float_t evaluate(formalism::datalog::LiftedArithmeticOperatorView element, const P& policy, const formalism::datalog::GrounderContext& context)
{
    return visit([&](auto&& arg) { return evaluate(arg, policy, context); }, element.get_variant());
}

template<FactSetCareAccessorConcept P>
bool evaluate(formalism::datalog::LiftedBooleanOperatorView element, const P& policy, const formalism::datalog::GrounderContext& context)
{
    return visit([&](auto&& arg) { return evaluate(arg, policy, context); }, element.get_variant());
}

/**
 * is_valid_binding
 */

template<formalism::FactKind T, FactSetCareAccessorConcept P>
bool is_valid_binding(formalism::datalog::LiteralView<T> element, const P& policy, const formalism::datalog::GrounderContext& context)
{
    return policy.template get<T>().predicate.check(element, context);
}

template<formalism::FactKind T, FactSetCareAccessorConcept P>
bool is_valid_binding(formalism::datalog::LiteralListView<T> elements, const P& policy, const formalism::datalog::GrounderContext& context)
{
    return std::all_of(elements.begin(), elements.end(), [&](auto&& arg) { return is_valid_binding(arg, policy, context); });
}

template<FactSetCareAccessorConcept P>
bool is_valid_binding(formalism::datalog::LiftedBooleanOperatorListView elements, const P& policy, const formalism::datalog::GrounderContext& context)
{
    return std::all_of(elements.begin(), elements.end(), [&](auto&& arg) { return evaluate(arg, policy, context); });
}

template<FactSetCareAccessorConcept P>
bool is_valid_binding(formalism::datalog::ConjunctiveConditionView element, const P& policy, const formalism::datalog::GrounderContext& context)
{
    return is_valid_binding(element.template get_literals<formalism::StaticTag>(), policy, context)     //
           && is_valid_binding(element.template get_literals<formalism::FluentTag>(), policy, context)  //
           && is_valid_binding(element.get_numeric_constraints(), policy, context);
}

}
