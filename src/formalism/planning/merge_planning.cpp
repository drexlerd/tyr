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

#include "tyr/formalism/planning/merge_planning.hpp"

#ifndef TYR_HEADER_INSTANTIATION

namespace tyr::formalism::planning
{

template std::pair<PredicateView<StaticTag>, bool> merge_d2p(formalism::datalog::PredicateView<StaticTag> element, MergePlanningContext& context);
template std::pair<PredicateView<FluentTag>, bool> merge_d2p(formalism::datalog::PredicateView<FluentTag> element, MergePlanningContext& context);
template std::pair<PredicateView<DerivedTag>, bool> merge_d2p(formalism::datalog::PredicateView<FluentTag> element, MergePlanningContext& context);

template std::pair<AtomView<StaticTag>, bool>
merge_d2p(formalism::datalog::AtomView<StaticTag> element,
          const UnorderedMap<formalism::datalog::PredicateView<StaticTag>, PredicateView<StaticTag>>& predicate_mapping,
          MergePlanningContext& context);
template std::pair<AtomView<FluentTag>, bool>
merge_d2p(formalism::datalog::AtomView<FluentTag> element,
          const UnorderedMap<formalism::datalog::PredicateView<FluentTag>, PredicateView<FluentTag>>& predicate_mapping,
          MergePlanningContext& context);
template std::pair<AtomView<DerivedTag>, bool>
merge_d2p(formalism::datalog::AtomView<FluentTag> element,
          const UnorderedMap<formalism::datalog::PredicateView<FluentTag>, PredicateView<DerivedTag>>& predicate_mapping,
          MergePlanningContext& context);

template std::pair<PredicateBindingView<StaticTag>, bool>
merge_d2p(formalism::datalog::PredicateBindingView<StaticTag> element,
          const UnorderedMap<formalism::datalog::PredicateView<StaticTag>, PredicateView<StaticTag>>& predicate_mapping,
          MergePlanningContext& context);
template std::pair<PredicateBindingView<FluentTag>, bool>
merge_d2p(formalism::datalog::PredicateBindingView<FluentTag> element,
          const UnorderedMap<formalism::datalog::PredicateView<FluentTag>, PredicateView<FluentTag>>& predicate_mapping,
          MergePlanningContext& context);
template std::pair<PredicateBindingView<DerivedTag>, bool>
merge_d2p(formalism::datalog::PredicateBindingView<FluentTag> element,
          const UnorderedMap<formalism::datalog::PredicateView<FluentTag>, PredicateView<DerivedTag>>& predicate_mapping,
          MergePlanningContext& context);

template std::pair<GroundAtomView<StaticTag>, bool>
merge_atom_d2p<StaticTag, StaticTag>(formalism::datalog::PredicateBindingView<StaticTag> element,
                                     const UnorderedMap<formalism::datalog::PredicateView<StaticTag>, PredicateView<StaticTag>>& predicate_mapping,
                                     MergePlanningContext& context);
template std::pair<GroundAtomView<FluentTag>, bool>
merge_atom_d2p<FluentTag, FluentTag>(formalism::datalog::PredicateBindingView<FluentTag> element,
                                     const UnorderedMap<formalism::datalog::PredicateView<FluentTag>, PredicateView<FluentTag>>& predicate_mapping,
                                     MergePlanningContext& context);
template std::pair<GroundAtomView<DerivedTag>, bool>
merge_atom_d2p<FluentTag, DerivedTag>(formalism::datalog::PredicateBindingView<FluentTag> element,
                                      const UnorderedMap<formalism::datalog::PredicateView<FluentTag>, PredicateView<DerivedTag>>& predicate_mapping,
                                      MergePlanningContext& context);

template std::pair<GroundAtomView<StaticTag>, bool>
merge_d2p(formalism::datalog::GroundAtomView<StaticTag> element,
          const UnorderedMap<formalism::datalog::PredicateView<StaticTag>, PredicateView<StaticTag>>& predicate_mapping,
          MergePlanningContext& context);
template std::pair<GroundAtomView<FluentTag>, bool>
merge_d2p(formalism::datalog::GroundAtomView<FluentTag> element,
          const UnorderedMap<formalism::datalog::PredicateView<FluentTag>, PredicateView<FluentTag>>& predicate_mapping,
          MergePlanningContext& context);
template std::pair<GroundAtomView<DerivedTag>, bool>
merge_d2p(formalism::datalog::GroundAtomView<FluentTag> element,
          const UnorderedMap<formalism::datalog::PredicateView<FluentTag>, PredicateView<DerivedTag>>& predicate_mapping,
          MergePlanningContext& context);

template std::pair<LiteralView<StaticTag>, bool>
merge_d2p(formalism::datalog::LiteralView<StaticTag> element,
          const UnorderedMap<formalism::datalog::PredicateView<StaticTag>, PredicateView<StaticTag>>& predicate_mapping,
          MergePlanningContext& context);
template std::pair<LiteralView<FluentTag>, bool>
merge_d2p(formalism::datalog::LiteralView<FluentTag> element,
          const UnorderedMap<formalism::datalog::PredicateView<FluentTag>, PredicateView<FluentTag>>& predicate_mapping,
          MergePlanningContext& context);
template std::pair<LiteralView<DerivedTag>, bool>
merge_d2p(formalism::datalog::LiteralView<FluentTag> element,
          const UnorderedMap<formalism::datalog::PredicateView<FluentTag>, PredicateView<DerivedTag>>& predicate_mapping,
          MergePlanningContext& context);

template std::pair<GroundLiteralView<StaticTag>, bool>
merge_d2p(formalism::datalog::GroundLiteralView<StaticTag> element,
          const UnorderedMap<formalism::datalog::PredicateView<StaticTag>, PredicateView<StaticTag>>& predicate_mapping,
          MergePlanningContext& context);
template std::pair<GroundLiteralView<FluentTag>, bool>
merge_d2p(formalism::datalog::GroundLiteralView<FluentTag> element,
          const UnorderedMap<formalism::datalog::PredicateView<FluentTag>, PredicateView<FluentTag>>& predicate_mapping,
          MergePlanningContext& context);
template std::pair<GroundLiteralView<DerivedTag>, bool>
merge_d2p(formalism::datalog::GroundLiteralView<FluentTag> element,
          const UnorderedMap<formalism::datalog::PredicateView<FluentTag>, PredicateView<DerivedTag>>& predicate_mapping,
          MergePlanningContext& context);

// Numeric

template std::pair<FunctionView<StaticTag>, bool> merge_d2p(formalism::datalog::FunctionView<StaticTag> element, MergePlanningContext& context);
template std::pair<FunctionView<FluentTag>, bool> merge_d2p(formalism::datalog::FunctionView<FluentTag> element, MergePlanningContext& context);

template std::pair<FunctionTermView<StaticTag>, bool> merge_d2p(formalism::datalog::FunctionTermView<StaticTag> element, MergePlanningContext& context);
template std::pair<FunctionTermView<FluentTag>, bool> merge_d2p(formalism::datalog::FunctionTermView<FluentTag> element, MergePlanningContext& context);

template std::pair<FunctionBindingView<StaticTag>, bool> merge_d2p(formalism::datalog::FunctionBindingView<StaticTag> element, MergePlanningContext& context);
template std::pair<FunctionBindingView<FluentTag>, bool> merge_d2p(formalism::datalog::FunctionBindingView<FluentTag> element, MergePlanningContext& context);

template std::pair<GroundFunctionTermView<StaticTag>, bool> merge_d2p(formalism::datalog::GroundFunctionTermView<StaticTag> element,
                                                                      MergePlanningContext& context);
template std::pair<GroundFunctionTermView<FluentTag>, bool> merge_d2p(formalism::datalog::GroundFunctionTermView<FluentTag> element,
                                                                      MergePlanningContext& context);

template std::pair<GroundFunctionTermValueView<StaticTag>, bool> merge_d2p(formalism::datalog::GroundFunctionTermValueView<StaticTag> element,
                                                                           MergePlanningContext& context);
template std::pair<GroundFunctionTermValueView<FluentTag>, bool> merge_d2p(formalism::datalog::GroundFunctionTermValueView<FluentTag> element,
                                                                           MergePlanningContext& context);

template std::pair<UnaryOperatorView<Sub, Data<FunctionExpression>>, bool>
merge_d2p(formalism::datalog::UnaryOperatorView<Sub, Data<formalism::datalog::FunctionExpression>> element, MergePlanningContext& context);
template std::pair<UnaryOperatorView<Sub, Data<GroundFunctionExpression>>, bool>
merge_d2p(formalism::datalog::UnaryOperatorView<Sub, Data<formalism::datalog::GroundFunctionExpression>> element, MergePlanningContext& context);

template std::pair<BinaryOperatorView<Eq, Data<FunctionExpression>>, bool>
merge_d2p(formalism::datalog::BinaryOperatorView<Eq, Data<formalism::datalog::FunctionExpression>> element, MergePlanningContext& context);
template std::pair<BinaryOperatorView<Ne, Data<FunctionExpression>>, bool>
merge_d2p(formalism::datalog::BinaryOperatorView<Ne, Data<formalism::datalog::FunctionExpression>> element, MergePlanningContext& context);
template std::pair<BinaryOperatorView<Ge, Data<FunctionExpression>>, bool>
merge_d2p(formalism::datalog::BinaryOperatorView<Ge, Data<formalism::datalog::FunctionExpression>> element, MergePlanningContext& context);
template std::pair<BinaryOperatorView<Gt, Data<FunctionExpression>>, bool>
merge_d2p(formalism::datalog::BinaryOperatorView<Gt, Data<formalism::datalog::FunctionExpression>> element, MergePlanningContext& context);
template std::pair<BinaryOperatorView<Le, Data<FunctionExpression>>, bool>
merge_d2p(formalism::datalog::BinaryOperatorView<Le, Data<formalism::datalog::FunctionExpression>> element, MergePlanningContext& context);
template std::pair<BinaryOperatorView<Lt, Data<FunctionExpression>>, bool>
merge_d2p(formalism::datalog::BinaryOperatorView<Lt, Data<formalism::datalog::FunctionExpression>> element, MergePlanningContext& context);
template std::pair<BinaryOperatorView<Add, Data<FunctionExpression>>, bool>
merge_d2p(formalism::datalog::BinaryOperatorView<Add, Data<formalism::datalog::FunctionExpression>> element, MergePlanningContext& context);
template std::pair<BinaryOperatorView<Sub, Data<FunctionExpression>>, bool>
merge_d2p(formalism::datalog::BinaryOperatorView<Sub, Data<formalism::datalog::FunctionExpression>> element, MergePlanningContext& context);
template std::pair<BinaryOperatorView<Mul, Data<FunctionExpression>>, bool>
merge_d2p(formalism::datalog::BinaryOperatorView<Mul, Data<formalism::datalog::FunctionExpression>> element, MergePlanningContext& context);
template std::pair<BinaryOperatorView<Div, Data<FunctionExpression>>, bool>
merge_d2p(formalism::datalog::BinaryOperatorView<Div, Data<formalism::datalog::FunctionExpression>> element, MergePlanningContext& context);
template std::pair<BinaryOperatorView<Eq, Data<GroundFunctionExpression>>, bool>
merge_d2p(formalism::datalog::BinaryOperatorView<Eq, Data<formalism::datalog::GroundFunctionExpression>> element, MergePlanningContext& context);
template std::pair<BinaryOperatorView<Ne, Data<GroundFunctionExpression>>, bool>
merge_d2p(formalism::datalog::BinaryOperatorView<Ne, Data<formalism::datalog::GroundFunctionExpression>> element, MergePlanningContext& context);
template std::pair<BinaryOperatorView<Ge, Data<GroundFunctionExpression>>, bool>
merge_d2p(formalism::datalog::BinaryOperatorView<Ge, Data<formalism::datalog::GroundFunctionExpression>> element, MergePlanningContext& context);
template std::pair<BinaryOperatorView<Gt, Data<GroundFunctionExpression>>, bool>
merge_d2p(formalism::datalog::BinaryOperatorView<Gt, Data<formalism::datalog::GroundFunctionExpression>> element, MergePlanningContext& context);
template std::pair<BinaryOperatorView<Le, Data<GroundFunctionExpression>>, bool>
merge_d2p(formalism::datalog::BinaryOperatorView<Le, Data<formalism::datalog::GroundFunctionExpression>> element, MergePlanningContext& context);
template std::pair<BinaryOperatorView<Lt, Data<GroundFunctionExpression>>, bool>
merge_d2p(formalism::datalog::BinaryOperatorView<Lt, Data<formalism::datalog::GroundFunctionExpression>> element, MergePlanningContext& context);
template std::pair<BinaryOperatorView<Add, Data<GroundFunctionExpression>>, bool>
merge_d2p(formalism::datalog::BinaryOperatorView<Add, Data<formalism::datalog::GroundFunctionExpression>> element, MergePlanningContext& context);
template std::pair<BinaryOperatorView<Sub, Data<GroundFunctionExpression>>, bool>
merge_d2p(formalism::datalog::BinaryOperatorView<Sub, Data<formalism::datalog::GroundFunctionExpression>> element, MergePlanningContext& context);
template std::pair<BinaryOperatorView<Mul, Data<GroundFunctionExpression>>, bool>
merge_d2p(formalism::datalog::BinaryOperatorView<Mul, Data<formalism::datalog::GroundFunctionExpression>> element, MergePlanningContext& context);
template std::pair<BinaryOperatorView<Div, Data<GroundFunctionExpression>>, bool>
merge_d2p(formalism::datalog::BinaryOperatorView<Div, Data<formalism::datalog::GroundFunctionExpression>> element, MergePlanningContext& context);

template std::pair<MultiOperatorView<Add, Data<FunctionExpression>>, bool>
merge_d2p(formalism::datalog::MultiOperatorView<Add, Data<formalism::datalog::FunctionExpression>> element, MergePlanningContext& context);
template std::pair<MultiOperatorView<Mul, Data<FunctionExpression>>, bool>
merge_d2p(formalism::datalog::MultiOperatorView<Mul, Data<formalism::datalog::FunctionExpression>> element, MergePlanningContext& context);
template std::pair<MultiOperatorView<Add, Data<GroundFunctionExpression>>, bool>
merge_d2p(formalism::datalog::MultiOperatorView<Add, Data<formalism::datalog::GroundFunctionExpression>> element, MergePlanningContext& context);
template std::pair<MultiOperatorView<Mul, Data<GroundFunctionExpression>>, bool>
merge_d2p(formalism::datalog::MultiOperatorView<Mul, Data<formalism::datalog::GroundFunctionExpression>> element, MergePlanningContext& context);

template Data<ArithmeticOperator<Data<FunctionExpression>>>
merge_d2p(formalism::datalog::ArithmeticOperatorView<Data<formalism::datalog::FunctionExpression>> element, MergePlanningContext& context);
template Data<ArithmeticOperator<Data<GroundFunctionExpression>>>
merge_d2p(formalism::datalog::ArithmeticOperatorView<Data<formalism::datalog::GroundFunctionExpression>> element, MergePlanningContext& context);

template Data<BooleanOperator<Data<FunctionExpression>>>
merge_d2p(formalism::datalog::BooleanOperatorView<Data<formalism::datalog::FunctionExpression>> element, MergePlanningContext& context);
template Data<BooleanOperator<Data<GroundFunctionExpression>>>
merge_d2p(formalism::datalog::BooleanOperatorView<Data<formalism::datalog::GroundFunctionExpression>> element, MergePlanningContext& context);

}

#endif
