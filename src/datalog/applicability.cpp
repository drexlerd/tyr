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

template float_t evaluate(formalism::datalog::GroundFunctionExpressionView element, const FactSetAccessor<StandardSemanticTag>& accessor);
template float_t evaluate(formalism::datalog::GroundFunctionExpressionView element, const FactSetAccessor<CareSemanticTag>& accessor);

template float_t evaluate(formalism::datalog::GroundArithmeticOperatorView element, const FactSetAccessor<StandardSemanticTag>& accessor);
template float_t evaluate(formalism::datalog::GroundArithmeticOperatorView element, const FactSetAccessor<CareSemanticTag>& accessor);

template float_t evaluate(formalism::datalog::FunctionExpressionView element,
                          const FactSetAccessor<StandardSemanticTag>& accessor,
                          const formalism::datalog::GrounderContext& context);
template float_t evaluate(formalism::datalog::FunctionExpressionView element,
                          const FactSetAccessor<CareSemanticTag>& accessor,
                          const formalism::datalog::GrounderContext& context);

template float_t evaluate(formalism::datalog::LiftedArithmeticOperatorView element,
                          const FactSetAccessor<StandardSemanticTag>& accessor,
                          const formalism::datalog::GrounderContext& context);
template float_t evaluate(formalism::datalog::LiftedArithmeticOperatorView element,
                          const FactSetAccessor<CareSemanticTag>& accessor,
                          const formalism::datalog::GrounderContext& context);

template bool evaluate(formalism::datalog::GroundBooleanOperatorView element, const FactSetAccessor<StandardSemanticTag>& accessor);
template bool evaluate(formalism::datalog::GroundBooleanOperatorView element, const FactSetAccessor<CareSemanticTag>& accessor);

template bool evaluate(formalism::datalog::LiftedBooleanOperatorView element,
                       const FactSetAccessor<StandardSemanticTag>& accessor,
                       const formalism::datalog::GrounderContext& context);
template bool evaluate(formalism::datalog::LiftedBooleanOperatorView element,
                       const FactSetAccessor<CareSemanticTag>& accessor,
                       const formalism::datalog::GrounderContext& context);

/**
 * is_applicable
 */

template bool is_applicable(formalism::datalog::GroundLiteralView<formalism::StaticTag> element, const FactSetAccessor<StandardSemanticTag>& accessor);
template bool is_applicable(formalism::datalog::GroundLiteralView<formalism::FluentTag> element, const FactSetAccessor<StandardSemanticTag>& accessor);
template bool is_applicable(formalism::datalog::GroundLiteralView<formalism::StaticTag> element, const FactSetAccessor<CareSemanticTag>& accessor);
template bool is_applicable(formalism::datalog::GroundLiteralView<formalism::FluentTag> element, const FactSetAccessor<CareSemanticTag>& accessor);

template bool is_applicable(formalism::datalog::GroundLiteralListView<formalism::StaticTag> elements, const FactSetAccessor<StandardSemanticTag>& accessor);
template bool is_applicable(formalism::datalog::GroundLiteralListView<formalism::FluentTag> elements, const FactSetAccessor<StandardSemanticTag>& accessor);
template bool is_applicable(formalism::datalog::GroundLiteralListView<formalism::StaticTag> elements, const FactSetAccessor<CareSemanticTag>& accessor);
template bool is_applicable(formalism::datalog::GroundLiteralListView<formalism::FluentTag> elements, const FactSetAccessor<CareSemanticTag>& accessor);

template bool is_applicable(formalism::datalog::GroundBooleanOperatorListView elements, const FactSetAccessor<StandardSemanticTag>& accessor);
template bool is_applicable(formalism::datalog::GroundBooleanOperatorListView elements, const FactSetAccessor<CareSemanticTag>& accessor);

template bool is_applicable(formalism::datalog::GroundConjunctiveConditionView element, const FactSetAccessor<StandardSemanticTag>& accessor);
template bool is_applicable(formalism::datalog::GroundConjunctiveConditionView element, const FactSetAccessor<CareSemanticTag>& accessor);

template bool is_applicable(formalism::datalog::GroundRuleView element, const FactSetAccessor<StandardSemanticTag>& accessor);
template bool is_applicable(formalism::datalog::GroundRuleView element, const FactSetAccessor<CareSemanticTag>& accessor);

/**
 * is_valid_binding
 */

template bool is_valid_binding(formalism::datalog::LiteralView<formalism::StaticTag> element,
                               const FactSetAccessor<StandardSemanticTag>& accessor,
                               const formalism::datalog::GrounderContext& context);
template bool is_valid_binding(formalism::datalog::LiteralView<formalism::FluentTag> element,
                               const FactSetAccessor<StandardSemanticTag>& accessor,
                               const formalism::datalog::GrounderContext& context);
template bool is_valid_binding(formalism::datalog::LiteralView<formalism::StaticTag> element,
                               const FactSetAccessor<CareSemanticTag>& accessor,
                               const formalism::datalog::GrounderContext& context);
template bool is_valid_binding(formalism::datalog::LiteralView<formalism::FluentTag> element,
                               const FactSetAccessor<CareSemanticTag>& accessor,
                               const formalism::datalog::GrounderContext& context);

template bool is_valid_binding(formalism::datalog::LiteralListView<formalism::StaticTag> elements,
                               const FactSetAccessor<StandardSemanticTag>& accessor,
                               const formalism::datalog::GrounderContext& context);
template bool is_valid_binding(formalism::datalog::LiteralListView<formalism::FluentTag> elements,
                               const FactSetAccessor<StandardSemanticTag>& accessor,
                               const formalism::datalog::GrounderContext& context);
template bool is_valid_binding(formalism::datalog::LiteralListView<formalism::StaticTag> elements,
                               const FactSetAccessor<CareSemanticTag>& accessor,
                               const formalism::datalog::GrounderContext& context);
template bool is_valid_binding(formalism::datalog::LiteralListView<formalism::FluentTag> elements,
                               const FactSetAccessor<CareSemanticTag>& accessor,
                               const formalism::datalog::GrounderContext& context);

template bool is_valid_binding(formalism::datalog::LiftedBooleanOperatorListView elements,
                               const FactSetAccessor<StandardSemanticTag>& accessor,
                               const formalism::datalog::GrounderContext& context);
template bool is_valid_binding(formalism::datalog::LiftedBooleanOperatorListView elements,
                               const FactSetAccessor<CareSemanticTag>& accessor,
                               const formalism::datalog::GrounderContext& context);

template bool is_valid_binding(formalism::datalog::ConjunctiveConditionView element,
                               const FactSetAccessor<StandardSemanticTag>& accessor,
                               const formalism::datalog::GrounderContext& context);
template bool is_valid_binding(formalism::datalog::ConjunctiveConditionView element,
                               const FactSetAccessor<CareSemanticTag>& accessor,
                               const formalism::datalog::GrounderContext& context);

}  // namespace tyr::datalog

#endif