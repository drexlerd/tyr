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
#include "tyr/common/semantics.hpp"
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

template<SemanticTag S>
float_t evaluate(float_t element, const FactSetAccessor<S>& accessor)
{
    return element;
}

template<formalism::ArithmeticOpKind O, SemanticTag S>
float_t evaluate(formalism::datalog::GroundUnaryOperatorView<O> element, const FactSetAccessor<S>& accessor)
{
    return formalism::apply(O {}, evaluate(element.get_arg(), accessor));
}

template<formalism::ArithmeticOpKind O, SemanticTag S>
float_t evaluate(formalism::datalog::GroundBinaryOperatorView<O> element, const FactSetAccessor<S>& accessor)
{
    return formalism::apply(O {}, evaluate(element.get_lhs(), accessor), evaluate(element.get_rhs(), accessor));
}

template<formalism::BooleanOpKind O, SemanticTag S>
bool evaluate(formalism::datalog::GroundBinaryOperatorView<O> element, const FactSetAccessor<S>& accessor)
{
    return formalism::apply(O {}, evaluate(element.get_lhs(), accessor), evaluate(element.get_rhs(), accessor));
}

template<formalism::ArithmeticOpKind O, SemanticTag S>
float_t evaluate(formalism::datalog::GroundMultiOperatorView<O> element, const FactSetAccessor<S>& accessor)
{
    const auto child_fexprs = element.get_args();

    return std::accumulate(std::next(child_fexprs.begin()),  // Start from the second expression
                           child_fexprs.end(),
                           evaluate(child_fexprs.front(), accessor),
                           [&](const auto& value, const auto& child_expr) { return formalism::apply(O {}, value, evaluate(child_expr, accessor)); });
}

template<formalism::FactKind T, SemanticTag S>
float_t evaluate(formalism::datalog::GroundFunctionTermView<T> element, const FactSetAccessor<S>& accessor)
{
    return accessor.template get<T>().function.check(element);
}

template<SemanticTag S>
float_t evaluate(formalism::datalog::GroundFunctionExpressionView element, const FactSetAccessor<S>& accessor)
{
    return visit([&](auto&& arg) { return evaluate(arg, accessor); }, element.get_variant());
}

template<SemanticTag S>
float_t evaluate(formalism::datalog::GroundArithmeticOperatorView element, const FactSetAccessor<S>& accessor)
{
    return visit([&](auto&& arg) { return evaluate(arg, accessor); }, element.get_variant());
}

template<SemanticTag S>
bool evaluate(formalism::datalog::GroundBooleanOperatorView element, const FactSetAccessor<S>& accessor)
{
    return visit([&](auto&& arg) { return evaluate(arg, accessor); }, element.get_variant());
}

/**
 * is_applicable
 */

template<formalism::FactKind T, SemanticTag S>
bool is_applicable(formalism::datalog::GroundLiteralView<T> element, const FactSetAccessor<S>& accessor)
{
    return accessor.template get<T>().predicate.check(element);
}

template<formalism::FactKind T, SemanticTag S>
bool is_applicable(formalism::datalog::GroundLiteralListView<T> elements, const FactSetAccessor<S>& accessor)
{
    return std::all_of(elements.begin(), elements.end(), [&](auto&& arg) { return is_applicable(arg, accessor); });
}

template<SemanticTag S>
bool is_applicable(formalism::datalog::GroundBooleanOperatorListView elements, const FactSetAccessor<S>& accessor)
{
    return std::all_of(elements.begin(), elements.end(), [&](auto&& arg) { return evaluate(arg, accessor); });
}

template<SemanticTag S>
bool is_applicable(formalism::datalog::GroundConjunctiveConditionView element, const FactSetAccessor<S>& accessor)
{
    return is_applicable(element.template get_literals<formalism::StaticTag>(), accessor)     //
           && is_applicable(element.template get_literals<formalism::FluentTag>(), accessor)  //
           && is_applicable(element.get_numeric_constraints(), accessor);
}

template<SemanticTag S>
bool is_applicable(formalism::datalog::GroundRuleView element, const FactSetAccessor<S>& accessor)
{
    return is_applicable(element.get_body(), accessor);
}

/**
 * evaluate
 */

template<SemanticTag S>
float_t evaluate(float_t element, const FactSetAccessor<S>&, const formalism::datalog::GrounderContext&)
{
    return element;
}

template<formalism::ArithmeticOpKind O, SemanticTag S>
float_t evaluate(formalism::datalog::LiftedUnaryOperatorView<O> element, const FactSetAccessor<S>& accessor, const formalism::datalog::GrounderContext& context)
{
    return formalism::apply(O {}, evaluate(element.get_arg(), accessor, context));
}

template<formalism::ArithmeticOpKind O, SemanticTag S>
float_t
evaluate(formalism::datalog::LiftedBinaryOperatorView<O> element, const FactSetAccessor<S>& accessor, const formalism::datalog::GrounderContext& context)
{
    return formalism::apply(O {}, evaluate(element.get_lhs(), accessor, context), evaluate(element.get_rhs(), accessor, context));
}

template<formalism::BooleanOpKind O, SemanticTag S>
bool evaluate(formalism::datalog::LiftedBinaryOperatorView<O> element, const FactSetAccessor<S>& accessor, const formalism::datalog::GrounderContext& context)
{
    return formalism::apply(O {}, evaluate(element.get_lhs(), accessor, context), evaluate(element.get_rhs(), accessor, context));
}

template<formalism::ArithmeticOpKind O, SemanticTag S>
float_t evaluate(formalism::datalog::LiftedMultiOperatorView<O> element, const FactSetAccessor<S>& accessor, const formalism::datalog::GrounderContext& context)
{
    const auto child_fexprs = element.get_args();

    return std::accumulate(std::next(child_fexprs.begin()),  // Start from the second expression
                           child_fexprs.end(),
                           evaluate(child_fexprs.front(), accessor, context),
                           [&](const auto& value, const auto& child_expr) { return formalism::apply(O {}, value, evaluate(child_expr, accessor, context)); });
}

template<formalism::FactKind T, SemanticTag S>
float_t evaluate(formalism::datalog::FunctionTermView<T> element, const FactSetAccessor<S>& accessor, const formalism::datalog::GrounderContext& context)
{
    return accessor.template get<T>().function.check(element, context);
}

template<SemanticTag S>
float_t evaluate(formalism::datalog::FunctionExpressionView element, const FactSetAccessor<S>& accessor, const formalism::datalog::GrounderContext& context)
{
    return visit([&](auto&& arg) { return evaluate(arg, accessor, context); }, element.get_variant());
}

template<SemanticTag S>
float_t
evaluate(formalism::datalog::LiftedArithmeticOperatorView element, const FactSetAccessor<S>& accessor, const formalism::datalog::GrounderContext& context)
{
    return visit([&](auto&& arg) { return evaluate(arg, accessor, context); }, element.get_variant());
}

template<SemanticTag S>
bool evaluate(formalism::datalog::LiftedBooleanOperatorView element, const FactSetAccessor<S>& accessor, const formalism::datalog::GrounderContext& context)
{
    return visit([&](auto&& arg) { return evaluate(arg, accessor, context); }, element.get_variant());
}

/**
 * is_valid_binding
 */

template<formalism::FactKind T, SemanticTag S>
bool is_valid_binding(formalism::datalog::LiteralView<T> element, const FactSetAccessor<S>& accessor, const formalism::datalog::GrounderContext& context)
{
    return accessor.template get<T>().predicate.check(element, context);
}

template<formalism::FactKind T, SemanticTag S>
bool is_valid_binding(formalism::datalog::LiteralListView<T> elements, const FactSetAccessor<S>& accessor, const formalism::datalog::GrounderContext& context)
{
    return std::all_of(elements.begin(), elements.end(), [&](auto&& arg) { return is_valid_binding(arg, accessor, context); });
}

template<SemanticTag S>
bool is_valid_binding(formalism::datalog::LiftedBooleanOperatorListView elements,
                      const FactSetAccessor<S>& accessor,
                      const formalism::datalog::GrounderContext& context)
{
    return std::all_of(elements.begin(), elements.end(), [&](auto&& arg) { return evaluate(arg, accessor, context); });
}

template<SemanticTag S>
bool is_valid_binding(formalism::datalog::ConjunctiveConditionView element,
                      const FactSetAccessor<S>& accessor,
                      const formalism::datalog::GrounderContext& context)
{
    return is_valid_binding(element.template get_literals<formalism::StaticTag>(), accessor, context)     //
           && is_valid_binding(element.template get_literals<formalism::FluentTag>(), accessor, context)  //
           && is_valid_binding(element.get_numeric_constraints(), accessor, context);
}

}
