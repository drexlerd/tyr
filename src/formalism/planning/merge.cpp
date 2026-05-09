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

#include "tyr/formalism/planning/merge.hpp"

#ifndef TYR_HEADER_INSTANTIATION
#include "tyr/formalism/planning/merge.ipp"

namespace tyr::formalism::planning
{

template std::pair<PredicateBindingView<StaticTag>, bool> merge_p2p(PredicateBindingView<StaticTag> element, MergeContext& context);
template std::pair<PredicateBindingView<FluentTag>, bool> merge_p2p(PredicateBindingView<FluentTag> element, MergeContext& context);
template std::pair<PredicateBindingView<DerivedTag>, bool> merge_p2p(PredicateBindingView<DerivedTag> element, MergeContext& context);

template std::pair<FunctionBindingView<StaticTag>, bool> merge_p2p(FunctionBindingView<StaticTag> element, MergeContext& context);
template std::pair<FunctionBindingView<FluentTag>, bool> merge_p2p(FunctionBindingView<FluentTag> element, MergeContext& context);
template std::pair<FunctionBindingView<AuxiliaryTag>, bool> merge_p2p(FunctionBindingView<AuxiliaryTag> element, MergeContext& context);

template std::pair<PredicateView<StaticTag>, bool> merge_p2p(PredicateView<StaticTag> element, MergeContext& context);
template std::pair<PredicateView<FluentTag>, bool> merge_p2p(PredicateView<FluentTag> element, MergeContext& context);
template std::pair<PredicateView<DerivedTag>, bool> merge_p2p(PredicateView<DerivedTag> element, MergeContext& context);

template std::pair<AtomView<StaticTag>, bool> merge_p2p(AtomView<StaticTag> element, MergeContext& context);
template std::pair<AtomView<FluentTag>, bool> merge_p2p(AtomView<FluentTag> element, MergeContext& context);
template std::pair<AtomView<DerivedTag>, bool> merge_p2p(AtomView<DerivedTag> element, MergeContext& context);

template std::pair<GroundAtomView<StaticTag>, bool> merge_p2p(GroundAtomView<StaticTag> element, MergeContext& context);
template std::pair<GroundAtomView<FluentTag>, bool> merge_p2p(GroundAtomView<FluentTag> element, MergeContext& context);
template std::pair<GroundAtomView<DerivedTag>, bool> merge_p2p(GroundAtomView<DerivedTag> element, MergeContext& context);

template std::pair<LiteralView<StaticTag>, bool> merge_p2p(LiteralView<StaticTag> element, MergeContext& context);
template std::pair<LiteralView<FluentTag>, bool> merge_p2p(LiteralView<FluentTag> element, MergeContext& context);
template std::pair<LiteralView<DerivedTag>, bool> merge_p2p(LiteralView<DerivedTag> element, MergeContext& context);

template std::pair<GroundLiteralView<StaticTag>, bool> merge_p2p(GroundLiteralView<StaticTag> element, MergeContext& context);
template std::pair<GroundLiteralView<FluentTag>, bool> merge_p2p(GroundLiteralView<FluentTag> element, MergeContext& context);
template std::pair<GroundLiteralView<DerivedTag>, bool> merge_p2p(GroundLiteralView<DerivedTag> element, MergeContext& context);

template std::pair<FunctionView<StaticTag>, bool> merge_p2p(FunctionView<StaticTag> element, MergeContext& context);
template std::pair<FunctionView<FluentTag>, bool> merge_p2p(FunctionView<FluentTag> element, MergeContext& context);
template std::pair<FunctionView<AuxiliaryTag>, bool> merge_p2p(FunctionView<AuxiliaryTag> element, MergeContext& context);

template std::pair<FunctionTermView<StaticTag>, bool> merge_p2p(FunctionTermView<StaticTag> element, MergeContext& context);
template std::pair<FunctionTermView<FluentTag>, bool> merge_p2p(FunctionTermView<FluentTag> element, MergeContext& context);
template std::pair<FunctionTermView<AuxiliaryTag>, bool> merge_p2p(FunctionTermView<AuxiliaryTag> element, MergeContext& context);

template std::pair<GroundFunctionTermView<StaticTag>, bool> merge_p2p(GroundFunctionTermView<StaticTag> element, MergeContext& context);
template std::pair<GroundFunctionTermView<FluentTag>, bool> merge_p2p(GroundFunctionTermView<FluentTag> element, MergeContext& context);
template std::pair<GroundFunctionTermView<AuxiliaryTag>, bool> merge_p2p(GroundFunctionTermView<AuxiliaryTag> element, MergeContext& context);

template std::pair<GroundFunctionTermValueView<StaticTag>, bool> merge_p2p(GroundFunctionTermValueView<StaticTag> element, MergeContext& context);
template std::pair<GroundFunctionTermValueView<FluentTag>, bool> merge_p2p(GroundFunctionTermValueView<FluentTag> element, MergeContext& context);
template std::pair<GroundFunctionTermValueView<AuxiliaryTag>, bool> merge_p2p(GroundFunctionTermValueView<AuxiliaryTag> element, MergeContext& context);

template std::pair<UnaryOperatorView<Sub, Data<FunctionExpression>>, bool> merge_p2p(UnaryOperatorView<Sub, Data<FunctionExpression>> element,
                                                                                     MergeContext& context);
template std::pair<UnaryOperatorView<Sub, Data<GroundFunctionExpression>>, bool> merge_p2p(UnaryOperatorView<Sub, Data<GroundFunctionExpression>> element,
                                                                                           MergeContext& context);

template std::pair<BinaryOperatorView<Eq, Data<FunctionExpression>>, bool> merge_p2p(BinaryOperatorView<Eq, Data<FunctionExpression>> element,
                                                                                     MergeContext& context);
template std::pair<BinaryOperatorView<Ne, Data<FunctionExpression>>, bool> merge_p2p(BinaryOperatorView<Ne, Data<FunctionExpression>> element,
                                                                                     MergeContext& context);
template std::pair<BinaryOperatorView<Ge, Data<FunctionExpression>>, bool> merge_p2p(BinaryOperatorView<Ge, Data<FunctionExpression>> element,
                                                                                     MergeContext& context);
template std::pair<BinaryOperatorView<Gt, Data<FunctionExpression>>, bool> merge_p2p(BinaryOperatorView<Gt, Data<FunctionExpression>> element,
                                                                                     MergeContext& context);
template std::pair<BinaryOperatorView<Le, Data<FunctionExpression>>, bool> merge_p2p(BinaryOperatorView<Le, Data<FunctionExpression>> element,
                                                                                     MergeContext& context);
template std::pair<BinaryOperatorView<Lt, Data<FunctionExpression>>, bool> merge_p2p(BinaryOperatorView<Lt, Data<FunctionExpression>> element,
                                                                                     MergeContext& context);
template std::pair<BinaryOperatorView<Add, Data<FunctionExpression>>, bool> merge_p2p(BinaryOperatorView<Add, Data<FunctionExpression>> element,
                                                                                      MergeContext& context);
template std::pair<BinaryOperatorView<Sub, Data<FunctionExpression>>, bool> merge_p2p(BinaryOperatorView<Sub, Data<FunctionExpression>> element,
                                                                                      MergeContext& context);
template std::pair<BinaryOperatorView<Mul, Data<FunctionExpression>>, bool> merge_p2p(BinaryOperatorView<Mul, Data<FunctionExpression>> element,
                                                                                      MergeContext& context);
template std::pair<BinaryOperatorView<Div, Data<FunctionExpression>>, bool> merge_p2p(BinaryOperatorView<Div, Data<FunctionExpression>> element,
                                                                                      MergeContext& context);
template std::pair<BinaryOperatorView<Eq, Data<GroundFunctionExpression>>, bool> merge_p2p(BinaryOperatorView<Eq, Data<GroundFunctionExpression>> element,
                                                                                           MergeContext& context);
template std::pair<BinaryOperatorView<Ne, Data<GroundFunctionExpression>>, bool> merge_p2p(BinaryOperatorView<Ne, Data<GroundFunctionExpression>> element,
                                                                                           MergeContext& context);
template std::pair<BinaryOperatorView<Ge, Data<GroundFunctionExpression>>, bool> merge_p2p(BinaryOperatorView<Ge, Data<GroundFunctionExpression>> element,
                                                                                           MergeContext& context);
template std::pair<BinaryOperatorView<Gt, Data<GroundFunctionExpression>>, bool> merge_p2p(BinaryOperatorView<Gt, Data<GroundFunctionExpression>> element,
                                                                                           MergeContext& context);
template std::pair<BinaryOperatorView<Le, Data<GroundFunctionExpression>>, bool> merge_p2p(BinaryOperatorView<Le, Data<GroundFunctionExpression>> element,
                                                                                           MergeContext& context);
template std::pair<BinaryOperatorView<Lt, Data<GroundFunctionExpression>>, bool> merge_p2p(BinaryOperatorView<Lt, Data<GroundFunctionExpression>> element,
                                                                                           MergeContext& context);
template std::pair<BinaryOperatorView<Add, Data<GroundFunctionExpression>>, bool> merge_p2p(BinaryOperatorView<Add, Data<GroundFunctionExpression>> element,
                                                                                            MergeContext& context);
template std::pair<BinaryOperatorView<Sub, Data<GroundFunctionExpression>>, bool> merge_p2p(BinaryOperatorView<Sub, Data<GroundFunctionExpression>> element,
                                                                                            MergeContext& context);
template std::pair<BinaryOperatorView<Mul, Data<GroundFunctionExpression>>, bool> merge_p2p(BinaryOperatorView<Mul, Data<GroundFunctionExpression>> element,
                                                                                            MergeContext& context);
template std::pair<BinaryOperatorView<Div, Data<GroundFunctionExpression>>, bool> merge_p2p(BinaryOperatorView<Div, Data<GroundFunctionExpression>> element,
                                                                                            MergeContext& context);

template std::pair<MultiOperatorView<Add, Data<FunctionExpression>>, bool> merge_p2p(MultiOperatorView<Add, Data<FunctionExpression>> element,
                                                                                     MergeContext& context);
template std::pair<MultiOperatorView<Mul, Data<FunctionExpression>>, bool> merge_p2p(MultiOperatorView<Mul, Data<FunctionExpression>> element,
                                                                                     MergeContext& context);
template std::pair<MultiOperatorView<Add, Data<GroundFunctionExpression>>, bool> merge_p2p(MultiOperatorView<Add, Data<GroundFunctionExpression>> element,
                                                                                           MergeContext& context);
template std::pair<MultiOperatorView<Mul, Data<GroundFunctionExpression>>, bool> merge_p2p(MultiOperatorView<Mul, Data<GroundFunctionExpression>> element,
                                                                                           MergeContext& context);

template Data<ArithmeticOperator<Data<FunctionExpression>>> merge_p2p(ArithmeticOperatorView<Data<FunctionExpression>> element, MergeContext& context);
template Data<ArithmeticOperator<Data<GroundFunctionExpression>>> merge_p2p(ArithmeticOperatorView<Data<GroundFunctionExpression>> element,
                                                                            MergeContext& context);

template Data<BooleanOperator<Data<FunctionExpression>>> merge_p2p(BooleanOperatorView<Data<FunctionExpression>> element, MergeContext& context);
template Data<BooleanOperator<Data<GroundFunctionExpression>>> merge_p2p(BooleanOperatorView<Data<GroundFunctionExpression>> element, MergeContext& context);

template std::pair<NumericEffectView<Assign, FluentTag>, bool> merge_p2p(NumericEffectView<Assign, FluentTag> element, MergeContext& context);
template std::pair<NumericEffectView<Increase, FluentTag>, bool> merge_p2p(NumericEffectView<Increase, FluentTag> element, MergeContext& context);
template std::pair<NumericEffectView<Decrease, FluentTag>, bool> merge_p2p(NumericEffectView<Decrease, FluentTag> element, MergeContext& context);
template std::pair<NumericEffectView<ScaleUp, FluentTag>, bool> merge_p2p(NumericEffectView<ScaleUp, FluentTag> element, MergeContext& context);
template std::pair<NumericEffectView<ScaleDown, FluentTag>, bool> merge_p2p(NumericEffectView<ScaleDown, FluentTag> element, MergeContext& context);

template std::pair<NumericEffectView<Increase, AuxiliaryTag>, bool> merge_p2p(NumericEffectView<Increase, AuxiliaryTag> element, MergeContext& context);

template Data<NumericEffectOperator<FluentTag>> merge_p2p(NumericEffectOperatorView<FluentTag> element, MergeContext& context);
template Data<NumericEffectOperator<AuxiliaryTag>> merge_p2p(NumericEffectOperatorView<AuxiliaryTag> element, MergeContext& context);

template std::pair<GroundNumericEffectView<Assign, FluentTag>, bool> merge_p2p(GroundNumericEffectView<Assign, FluentTag> element, MergeContext& context);
template std::pair<GroundNumericEffectView<Increase, FluentTag>, bool> merge_p2p(GroundNumericEffectView<Increase, FluentTag> element, MergeContext& context);
template std::pair<GroundNumericEffectView<Decrease, FluentTag>, bool> merge_p2p(GroundNumericEffectView<Decrease, FluentTag> element, MergeContext& context);
template std::pair<GroundNumericEffectView<ScaleUp, FluentTag>, bool> merge_p2p(GroundNumericEffectView<ScaleUp, FluentTag> element, MergeContext& context);
template std::pair<GroundNumericEffectView<ScaleDown, FluentTag>, bool> merge_p2p(GroundNumericEffectView<ScaleDown, FluentTag> element, MergeContext& context);

template std::pair<GroundNumericEffectView<Increase, AuxiliaryTag>, bool> merge_p2p(GroundNumericEffectView<Increase, AuxiliaryTag> element,
                                                                                    MergeContext& context);

template Data<GroundNumericEffectOperator<FluentTag>> merge_p2p(GroundNumericEffectOperatorView<FluentTag> element, MergeContext& context);
template Data<GroundNumericEffectOperator<AuxiliaryTag>> merge_p2p(GroundNumericEffectOperatorView<AuxiliaryTag> element, MergeContext& context);

}

#endif
