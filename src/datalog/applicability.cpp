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

template float_t evaluate(formalism::datalog::GroundUnaryOperatorView<formalism::OpSub> element, const FactSets& fact_sets);

template float_t evaluate(formalism::datalog::GroundBinaryOperatorView<formalism::OpAdd> element, const FactSets& fact_sets);
template float_t evaluate(formalism::datalog::GroundBinaryOperatorView<formalism::OpSub> element, const FactSets& fact_sets);
template float_t evaluate(formalism::datalog::GroundBinaryOperatorView<formalism::OpMul> element, const FactSets& fact_sets);
template float_t evaluate(formalism::datalog::GroundBinaryOperatorView<formalism::OpDiv> element, const FactSets& fact_sets);

template bool evaluate(formalism::datalog::GroundBinaryOperatorView<formalism::OpEq> element, const FactSets& fact_sets);
template bool evaluate(formalism::datalog::GroundBinaryOperatorView<formalism::OpNe> element, const FactSets& fact_sets);
template bool evaluate(formalism::datalog::GroundBinaryOperatorView<formalism::OpGe> element, const FactSets& fact_sets);
template bool evaluate(formalism::datalog::GroundBinaryOperatorView<formalism::OpGt> element, const FactSets& fact_sets);
template bool evaluate(formalism::datalog::GroundBinaryOperatorView<formalism::OpLe> element, const FactSets& fact_sets);
template bool evaluate(formalism::datalog::GroundBinaryOperatorView<formalism::OpLt> element, const FactSets& fact_sets);

template float_t evaluate(formalism::datalog::GroundMultiOperatorView<formalism::OpAdd> element, const FactSets& fact_sets);
template float_t evaluate(formalism::datalog::GroundMultiOperatorView<formalism::OpMul> element, const FactSets& fact_sets);

template float_t evaluate(formalism::datalog::GroundFunctionTermView<formalism::StaticTag> element, const FactSets& fact_sets);
template float_t evaluate(formalism::datalog::GroundFunctionTermView<formalism::FluentTag> element, const FactSets& fact_sets);

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

}

#endif
