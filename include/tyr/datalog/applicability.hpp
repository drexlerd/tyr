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

#ifndef TYR_DATALOG_APPLICABILITY_HPP_
#define TYR_DATALOG_APPLICABILITY_HPP_

#include "tyr/common/semantics.hpp"
#include "tyr/common/vector.hpp"
#include "tyr/datalog/declarations.hpp"
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

template<SemanticTag S>
float_t evaluate(float_t element, const FactSetAccessor<S>& accessor);

template<formalism::ArithmeticOpKind O, SemanticTag S>
float_t evaluate(formalism::datalog::GroundUnaryOperatorView<O> element, const FactSetAccessor<S>& accessor);

template<formalism::ArithmeticOpKind O, SemanticTag S>
float_t evaluate(formalism::datalog::GroundBinaryOperatorView<O> element, const FactSetAccessor<S>& accessor);

template<formalism::BooleanOpKind O, SemanticTag S>
bool evaluate(formalism::datalog::GroundBinaryOperatorView<O> element, const FactSetAccessor<S>& accessor);

template<formalism::ArithmeticOpKind O, SemanticTag S>
float_t evaluate(formalism::datalog::GroundMultiOperatorView<O> element, const FactSetAccessor<S>& accessor);

template<formalism::FactKind T, SemanticTag S>
float_t evaluate(formalism::datalog::GroundFunctionTermView<T> element, const FactSetAccessor<S>& accessor);

template<SemanticTag S>
float_t evaluate(formalism::datalog::GroundFunctionExpressionView element, const FactSetAccessor<S>& accessor);

template<SemanticTag S>
float_t evaluate(formalism::datalog::GroundArithmeticOperatorView element, const FactSetAccessor<S>& accessor);

template<SemanticTag S>
bool evaluate(formalism::datalog::GroundBooleanOperatorView element, const FactSetAccessor<S>& accessor);

/**
 * is_applicable
 */

template<formalism::FactKind T, SemanticTag S>
bool is_applicable(formalism::datalog::GroundLiteralView<T> element, const FactSetAccessor<S>& accessor);

template<formalism::FactKind T, SemanticTag S>
bool is_applicable(formalism::datalog::GroundLiteralListView<T> elements, const FactSetAccessor<S>& accessor);

template<SemanticTag S>
bool is_applicable(formalism::datalog::GroundBooleanOperatorListView elements, const FactSetAccessor<S>& accessor);

template<SemanticTag S>
bool is_applicable(formalism::datalog::GroundConjunctiveConditionView element, const FactSetAccessor<S>& accessor);

template<SemanticTag S>
bool is_applicable(formalism::datalog::GroundRuleView element, const FactSetAccessor<S>& accessor);

/**
 * evaluate
 */

template<SemanticTag S>
float_t evaluate(float_t element, const FactSetAccessor<S>& accessor, const formalism::datalog::GrounderContext&);

template<formalism::ArithmeticOpKind O, SemanticTag S>
float_t
evaluate(formalism::datalog::LiftedUnaryOperatorView<O> element, const FactSetAccessor<S>& accessor, const formalism::datalog::GrounderContext& context);

template<formalism::ArithmeticOpKind O, SemanticTag S>
float_t
evaluate(formalism::datalog::LiftedBinaryOperatorView<O> element, const FactSetAccessor<S>& accessor, const formalism::datalog::GrounderContext& context);

template<formalism::BooleanOpKind O, SemanticTag S>
bool evaluate(formalism::datalog::LiftedBinaryOperatorView<O> element, const FactSetAccessor<S>& accessor, const formalism::datalog::GrounderContext& context);

template<formalism::ArithmeticOpKind O, SemanticTag S>
float_t
evaluate(formalism::datalog::LiftedMultiOperatorView<O> element, const FactSetAccessor<S>& accessor, const formalism::datalog::GrounderContext& context);

template<formalism::FactKind T, SemanticTag S>
float_t evaluate(formalism::datalog::FunctionTermView<T> element, const FactSetAccessor<S>& accessor, const formalism::datalog::GrounderContext& context);

template<SemanticTag S>
float_t evaluate(formalism::datalog::FunctionExpressionView element, const FactSetAccessor<S>& accessor, const formalism::datalog::GrounderContext& context);

template<SemanticTag S>
float_t
evaluate(formalism::datalog::LiftedArithmeticOperatorView element, const FactSetAccessor<S>& accessor, const formalism::datalog::GrounderContext& context);

template<SemanticTag S>
bool evaluate(formalism::datalog::LiftedBooleanOperatorView element, const FactSetAccessor<S>& accessor, const formalism::datalog::GrounderContext& context);

/**
 * is_valid_binding
 */

template<formalism::FactKind T, SemanticTag S>
bool is_valid_binding(formalism::datalog::LiteralView<T> element, const FactSetAccessor<S>& accessor, const formalism::datalog::GrounderContext& context);

template<formalism::FactKind T, SemanticTag S>
bool is_valid_binding(formalism::datalog::LiteralListView<T> elements, const FactSetAccessor<S>& accessor, const formalism::datalog::GrounderContext& context);

template<SemanticTag S>
bool is_valid_binding(formalism::datalog::LiftedBooleanOperatorListView elements,
                      const FactSetAccessor<S>& accessor,
                      const formalism::datalog::GrounderContext& context);

template<SemanticTag S>
bool is_valid_binding(formalism::datalog::ConjunctiveConditionView element,
                      const FactSetAccessor<S>& accessor,
                      const formalism::datalog::GrounderContext& context);

}

#ifdef TYR_HEADER_INSTANTIATION
#include "tyr/datalog/applicability.ipp"
#endif

#endif
