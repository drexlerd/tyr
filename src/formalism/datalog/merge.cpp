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

#include "tyr/formalism/datalog/merge.hpp"

#ifndef TYR_HEADER_INSTANTIATION

namespace tyr::formalism::datalog
{
template std::pair<PredicateView<StaticTag>, bool> merge_d2d(PredicateView<StaticTag> element, MergeContext& context);
template std::pair<PredicateView<FluentTag>, bool> merge_d2d(PredicateView<FluentTag> element, MergeContext& context);

template std::pair<AtomView<StaticTag>, bool> merge_d2d(AtomView<StaticTag> element, MergeContext& context);
template std::pair<AtomView<FluentTag>, bool> merge_d2d(AtomView<FluentTag> element, MergeContext& context);

template std::pair<PredicateBindingView<StaticTag>, bool> merge_d2d(PredicateBindingView<StaticTag> element, MergeContext& context);
template std::pair<PredicateBindingView<FluentTag>, bool> merge_d2d(PredicateBindingView<FluentTag> element, MergeContext& context);

template std::pair<GroundAtomView<StaticTag>, bool> merge_d2d(GroundAtomView<StaticTag> element, MergeContext& context);
template std::pair<GroundAtomView<FluentTag>, bool> merge_d2d(GroundAtomView<FluentTag> element, MergeContext& context);

template std::pair<LiteralView<StaticTag>, bool> merge_d2d(LiteralView<StaticTag> element, MergeContext& context);
template std::pair<LiteralView<FluentTag>, bool> merge_d2d(LiteralView<FluentTag> element, MergeContext& context);

template std::pair<GroundLiteralView<StaticTag>, bool> merge_d2d(GroundLiteralView<StaticTag> element, MergeContext& context);
template std::pair<GroundLiteralView<FluentTag>, bool> merge_d2d(GroundLiteralView<FluentTag> element, MergeContext& context);

template std::pair<FunctionView<StaticTag>, bool> merge_d2d(FunctionView<StaticTag> element, MergeContext& context);
template std::pair<FunctionView<FluentTag>, bool> merge_d2d(FunctionView<FluentTag> element, MergeContext& context);

template std::pair<FunctionTermView<StaticTag>, bool> merge_d2d(FunctionTermView<StaticTag> element, MergeContext& context);
template std::pair<FunctionTermView<FluentTag>, bool> merge_d2d(FunctionTermView<FluentTag> element, MergeContext& context);

template std::pair<FunctionBindingView<StaticTag>, bool> merge_d2d(FunctionBindingView<StaticTag> element, MergeContext& context);
template std::pair<FunctionBindingView<FluentTag>, bool> merge_d2d(FunctionBindingView<FluentTag> element, MergeContext& context);

template std::pair<GroundFunctionTermView<StaticTag>, bool> merge_d2d(GroundFunctionTermView<StaticTag> element, MergeContext& context);
template std::pair<GroundFunctionTermView<FluentTag>, bool> merge_d2d(GroundFunctionTermView<FluentTag> element, MergeContext& context);

template std::pair<GroundFunctionTermValueView<StaticTag>, bool> merge_d2d(GroundFunctionTermValueView<StaticTag> element, MergeContext& context);
template std::pair<GroundFunctionTermValueView<FluentTag>, bool> merge_d2d(GroundFunctionTermValueView<FluentTag> element, MergeContext& context);

template std::pair<UnaryOperatorView<Sub, Data<FunctionExpression>>, bool> merge_d2d(UnaryOperatorView<Sub, Data<FunctionExpression>> element,
                                                                                     MergeContext& context);
template std::pair<UnaryOperatorView<Sub, Data<GroundFunctionExpression>>, bool> merge_d2d(UnaryOperatorView<Sub, Data<GroundFunctionExpression>> element,
                                                                                           MergeContext& context);

template std::pair<BinaryOperatorView<Eq, Data<FunctionExpression>>, bool> merge_d2d(BinaryOperatorView<Eq, Data<FunctionExpression>> element,
                                                                                     MergeContext& context);
template std::pair<BinaryOperatorView<Ne, Data<FunctionExpression>>, bool> merge_d2d(BinaryOperatorView<Ne, Data<FunctionExpression>> element,
                                                                                     MergeContext& context);
template std::pair<BinaryOperatorView<Ge, Data<FunctionExpression>>, bool> merge_d2d(BinaryOperatorView<Ge, Data<FunctionExpression>> element,
                                                                                     MergeContext& context);
template std::pair<BinaryOperatorView<Gt, Data<FunctionExpression>>, bool> merge_d2d(BinaryOperatorView<Gt, Data<FunctionExpression>> element,
                                                                                     MergeContext& context);
template std::pair<BinaryOperatorView<Le, Data<FunctionExpression>>, bool> merge_d2d(BinaryOperatorView<Le, Data<FunctionExpression>> element,
                                                                                     MergeContext& context);
template std::pair<BinaryOperatorView<Lt, Data<FunctionExpression>>, bool> merge_d2d(BinaryOperatorView<Lt, Data<FunctionExpression>> element,
                                                                                     MergeContext& context);
template std::pair<BinaryOperatorView<Add, Data<FunctionExpression>>, bool> merge_d2d(BinaryOperatorView<Add, Data<FunctionExpression>> element,
                                                                                      MergeContext& context);
template std::pair<BinaryOperatorView<Sub, Data<FunctionExpression>>, bool> merge_d2d(BinaryOperatorView<Sub, Data<FunctionExpression>> element,
                                                                                      MergeContext& context);
template std::pair<BinaryOperatorView<Mul, Data<FunctionExpression>>, bool> merge_d2d(BinaryOperatorView<Mul, Data<FunctionExpression>> element,
                                                                                      MergeContext& context);
template std::pair<BinaryOperatorView<Div, Data<FunctionExpression>>, bool> merge_d2d(BinaryOperatorView<Div, Data<FunctionExpression>> element,
                                                                                      MergeContext& context);
template std::pair<BinaryOperatorView<Eq, Data<GroundFunctionExpression>>, bool> merge_d2d(BinaryOperatorView<Eq, Data<GroundFunctionExpression>> element,
                                                                                           MergeContext& context);
template std::pair<BinaryOperatorView<Ne, Data<GroundFunctionExpression>>, bool> merge_d2d(BinaryOperatorView<Ne, Data<GroundFunctionExpression>> element,
                                                                                           MergeContext& context);
template std::pair<BinaryOperatorView<Ge, Data<GroundFunctionExpression>>, bool> merge_d2d(BinaryOperatorView<Ge, Data<GroundFunctionExpression>> element,
                                                                                           MergeContext& context);
template std::pair<BinaryOperatorView<Gt, Data<GroundFunctionExpression>>, bool> merge_d2d(BinaryOperatorView<Gt, Data<GroundFunctionExpression>> element,
                                                                                           MergeContext& context);
template std::pair<BinaryOperatorView<Le, Data<GroundFunctionExpression>>, bool> merge_d2d(BinaryOperatorView<Le, Data<GroundFunctionExpression>> element,
                                                                                           MergeContext& context);
template std::pair<BinaryOperatorView<Lt, Data<GroundFunctionExpression>>, bool> merge_d2d(BinaryOperatorView<Lt, Data<GroundFunctionExpression>> element,
                                                                                           MergeContext& context);
template std::pair<BinaryOperatorView<Add, Data<GroundFunctionExpression>>, bool> merge_d2d(BinaryOperatorView<Add, Data<GroundFunctionExpression>> element,
                                                                                            MergeContext& context);
template std::pair<BinaryOperatorView<Sub, Data<GroundFunctionExpression>>, bool> merge_d2d(BinaryOperatorView<Sub, Data<GroundFunctionExpression>> element,
                                                                                            MergeContext& context);
template std::pair<BinaryOperatorView<Mul, Data<GroundFunctionExpression>>, bool> merge_d2d(BinaryOperatorView<Mul, Data<GroundFunctionExpression>> element,
                                                                                            MergeContext& context);
template std::pair<BinaryOperatorView<Div, Data<GroundFunctionExpression>>, bool> merge_d2d(BinaryOperatorView<Div, Data<GroundFunctionExpression>> element,
                                                                                            MergeContext& context);

template std::pair<MultiOperatorView<Add, Data<FunctionExpression>>, bool> merge_d2d(MultiOperatorView<Add, Data<FunctionExpression>> element,
                                                                                     MergeContext& context);
template std::pair<MultiOperatorView<Mul, Data<FunctionExpression>>, bool> merge_d2d(MultiOperatorView<Mul, Data<FunctionExpression>> element,
                                                                                     MergeContext& context);
template std::pair<MultiOperatorView<Add, Data<GroundFunctionExpression>>, bool> merge_d2d(MultiOperatorView<Add, Data<GroundFunctionExpression>> element,
                                                                                           MergeContext& context);
template std::pair<MultiOperatorView<Mul, Data<GroundFunctionExpression>>, bool> merge_d2d(MultiOperatorView<Mul, Data<GroundFunctionExpression>> element,
                                                                                           MergeContext& context);

template Data<ArithmeticOperator<Data<FunctionExpression>>> merge_d2d(ArithmeticOperatorView<Data<FunctionExpression>> element, MergeContext& context);
template Data<ArithmeticOperator<Data<GroundFunctionExpression>>> merge_d2d(ArithmeticOperatorView<Data<GroundFunctionExpression>> element,
                                                                            MergeContext& context);

template std::pair<NumericEffectView<Assign, FluentTag>, bool> merge_d2d(NumericEffectView<Assign, FluentTag> element, MergeContext& context);
template std::pair<NumericEffectView<Increase, FluentTag>, bool> merge_d2d(NumericEffectView<Increase, FluentTag> element, MergeContext& context);
template std::pair<NumericEffectView<Decrease, FluentTag>, bool> merge_d2d(NumericEffectView<Decrease, FluentTag> element, MergeContext& context);
template std::pair<NumericEffectView<ScaleUp, FluentTag>, bool> merge_d2d(NumericEffectView<ScaleUp, FluentTag> element, MergeContext& context);
template std::pair<NumericEffectView<ScaleDown, FluentTag>, bool> merge_d2d(NumericEffectView<ScaleDown, FluentTag> element, MergeContext& context);
template Data<NumericEffectOperator<FluentTag>> merge_d2d(NumericEffectOperatorView<FluentTag> element, MergeContext& context);
}

#endif
