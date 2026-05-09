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

#include "tyr/datalog/applicability.hpp"

#ifndef TYR_HEADER_INSTANTIATION
#include "tyr/datalog/applicability.ipp"

namespace tyr::datalog
{

/**
 * evaluate
 */

template ClosedInterval<float_t> evaluate(formalism::datalog::GroundUnaryOperatorView<formalism::Sub> element, const FactSets& fact_sets);

template ClosedInterval<float_t> evaluate(formalism::datalog::GroundBinaryOperatorView<formalism::Add> element, const FactSets& fact_sets);
template ClosedInterval<float_t> evaluate(formalism::datalog::GroundBinaryOperatorView<formalism::Sub> element, const FactSets& fact_sets);
template ClosedInterval<float_t> evaluate(formalism::datalog::GroundBinaryOperatorView<formalism::Mul> element, const FactSets& fact_sets);
template ClosedInterval<float_t> evaluate(formalism::datalog::GroundBinaryOperatorView<formalism::Div> element, const FactSets& fact_sets);

template bool evaluate(formalism::datalog::GroundBinaryOperatorView<formalism::Eq> element, const FactSets& fact_sets);
template bool evaluate(formalism::datalog::GroundBinaryOperatorView<formalism::Ne> element, const FactSets& fact_sets);
template bool evaluate(formalism::datalog::GroundBinaryOperatorView<formalism::Ge> element, const FactSets& fact_sets);
template bool evaluate(formalism::datalog::GroundBinaryOperatorView<formalism::Gt> element, const FactSets& fact_sets);
template bool evaluate(formalism::datalog::GroundBinaryOperatorView<formalism::Le> element, const FactSets& fact_sets);
template bool evaluate(formalism::datalog::GroundBinaryOperatorView<formalism::Lt> element, const FactSets& fact_sets);

template ClosedInterval<float_t> evaluate(formalism::datalog::GroundMultiOperatorView<formalism::Add> element, const FactSets& fact_sets);
template ClosedInterval<float_t> evaluate(formalism::datalog::GroundMultiOperatorView<formalism::Mul> element, const FactSets& fact_sets);

template ClosedInterval<float_t> evaluate(formalism::datalog::GroundFunctionTermView<formalism::StaticTag> element, const FactSets& fact_sets);
template ClosedInterval<float_t> evaluate(formalism::datalog::GroundFunctionTermView<formalism::FluentTag> element, const FactSets& fact_sets);

/**
 * is_applicable
 */

template bool is_applicable(formalism::datalog::GroundLiteralView<formalism::StaticTag> element, const FactSets& fact_sets);
template bool is_applicable(formalism::datalog::GroundLiteralView<formalism::FluentTag> element, const FactSets& fact_sets);

template bool is_applicable(formalism::datalog::GroundLiteralListView<formalism::StaticTag> elements, const FactSets& fact_sets);
template bool is_applicable(formalism::datalog::GroundLiteralListView<formalism::FluentTag> elements, const FactSets& fact_sets);

// GroundConjunctiveCondition

// GroundRule

/**
 * is_valid_binding
 */

template bool
is_valid_binding(formalism::datalog::LiteralView<formalism::StaticTag> element, const FactSets& fact_sets, formalism::datalog::GrounderContext& context);
template bool
is_valid_binding(formalism::datalog::LiteralView<formalism::FluentTag> element, const FactSets& fact_sets, formalism::datalog::GrounderContext& context);

template bool
is_valid_binding(formalism::datalog::LiteralListView<formalism::StaticTag> elements, const FactSets& fact_sets, formalism::datalog::GrounderContext& context);
template bool
is_valid_binding(formalism::datalog::LiteralListView<formalism::FluentTag> elements, const FactSets& fact_sets, formalism::datalog::GrounderContext& context);

template ClosedInterval<float_t> is_valid_binding(formalism::datalog::NumericEffectView<formalism::Assign, formalism::FluentTag> element,
                                                  const FactSets& fact_sets,
                                                  formalism::datalog::GrounderContext& context);
template ClosedInterval<float_t> is_valid_binding(formalism::datalog::NumericEffectView<formalism::Increase, formalism::FluentTag> element,
                                                  const FactSets& fact_sets,
                                                  formalism::datalog::GrounderContext& context);
template ClosedInterval<float_t> is_valid_binding(formalism::datalog::NumericEffectView<formalism::Decrease, formalism::FluentTag> element,
                                                  const FactSets& fact_sets,
                                                  formalism::datalog::GrounderContext& context);
template ClosedInterval<float_t> is_valid_binding(formalism::datalog::NumericEffectView<formalism::ScaleUp, formalism::FluentTag> element,
                                                  const FactSets& fact_sets,
                                                  formalism::datalog::GrounderContext& context);
template ClosedInterval<float_t> is_valid_binding(formalism::datalog::NumericEffectView<formalism::ScaleDown, formalism::FluentTag> element,
                                                  const FactSets& fact_sets,
                                                  formalism::datalog::GrounderContext& context);
template ClosedInterval<float_t> is_valid_binding(formalism::datalog::NumericEffectOperatorView<formalism::FluentTag> element,
                                                  const FactSets& fact_sets,
                                                  formalism::datalog::GrounderContext& context);

}

#endif
