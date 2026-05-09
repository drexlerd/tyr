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

#include "tyr/formalism/planning/merge_datalog.hpp"

#ifndef TYR_HEADER_INSTANTIATION
#include "tyr/formalism/planning/merge_datalog.ipp"

namespace tyr::formalism::planning
{

template std::pair<formalism::datalog::PredicateView<StaticTag>, bool> merge_p2d(PredicateView<StaticTag> element, MergeDatalogContext& context);
template std::pair<formalism::datalog::PredicateView<FluentTag>, bool> merge_p2d(PredicateView<FluentTag> element, MergeDatalogContext& context);
template std::pair<formalism::datalog::PredicateView<FluentTag>, bool> merge_p2d(PredicateView<DerivedTag> element, MergeDatalogContext& context);

template std::pair<formalism::datalog::AtomView<StaticTag>, bool>
merge_p2d(AtomView<StaticTag> element,
          const UnorderedMap<PredicateView<StaticTag>, formalism::datalog::PredicateView<StaticTag>>& predicate_mapping,
          MergeDatalogContext& context);
template std::pair<formalism::datalog::AtomView<FluentTag>, bool>
merge_p2d(AtomView<FluentTag> element,
          const UnorderedMap<PredicateView<FluentTag>, formalism::datalog::PredicateView<FluentTag>>& predicate_mapping,
          MergeDatalogContext& context);
template std::pair<formalism::datalog::AtomView<FluentTag>, bool>
merge_p2d(AtomView<DerivedTag> element,
          const UnorderedMap<PredicateView<DerivedTag>, formalism::datalog::PredicateView<FluentTag>>& predicate_mapping,
          MergeDatalogContext& context);
template std::pair<formalism::datalog::PredicateBindingView<StaticTag>, bool>
merge_p2d(PredicateBindingView<StaticTag> element,
          const UnorderedMap<PredicateView<StaticTag>, formalism::datalog::PredicateView<StaticTag>>& predicate_mapping,
          MergeDatalogContext& context);
template std::pair<formalism::datalog::PredicateBindingView<FluentTag>, bool>
merge_p2d(PredicateBindingView<FluentTag> element,
          const UnorderedMap<PredicateView<FluentTag>, formalism::datalog::PredicateView<FluentTag>>& predicate_mapping,
          MergeDatalogContext& context);
template std::pair<formalism::datalog::PredicateBindingView<FluentTag>, bool>
merge_p2d(PredicateBindingView<DerivedTag> element,
          const UnorderedMap<PredicateView<DerivedTag>, formalism::datalog::PredicateView<FluentTag>>& predicate_mapping,
          MergeDatalogContext& context);

template std::pair<formalism::datalog::GroundAtomView<StaticTag>, bool>
merge_p2d(GroundAtomView<StaticTag> element,
          const UnorderedMap<PredicateView<StaticTag>, formalism::datalog::PredicateView<StaticTag>>& predicate_mapping,
          MergeDatalogContext& context);
template std::pair<formalism::datalog::GroundAtomView<FluentTag>, bool>
merge_p2d(GroundAtomView<FluentTag> element,
          const UnorderedMap<PredicateView<FluentTag>, formalism::datalog::PredicateView<FluentTag>>& predicate_mapping,
          MergeDatalogContext& context);
template std::pair<formalism::datalog::GroundAtomView<FluentTag>, bool>
merge_p2d(GroundAtomView<DerivedTag> element,
          const UnorderedMap<PredicateView<DerivedTag>, formalism::datalog::PredicateView<FluentTag>>& predicate_mapping,
          MergeDatalogContext& context);

template std::pair<formalism::datalog::LiteralView<StaticTag>, bool>
merge_p2d(LiteralView<StaticTag> element,
          const UnorderedMap<PredicateView<StaticTag>, formalism::datalog::PredicateView<StaticTag>>& predicate_mapping,
          MergeDatalogContext& context);
template std::pair<formalism::datalog::LiteralView<FluentTag>, bool>
merge_p2d(LiteralView<FluentTag> element,
          const UnorderedMap<PredicateView<FluentTag>, formalism::datalog::PredicateView<FluentTag>>& predicate_mapping,
          MergeDatalogContext& context);
template std::pair<formalism::datalog::LiteralView<FluentTag>, bool>
merge_p2d(LiteralView<DerivedTag> element,
          const UnorderedMap<PredicateView<DerivedTag>, formalism::datalog::PredicateView<FluentTag>>& predicate_mapping,
          MergeDatalogContext& context);
template std::pair<formalism::datalog::GroundLiteralView<StaticTag>, bool>
merge_p2d(GroundLiteralView<StaticTag> element,
          const UnorderedMap<PredicateView<StaticTag>, formalism::datalog::PredicateView<StaticTag>>& predicate_mapping,
          MergeDatalogContext& context);
template std::pair<formalism::datalog::GroundLiteralView<FluentTag>, bool>
merge_p2d(GroundLiteralView<FluentTag> element,
          const UnorderedMap<PredicateView<FluentTag>, formalism::datalog::PredicateView<FluentTag>>& predicate_mapping,
          MergeDatalogContext& context);
template std::pair<formalism::datalog::GroundLiteralView<FluentTag>, bool>
merge_p2d(GroundLiteralView<DerivedTag> element,
          const UnorderedMap<PredicateView<DerivedTag>, formalism::datalog::PredicateView<FluentTag>>& predicate_mapping,
          MergeDatalogContext& context);

std::optional<formalism::datalog::GroundLiteralView<FluentTag>>
merge_p2d(FDRFactView<FluentTag> element,
          bool polarity,
          const UnorderedMap<PredicateView<FluentTag>, formalism::datalog::PredicateView<FluentTag>>& predicate_mapping,
          MergeDatalogContext& context);

std::pair<formalism::datalog::GroundConjunctiveConditionView, bool>
merge_p2d(GroundConjunctiveConditionView element,
          const UnorderedMap<PredicateView<FluentTag>, formalism::datalog::PredicateView<FluentTag>>& fluent_predicate_mapping,
          const UnorderedMap<PredicateView<DerivedTag>, formalism::datalog::PredicateView<FluentTag>>& derived_predicate_mapping,
          MergeDatalogContext& context);

// Numeric

template std::pair<formalism::datalog::FunctionView<StaticTag>, bool> merge_p2d(FunctionView<StaticTag> element, MergeDatalogContext& context);
template std::pair<formalism::datalog::FunctionView<FluentTag>, bool> merge_p2d(FunctionView<FluentTag> element, MergeDatalogContext& context);

template std::pair<formalism::datalog::FunctionTermView<StaticTag>, bool> merge_p2d(FunctionTermView<StaticTag> element, MergeDatalogContext& context);
template std::pair<formalism::datalog::FunctionTermView<FluentTag>, bool> merge_p2d(FunctionTermView<FluentTag> element, MergeDatalogContext& context);

template std::pair<formalism::datalog::FunctionBindingView<StaticTag>, bool> merge_p2d(FunctionBindingView<StaticTag> element, MergeDatalogContext& context);
template std::pair<formalism::datalog::FunctionBindingView<FluentTag>, bool> merge_p2d(FunctionBindingView<FluentTag> element, MergeDatalogContext& context);

template std::pair<formalism::datalog::GroundFunctionTermView<StaticTag>, bool> merge_p2d(GroundFunctionTermView<StaticTag> element,
                                                                                          MergeDatalogContext& context);
template std::pair<formalism::datalog::GroundFunctionTermView<FluentTag>, bool> merge_p2d(GroundFunctionTermView<FluentTag> element,
                                                                                          MergeDatalogContext& context);

template std::pair<formalism::datalog::GroundFunctionTermValueView<StaticTag>, bool> merge_p2d(GroundFunctionTermValueView<StaticTag> element,
                                                                                               MergeDatalogContext& context);
template std::pair<formalism::datalog::GroundFunctionTermValueView<FluentTag>, bool> merge_p2d(GroundFunctionTermValueView<FluentTag> element,
                                                                                               MergeDatalogContext& context);

template std::pair<formalism::datalog::UnaryOperatorView<Sub, Data<formalism::datalog::FunctionExpression>>, bool>
merge_p2d(UnaryOperatorView<Sub, Data<FunctionExpression>> element, MergeDatalogContext& context);
template std::pair<formalism::datalog::UnaryOperatorView<Sub, Data<formalism::datalog::GroundFunctionExpression>>, bool>
merge_p2d(UnaryOperatorView<Sub, Data<GroundFunctionExpression>> element, MergeDatalogContext& context);

template std::pair<formalism::datalog::BinaryOperatorView<Eq, Data<formalism::datalog::FunctionExpression>>, bool>
merge_p2d(BinaryOperatorView<Eq, Data<FunctionExpression>> element, MergeDatalogContext& context);
template std::pair<formalism::datalog::BinaryOperatorView<Ne, Data<formalism::datalog::FunctionExpression>>, bool>
merge_p2d(BinaryOperatorView<Ne, Data<FunctionExpression>> element, MergeDatalogContext& context);
template std::pair<formalism::datalog::BinaryOperatorView<Ge, Data<formalism::datalog::FunctionExpression>>, bool>
merge_p2d(BinaryOperatorView<Ge, Data<FunctionExpression>> element, MergeDatalogContext& context);
template std::pair<formalism::datalog::BinaryOperatorView<Gt, Data<formalism::datalog::FunctionExpression>>, bool>
merge_p2d(BinaryOperatorView<Gt, Data<FunctionExpression>> element, MergeDatalogContext& context);
template std::pair<formalism::datalog::BinaryOperatorView<Le, Data<formalism::datalog::FunctionExpression>>, bool>
merge_p2d(BinaryOperatorView<Le, Data<FunctionExpression>> element, MergeDatalogContext& context);
template std::pair<formalism::datalog::BinaryOperatorView<Lt, Data<formalism::datalog::FunctionExpression>>, bool>
merge_p2d(BinaryOperatorView<Lt, Data<FunctionExpression>> element, MergeDatalogContext& context);
template std::pair<formalism::datalog::BinaryOperatorView<Add, Data<formalism::datalog::FunctionExpression>>, bool>
merge_p2d(BinaryOperatorView<Add, Data<FunctionExpression>> element, MergeDatalogContext& context);
template std::pair<formalism::datalog::BinaryOperatorView<Sub, Data<formalism::datalog::FunctionExpression>>, bool>
merge_p2d(BinaryOperatorView<Sub, Data<FunctionExpression>> element, MergeDatalogContext& context);
template std::pair<formalism::datalog::BinaryOperatorView<Mul, Data<formalism::datalog::FunctionExpression>>, bool>
merge_p2d(BinaryOperatorView<Mul, Data<FunctionExpression>> element, MergeDatalogContext& context);
template std::pair<formalism::datalog::BinaryOperatorView<Div, Data<formalism::datalog::FunctionExpression>>, bool>
merge_p2d(BinaryOperatorView<Div, Data<FunctionExpression>> element, MergeDatalogContext& context);
template std::pair<formalism::datalog::BinaryOperatorView<Eq, Data<formalism::datalog::GroundFunctionExpression>>, bool>
merge_p2d(BinaryOperatorView<Eq, Data<GroundFunctionExpression>> element, MergeDatalogContext& context);
template std::pair<formalism::datalog::BinaryOperatorView<Ne, Data<formalism::datalog::GroundFunctionExpression>>, bool>
merge_p2d(BinaryOperatorView<Ne, Data<GroundFunctionExpression>> element, MergeDatalogContext& context);
template std::pair<formalism::datalog::BinaryOperatorView<Ge, Data<formalism::datalog::GroundFunctionExpression>>, bool>
merge_p2d(BinaryOperatorView<Ge, Data<GroundFunctionExpression>> element, MergeDatalogContext& context);
template std::pair<formalism::datalog::BinaryOperatorView<Gt, Data<formalism::datalog::GroundFunctionExpression>>, bool>
merge_p2d(BinaryOperatorView<Gt, Data<GroundFunctionExpression>> element, MergeDatalogContext& context);
template std::pair<formalism::datalog::BinaryOperatorView<Le, Data<formalism::datalog::GroundFunctionExpression>>, bool>
merge_p2d(BinaryOperatorView<Le, Data<GroundFunctionExpression>> element, MergeDatalogContext& context);
template std::pair<formalism::datalog::BinaryOperatorView<Lt, Data<formalism::datalog::GroundFunctionExpression>>, bool>
merge_p2d(BinaryOperatorView<Lt, Data<GroundFunctionExpression>> element, MergeDatalogContext& context);
template std::pair<formalism::datalog::BinaryOperatorView<Add, Data<formalism::datalog::GroundFunctionExpression>>, bool>
merge_p2d(BinaryOperatorView<Add, Data<GroundFunctionExpression>> element, MergeDatalogContext& context);
template std::pair<formalism::datalog::BinaryOperatorView<Sub, Data<formalism::datalog::GroundFunctionExpression>>, bool>
merge_p2d(BinaryOperatorView<Sub, Data<GroundFunctionExpression>> element, MergeDatalogContext& context);
template std::pair<formalism::datalog::BinaryOperatorView<Mul, Data<formalism::datalog::GroundFunctionExpression>>, bool>
merge_p2d(BinaryOperatorView<Mul, Data<GroundFunctionExpression>> element, MergeDatalogContext& context);
template std::pair<formalism::datalog::BinaryOperatorView<Div, Data<formalism::datalog::GroundFunctionExpression>>, bool>
merge_p2d(BinaryOperatorView<Div, Data<GroundFunctionExpression>> element, MergeDatalogContext& context);

template std::pair<formalism::datalog::MultiOperatorView<Add, Data<formalism::datalog::FunctionExpression>>, bool>
merge_p2d(MultiOperatorView<Add, Data<FunctionExpression>> element, MergeDatalogContext& context);
template std::pair<formalism::datalog::MultiOperatorView<Mul, Data<formalism::datalog::FunctionExpression>>, bool>
merge_p2d(MultiOperatorView<Mul, Data<FunctionExpression>> element, MergeDatalogContext& context);
template std::pair<formalism::datalog::MultiOperatorView<Add, Data<formalism::datalog::GroundFunctionExpression>>, bool>
merge_p2d(MultiOperatorView<Add, Data<GroundFunctionExpression>> element, MergeDatalogContext& context);
template std::pair<formalism::datalog::MultiOperatorView<Mul, Data<formalism::datalog::GroundFunctionExpression>>, bool>
merge_p2d(MultiOperatorView<Mul, Data<GroundFunctionExpression>> element, MergeDatalogContext& context);

template Data<formalism::datalog::ArithmeticOperator<Data<formalism::datalog::FunctionExpression>>>
merge_p2d(ArithmeticOperatorView<Data<FunctionExpression>> element, MergeDatalogContext& context);
template Data<formalism::datalog::ArithmeticOperator<Data<formalism::datalog::GroundFunctionExpression>>>
merge_p2d(ArithmeticOperatorView<Data<GroundFunctionExpression>> element, MergeDatalogContext& context);

template Data<formalism::datalog::BooleanOperator<Data<formalism::datalog::FunctionExpression>>>
merge_p2d(BooleanOperatorView<Data<FunctionExpression>> element, MergeDatalogContext& context);
template Data<formalism::datalog::BooleanOperator<Data<formalism::datalog::GroundFunctionExpression>>>
merge_p2d(BooleanOperatorView<Data<GroundFunctionExpression>> element, MergeDatalogContext& context);

}

#endif
