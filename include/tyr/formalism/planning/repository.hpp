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

#ifndef TYR_FORMALISM_PLANNING_REPOSITORY_HPP_
#define TYR_FORMALISM_PLANNING_REPOSITORY_HPP_

#include "tyr/buffer/declarations.hpp"
#include "tyr/buffer/indexed_hash_set.hpp"
#include "tyr/buffer/segmented_buffer.hpp"
#include "tyr/common/tuple.hpp"
#include "tyr/formalism/function_view.hpp"
#include "tyr/formalism/planning/canonicalization.hpp"
#include "tyr/formalism/planning/datas.hpp"
#include "tyr/formalism/planning/declarations.hpp"
#include "tyr/formalism/planning/indices.hpp"
#include "tyr/formalism/planning/views.hpp"
#include "tyr/formalism/predicate_view.hpp"
#include "tyr/formalism/relation_repository.hpp"
#include "tyr/formalism/repository.hpp"
#include "tyr/formalism/repository_factory.hpp"
#include "tyr/formalism/symbol_repository.hpp"

#include <cassert>
#include <optional>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

namespace tyr::formalism::planning
{

using SymbolRepository = tyr::ApplyTypeListT<tyr::formalism::SymbolRepository, SymbolRepositoryTypes>;

using RelationRepository = tyr::ApplyTypeListT<tyr::formalism::RelationRepository, RelationRepositoryTypes>;

using Repository = tyr::formalism::Repository<SymbolRepository, RelationRepository>;
using RepositoryPtr = std::shared_ptr<Repository>;

using RepositoryFactory = tyr::formalism::RepositoryFactory<SymbolRepository, RelationRepository>;
using RepositoryFactoryPtr = std::shared_ptr<RepositoryFactory>;

using ActionView = View<Index<Action>, Repository>;
using ActionListView = View<IndexList<Action>, Repository>;
using ActionViewList = std::vector<ActionView>;

template<typename T>
using ArithmeticOperatorView = View<Data<ArithmeticOperator<T>>, Repository>;
using LiftedArithmeticOperatorView = View<Data<ArithmeticOperator<Data<FunctionExpression>>>, Repository>;
using GroundArithmeticOperatorView = View<Data<ArithmeticOperator<Data<GroundFunctionExpression>>>, Repository>;

template<typename T>
using ArithmeticOperatorListView = View<DataList<ArithmeticOperator<T>>, Repository>;
using LiftedArithmeticOperatorListView = View<DataList<ArithmeticOperator<Data<FunctionExpression>>>, Repository>;
using GroundArithmeticOperatorListView = View<DataList<ArithmeticOperator<Data<GroundFunctionExpression>>>, Repository>;

template<FactKind T>
using AtomView = View<Index<Atom<T>>, Repository>;
template<FactKind T>
using AtomListView = View<IndexList<Atom<T>>, Repository>;
template<FactKind T>
using AtomViewList = std::vector<AtomView<T>>;

using AxiomView = View<Index<Axiom>, Repository>;
using AxiomListView = View<IndexList<Axiom>, Repository>;
using AxiomViewList = std::vector<AxiomView>;

template<OpKind Op, typename T>
using BinaryOperatorView = View<Index<BinaryOperator<Op, T>>, Repository>;
template<OpKind Op>
using LiftedBinaryOperatorView = View<Index<BinaryOperator<Op, Data<FunctionExpression>>>, Repository>;
template<OpKind Op>
using GroundBinaryOperatorView = View<Index<BinaryOperator<Op, Data<GroundFunctionExpression>>>, Repository>;

template<OpKind Op, typename T>
using BinaryOperatorListView = View<IndexList<BinaryOperator<Op, T>>, Repository>;
template<OpKind Op>
using LiftedBinaryOperatorListView = View<IndexList<BinaryOperator<Op, Data<FunctionExpression>>>, Repository>;
template<OpKind Op>
using GroundBinaryOperatorListView = View<IndexList<BinaryOperator<Op, Data<GroundFunctionExpression>>>, Repository>;

template<FactKind T>
using PredicateBindingView = View<Index<RelationBinding<Predicate<T>>>, Repository>;
template<FactKind T>
using FunctionBindingView = View<Index<RelationBinding<Function<T>>>, Repository>;
using ActionBindingView = View<Index<RelationBinding<Action>>, Repository>;
using AxiomBindingView = View<Index<RelationBinding<Axiom>>, Repository>;

template<typename T>
using BooleanOperatorView = View<Data<BooleanOperator<T>>, Repository>;
using LiftedBooleanOperatorView = View<Data<BooleanOperator<Data<FunctionExpression>>>, Repository>;
using GroundBooleanOperatorView = View<Data<BooleanOperator<Data<GroundFunctionExpression>>>, Repository>;

template<typename T>
using BooleanOperatorListView = View<DataList<BooleanOperator<T>>, Repository>;
using LiftedBooleanOperatorListView = View<DataList<BooleanOperator<Data<FunctionExpression>>>, Repository>;
using GroundBooleanOperatorListView = View<DataList<BooleanOperator<Data<GroundFunctionExpression>>>, Repository>;
using LiftedBooleanOperatorViewList = std::vector<View<Data<BooleanOperator<Data<FunctionExpression>>>, Repository>>;
using GroundBooleanOperatorViewList = std::vector<View<Data<BooleanOperator<Data<GroundFunctionExpression>>>, Repository>>;

using ConditionalEffectView = View<Index<ConditionalEffect>, Repository>;
using ConditionalEffectListView = View<IndexList<ConditionalEffect>, Repository>;
using ConditionalEffectViewList = std::vector<View<Index<ConditionalEffect>, Repository>>;

using ConjunctiveConditionView = View<Index<ConjunctiveCondition>, Repository>;
using ConjunctiveConditionListView = View<IndexList<ConjunctiveCondition>, Repository>;

using ConjunctiveEffectView = View<Index<ConjunctiveEffect>, Repository>;
using ConjunctiveEffectListView = View<IndexList<ConjunctiveEffect>, Repository>;

using DomainView = View<Index<Domain>, Repository>;
using DomainListView = View<IndexList<Domain>, Repository>;

template<FactKind T>
using FDRFactView = View<Data<FDRFact<T>>, Repository>;
template<FactKind T>
using FDRFactListView = View<DataList<FDRFact<T>>, Repository>;
template<FactKind T>
using FDRFactViewList = std::vector<FDRFactView<T>>;

using FDRTaskView = View<Index<FDRTask>, Repository>;
using FDRTaskListView = View<IndexList<FDRTask>, Repository>;

template<FactKind T>
using FDRVariableView = View<Index<FDRVariable<T>>, Repository>;
template<FactKind T>
using FDRVariableListView = View<IndexList<FDRVariable<T>>, Repository>;
template<FactKind T>
using FDRVariableViewList = std::vector<FDRVariableView<T>>;

using FunctionExpressionView = View<Data<FunctionExpression>, Repository>;
using FunctionExpressionListView = View<DataList<FunctionExpression>, Repository>;

template<FactKind T>
using FunctionTermView = View<Index<FunctionTerm<T>>, Repository>;
template<FactKind T>
using FunctionTermListView = View<IndexList<FunctionTerm<T>>, Repository>;

template<FactKind T>
using FunctionView = View<Index<Function<T>>, Repository>;
template<FactKind T>
using FunctionListView = View<IndexList<Function<T>>, Repository>;
template<FactKind T>
using FunctionViewList = std::vector<FunctionView<T>>;

using GroundActionView = View<Index<GroundAction>, Repository>;
using GroundActionListView = View<IndexList<GroundAction>, Repository>;
using GroundActionViewList = std::vector<GroundActionView>;

template<FactKind T>
using GroundAtomView = View<Index<GroundAtom<T>>, Repository>;
template<FactKind T>
using GroundAtomListView = View<IndexList<GroundAtom<T>>, Repository>;
template<FactKind T>
using GroundAtomViewList = std::vector<GroundAtomView<T>>;

using GroundAxiomView = View<Index<GroundAxiom>, Repository>;
using GroundAxiomListView = View<IndexList<GroundAxiom>, Repository>;
using GroundAxiomViewList = std::vector<GroundAxiomView>;

using GroundConditionalEffectView = View<Index<GroundConditionalEffect>, Repository>;
using GroundConditionalEffectListView = View<IndexList<GroundConditionalEffect>, Repository>;
using GroundConditionalEffectViewList = std::vector<GroundConditionalEffectView>;

using GroundConjunctiveConditionView = View<Index<GroundConjunctiveCondition>, Repository>;
using GroundConjunctiveConditionListView = View<IndexList<GroundConjunctiveCondition>, Repository>;

using GroundConjunctiveEffectView = View<Index<GroundConjunctiveEffect>, Repository>;
using GroundConjunctiveEffectListView = View<IndexList<GroundConjunctiveEffect>, Repository>;

using GroundFunctionExpressionView = View<Data<GroundFunctionExpression>, Repository>;
using GroundFunctionExpressionListView = View<DataList<GroundFunctionExpression>, Repository>;

template<FactKind T>
using GroundFunctionTermValueView = View<Index<GroundFunctionTermValue<T>>, Repository>;
template<FactKind T>
using GroundFunctionTermValueListView = View<IndexList<GroundFunctionTermValue<T>>, Repository>;
template<FactKind T>
using GroundFunctionTermValueViewList = std::vector<GroundFunctionTermValueView<T>>;

template<FactKind T>
using GroundFunctionTermViewValuePair = std::pair<View<Index<GroundFunctionTerm<T>>, Repository>, float_t>;
template<FactKind T>
using GroundFunctionTermViewValuePairList = std::vector<GroundFunctionTermViewValuePair<T>>;

template<FactKind T>
using GroundFunctionTermView = View<Index<GroundFunctionTerm<T>>, Repository>;
template<FactKind T>
using GroundFunctionTermListView = View<IndexList<GroundFunctionTerm<T>>, Repository>;
template<FactKind T>
using GroundFunctionTermViewList = std::vector<GroundFunctionTermView<T>>;

template<FactKind T>
using GroundLiteralView = View<Index<GroundLiteral<T>>, Repository>;
template<FactKind T>
using GroundLiteralListView = View<IndexList<GroundLiteral<T>>, Repository>;
template<FactKind T>
using GroundLiteralViewList = std::vector<GroundLiteralView<T>>;

template<FactKind T>
using GroundNumericEffectOperatorView = View<Data<GroundNumericEffectOperator<T>>, Repository>;
template<FactKind T>
using GroundNumericEffectOperatorListView = View<DataList<GroundNumericEffectOperator<T>>, Repository>;
template<FactKind T>
using GroundNumericEffectOperatorViewList = std::vector<GroundNumericEffectOperatorView<T>>;

template<NumericEffectOpKind Op, FactKind T>
using GroundNumericEffectView = View<Index<GroundNumericEffect<Op, T>>, Repository>;
template<NumericEffectOpKind Op, FactKind T>
using GroundNumericEffectListView = View<IndexList<GroundNumericEffect<Op, T>>, Repository>;

template<FactKind T>
using LiteralView = View<Index<Literal<T>>, Repository>;
template<FactKind T>
using LiteralListView = View<IndexList<Literal<T>>, Repository>;
template<FactKind T>
using LiteralViewList = std::vector<LiteralView<T>>;

using MetricView = View<Index<Metric>, Repository>;
using MetricListView = View<IndexList<Metric>, Repository>;

template<OpKind Op, typename T>
using MultiOperatorView = View<Index<MultiOperator<Op, T>>, Repository>;
template<OpKind Op>
using LiftedMultiOperatorView = View<Index<MultiOperator<Op, Data<FunctionExpression>>>, Repository>;
template<OpKind Op>
using GroundMultiOperatorView = View<Index<MultiOperator<Op, Data<GroundFunctionExpression>>>, Repository>;

template<OpKind Op, typename T>
using MultiOperatorListView = View<IndexList<MultiOperator<Op, T>>, Repository>;
template<OpKind Op>
using LiftedMultiOperatorListView = View<IndexList<MultiOperator<Op, Data<FunctionExpression>>>, Repository>;
template<OpKind Op>
using GroundMultiOperatorListView = View<IndexList<MultiOperator<Op, Data<GroundFunctionExpression>>>, Repository>;

template<FactKind T>
using NumericEffectOperatorView = View<Data<NumericEffectOperator<T>>, Repository>;
template<FactKind T>
using NumericEffectOperatorListView = View<DataList<NumericEffectOperator<T>>, Repository>;
template<FactKind T>
using NumericEffectOperatorViewList = std::vector<NumericEffectOperatorView<T>>;

template<NumericEffectOpKind Op, FactKind T>
using NumericEffectView = View<Index<NumericEffect<Op, T>>, Repository>;
template<NumericEffectOpKind Op, FactKind T>
using NumericEffectListView = View<IndexList<NumericEffect<Op, T>>, Repository>;

using ObjectView = View<Index<Object>, Repository>;
using ObjectListView = View<IndexList<Object>, Repository>;
using ObjectViewList = std::vector<ObjectView>;

template<FactKind T>
using PredicateView = View<Index<Predicate<T>>, Repository>;
template<FactKind T>
using PredicateListView = View<IndexList<Predicate<T>>, Repository>;
template<FactKind T>
using PredicateViewList = std::vector<PredicateView<T>>;

using TaskView = View<Index<Task>, Repository>;
using TaskListView = View<IndexList<Task>, Repository>;

using TermView = View<Data<Term>, Repository>;
using TermListView = View<DataList<Term>, Repository>;
using TermViewList = std::vector<TermView>;

template<OpKind Op, typename T>
using UnaryOperatorView = View<Index<UnaryOperator<Op, T>>, Repository>;
template<OpKind Op>
using LiftedUnaryOperatorView = View<Index<UnaryOperator<Op, Data<FunctionExpression>>>, Repository>;
template<OpKind Op>
using GroundUnaryOperatorView = View<Index<UnaryOperator<Op, Data<GroundFunctionExpression>>>, Repository>;

template<OpKind Op, typename T>
using UnaryOperatorListView = View<IndexList<UnaryOperator<Op, T>>, Repository>;
template<OpKind Op>
using LiftedUnaryOperatorListView = View<IndexList<UnaryOperator<Op, Data<FunctionExpression>>>, Repository>;
template<OpKind Op>
using GroundUnaryOperatorListView = View<IndexList<UnaryOperator<Op, Data<GroundFunctionExpression>>>, Repository>;

using VariableView = View<Index<Variable>, Repository>;
using VariableListView = View<IndexList<Variable>, Repository>;
using VariableViewList = std::vector<VariableView>;

}

#ifndef TYR_HEADER_INSTANTIATION

namespace tyr::formalism
{
// BasicRelationRepository
extern template class BasicRelationRepository<Predicate<StaticTag>>;
extern template class BasicRelationRepository<Predicate<FluentTag>>;
extern template class BasicRelationRepository<Predicate<DerivedTag>>;
extern template class BasicRelationRepository<Function<StaticTag>>;
extern template class BasicRelationRepository<Function<FluentTag>>;
extern template class BasicRelationRepository<Function<AuxiliaryTag>>;
extern template class BasicRelationRepository<planning::Action>;
extern template class BasicRelationRepository<planning::Axiom>;

// BasicSymbolRepository
extern template class BasicSymbolRepository<Variable>;
extern template class BasicSymbolRepository<Object>;
extern template class BasicSymbolRepository<Predicate<StaticTag>>;
extern template class BasicSymbolRepository<Predicate<FluentTag>>;
extern template class BasicSymbolRepository<Predicate<DerivedTag>>;
extern template class BasicSymbolRepository<planning::Atom<StaticTag>>;
extern template class BasicSymbolRepository<planning::Atom<FluentTag>>;
extern template class BasicSymbolRepository<planning::Atom<DerivedTag>>;
extern template class BasicSymbolRepository<planning::GroundAtom<StaticTag>>;
extern template class BasicSymbolRepository<planning::GroundAtom<FluentTag>>;
extern template class BasicSymbolRepository<planning::GroundAtom<DerivedTag>>;
extern template class BasicSymbolRepository<planning::Literal<StaticTag>>;
extern template class BasicSymbolRepository<planning::Literal<FluentTag>>;
extern template class BasicSymbolRepository<planning::Literal<DerivedTag>>;
extern template class BasicSymbolRepository<planning::GroundLiteral<StaticTag>>;
extern template class BasicSymbolRepository<planning::GroundLiteral<FluentTag>>;
extern template class BasicSymbolRepository<planning::GroundLiteral<DerivedTag>>;
extern template class BasicSymbolRepository<Function<StaticTag>>;
extern template class BasicSymbolRepository<Function<FluentTag>>;
extern template class BasicSymbolRepository<Function<AuxiliaryTag>>;
extern template class BasicSymbolRepository<planning::FunctionTerm<StaticTag>>;
extern template class BasicSymbolRepository<planning::FunctionTerm<FluentTag>>;
extern template class BasicSymbolRepository<planning::FunctionTerm<AuxiliaryTag>>;
extern template class BasicSymbolRepository<planning::GroundFunctionTerm<StaticTag>>;
extern template class BasicSymbolRepository<planning::GroundFunctionTerm<FluentTag>>;
extern template class BasicSymbolRepository<planning::GroundFunctionTerm<AuxiliaryTag>>;
extern template class BasicSymbolRepository<planning::GroundFunctionTermValue<StaticTag>>;
extern template class BasicSymbolRepository<planning::GroundFunctionTermValue<FluentTag>>;
extern template class BasicSymbolRepository<planning::GroundFunctionTermValue<AuxiliaryTag>>;
extern template class BasicSymbolRepository<planning::UnaryOperator<Sub, Data<planning::FunctionExpression>>>;
extern template class BasicSymbolRepository<planning::BinaryOperator<Add, Data<planning::FunctionExpression>>>;
extern template class BasicSymbolRepository<planning::BinaryOperator<Sub, Data<planning::FunctionExpression>>>;
extern template class BasicSymbolRepository<planning::BinaryOperator<Mul, Data<planning::FunctionExpression>>>;
extern template class BasicSymbolRepository<planning::BinaryOperator<Div, Data<planning::FunctionExpression>>>;
extern template class BasicSymbolRepository<planning::MultiOperator<Add, Data<planning::FunctionExpression>>>;
extern template class BasicSymbolRepository<planning::MultiOperator<Mul, Data<planning::FunctionExpression>>>;
extern template class BasicSymbolRepository<planning::BinaryOperator<Eq, Data<planning::FunctionExpression>>>;
extern template class BasicSymbolRepository<planning::BinaryOperator<Ne, Data<planning::FunctionExpression>>>;
extern template class BasicSymbolRepository<planning::BinaryOperator<Le, Data<planning::FunctionExpression>>>;
extern template class BasicSymbolRepository<planning::BinaryOperator<Lt, Data<planning::FunctionExpression>>>;
extern template class BasicSymbolRepository<planning::BinaryOperator<Ge, Data<planning::FunctionExpression>>>;
extern template class BasicSymbolRepository<planning::BinaryOperator<Gt, Data<planning::FunctionExpression>>>;
extern template class BasicSymbolRepository<planning::UnaryOperator<Sub, Data<planning::GroundFunctionExpression>>>;
extern template class BasicSymbolRepository<planning::BinaryOperator<Add, Data<planning::GroundFunctionExpression>>>;
extern template class BasicSymbolRepository<planning::BinaryOperator<Sub, Data<planning::GroundFunctionExpression>>>;
extern template class BasicSymbolRepository<planning::BinaryOperator<Mul, Data<planning::GroundFunctionExpression>>>;
extern template class BasicSymbolRepository<planning::BinaryOperator<Div, Data<planning::GroundFunctionExpression>>>;
extern template class BasicSymbolRepository<planning::MultiOperator<Add, Data<planning::GroundFunctionExpression>>>;
extern template class BasicSymbolRepository<planning::MultiOperator<Mul, Data<planning::GroundFunctionExpression>>>;
extern template class BasicSymbolRepository<planning::BinaryOperator<Eq, Data<planning::GroundFunctionExpression>>>;
extern template class BasicSymbolRepository<planning::BinaryOperator<Ne, Data<planning::GroundFunctionExpression>>>;
extern template class BasicSymbolRepository<planning::BinaryOperator<Le, Data<planning::GroundFunctionExpression>>>;
extern template class BasicSymbolRepository<planning::BinaryOperator<Lt, Data<planning::GroundFunctionExpression>>>;
extern template class BasicSymbolRepository<planning::BinaryOperator<Ge, Data<planning::GroundFunctionExpression>>>;
extern template class BasicSymbolRepository<planning::BinaryOperator<Gt, Data<planning::GroundFunctionExpression>>>;
extern template class BasicSymbolRepository<planning::NumericEffect<Assign, FluentTag>>;
extern template class BasicSymbolRepository<planning::NumericEffect<Increase, FluentTag>>;
extern template class BasicSymbolRepository<planning::NumericEffect<Decrease, FluentTag>>;
extern template class BasicSymbolRepository<planning::NumericEffect<ScaleUp, FluentTag>>;
extern template class BasicSymbolRepository<planning::NumericEffect<ScaleDown, FluentTag>>;
extern template class BasicSymbolRepository<planning::NumericEffect<Increase, AuxiliaryTag>>;
extern template class BasicSymbolRepository<planning::GroundNumericEffect<Assign, FluentTag>>;
extern template class BasicSymbolRepository<planning::GroundNumericEffect<Increase, FluentTag>>;
extern template class BasicSymbolRepository<planning::GroundNumericEffect<Decrease, FluentTag>>;
extern template class BasicSymbolRepository<planning::GroundNumericEffect<ScaleUp, FluentTag>>;
extern template class BasicSymbolRepository<planning::GroundNumericEffect<ScaleDown, FluentTag>>;
extern template class BasicSymbolRepository<planning::GroundNumericEffect<Increase, AuxiliaryTag>>;
extern template class BasicSymbolRepository<planning::ConditionalEffect>;
extern template class BasicSymbolRepository<planning::GroundConditionalEffect>;
extern template class BasicSymbolRepository<planning::ConjunctiveEffect>;
extern template class BasicSymbolRepository<planning::GroundConjunctiveEffect>;
extern template class BasicSymbolRepository<planning::Action>;
extern template class BasicSymbolRepository<planning::GroundAction>;
extern template class BasicSymbolRepository<planning::Axiom>;
extern template class BasicSymbolRepository<planning::GroundAxiom>;
extern template class BasicSymbolRepository<planning::Metric>;
extern template class BasicSymbolRepository<planning::Domain>;
extern template class BasicSymbolRepository<planning::Task>;
extern template class BasicSymbolRepository<planning::FDRVariable<FluentTag>>;
extern template class BasicSymbolRepository<planning::ConjunctiveCondition>;
extern template class BasicSymbolRepository<planning::GroundConjunctiveCondition>;
extern template class BasicSymbolRepository<planning::FDRTask>;

// Outer repository
extern template class Repository<tyr::formalism::planning::SymbolRepository, tyr::formalism::planning::RelationRepository>;
}

namespace tyr
{
/**
 * Views
 */

// Views over indices
extern template struct View<Index<formalism::planning::Action>, formalism::planning::Repository>;
extern template struct View<Index<formalism::planning::Axiom>, formalism::planning::Repository>;
extern template struct View<Index<formalism::planning::ConditionalEffect>, formalism::planning::Repository>;
extern template struct View<Index<formalism::planning::ConjunctiveCondition>, formalism::planning::Repository>;
extern template struct View<Index<formalism::planning::ConjunctiveEffect>, formalism::planning::Repository>;
extern template struct View<Index<formalism::planning::Domain>, formalism::planning::Repository>;
extern template struct View<Index<formalism::planning::FDRTask>, formalism::planning::Repository>;
extern template struct View<Index<formalism::planning::GroundAction>, formalism::planning::Repository>;
extern template struct View<Index<formalism::planning::GroundAxiom>, formalism::planning::Repository>;
extern template struct View<Index<formalism::planning::GroundConditionalEffect>, formalism::planning::Repository>;
extern template struct View<Index<formalism::planning::GroundConjunctiveCondition>, formalism::planning::Repository>;
extern template struct View<Index<formalism::planning::GroundConjunctiveEffect>, formalism::planning::Repository>;
extern template struct View<Index<formalism::planning::Metric>, formalism::planning::Repository>;
extern template struct View<Index<formalism::Object>, formalism::planning::Repository>;
extern template struct View<Index<formalism::planning::Task>, formalism::planning::Repository>;
extern template struct View<Index<formalism::Variable>, formalism::planning::Repository>;

// FactKind-dependent index views
extern template struct View<Index<formalism::planning::Atom<formalism::StaticTag>>, formalism::planning::Repository>;
extern template struct View<Index<formalism::planning::Atom<formalism::FluentTag>>, formalism::planning::Repository>;
extern template struct View<Index<formalism::planning::Atom<formalism::DerivedTag>>, formalism::planning::Repository>;

extern template struct View<Index<formalism::planning::GroundAtom<formalism::StaticTag>>, formalism::planning::Repository>;
extern template struct View<Index<formalism::planning::GroundAtom<formalism::FluentTag>>, formalism::planning::Repository>;
extern template struct View<Index<formalism::planning::GroundAtom<formalism::DerivedTag>>, formalism::planning::Repository>;

extern template struct View<Index<formalism::planning::Literal<formalism::StaticTag>>, formalism::planning::Repository>;
extern template struct View<Index<formalism::planning::Literal<formalism::FluentTag>>, formalism::planning::Repository>;
extern template struct View<Index<formalism::planning::Literal<formalism::DerivedTag>>, formalism::planning::Repository>;

extern template struct View<Index<formalism::planning::GroundLiteral<formalism::StaticTag>>, formalism::planning::Repository>;
extern template struct View<Index<formalism::planning::GroundLiteral<formalism::FluentTag>>, formalism::planning::Repository>;
extern template struct View<Index<formalism::planning::GroundLiteral<formalism::DerivedTag>>, formalism::planning::Repository>;

extern template struct View<Index<formalism::Function<formalism::StaticTag>>, formalism::planning::Repository>;
extern template struct View<Index<formalism::Function<formalism::FluentTag>>, formalism::planning::Repository>;
extern template struct View<Index<formalism::Function<formalism::AuxiliaryTag>>, formalism::planning::Repository>;

extern template struct View<Index<formalism::planning::FunctionTerm<formalism::StaticTag>>, formalism::planning::Repository>;
extern template struct View<Index<formalism::planning::FunctionTerm<formalism::FluentTag>>, formalism::planning::Repository>;
extern template struct View<Index<formalism::planning::FunctionTerm<formalism::AuxiliaryTag>>, formalism::planning::Repository>;

extern template struct View<Index<formalism::planning::GroundFunctionTerm<formalism::StaticTag>>, formalism::planning::Repository>;
extern template struct View<Index<formalism::planning::GroundFunctionTerm<formalism::FluentTag>>, formalism::planning::Repository>;
extern template struct View<Index<formalism::planning::GroundFunctionTerm<formalism::AuxiliaryTag>>, formalism::planning::Repository>;

extern template struct View<Index<formalism::planning::GroundFunctionTermValue<formalism::StaticTag>>, formalism::planning::Repository>;
extern template struct View<Index<formalism::planning::GroundFunctionTermValue<formalism::FluentTag>>, formalism::planning::Repository>;
extern template struct View<Index<formalism::planning::GroundFunctionTermValue<formalism::AuxiliaryTag>>, formalism::planning::Repository>;

extern template struct View<Index<formalism::Predicate<formalism::StaticTag>>, formalism::planning::Repository>;
extern template struct View<Index<formalism::Predicate<formalism::FluentTag>>, formalism::planning::Repository>;
extern template struct View<Index<formalism::Predicate<formalism::DerivedTag>>, formalism::planning::Repository>;

extern template struct View<Index<formalism::planning::FDRVariable<formalism::FluentTag>>, formalism::planning::Repository>;

// Operator index views: lifted
extern template struct View<Index<formalism::planning::UnaryOperator<formalism::Sub, Data<formalism::planning::FunctionExpression>>>,
                            formalism::planning::Repository>;

extern template struct View<Index<formalism::planning::BinaryOperator<formalism::Add, Data<formalism::planning::FunctionExpression>>>,
                            formalism::planning::Repository>;
extern template struct View<Index<formalism::planning::BinaryOperator<formalism::Sub, Data<formalism::planning::FunctionExpression>>>,
                            formalism::planning::Repository>;
extern template struct View<Index<formalism::planning::BinaryOperator<formalism::Mul, Data<formalism::planning::FunctionExpression>>>,
                            formalism::planning::Repository>;
extern template struct View<Index<formalism::planning::BinaryOperator<formalism::Div, Data<formalism::planning::FunctionExpression>>>,
                            formalism::planning::Repository>;
extern template struct View<Index<formalism::planning::BinaryOperator<formalism::Eq, Data<formalism::planning::FunctionExpression>>>,
                            formalism::planning::Repository>;
extern template struct View<Index<formalism::planning::BinaryOperator<formalism::Ne, Data<formalism::planning::FunctionExpression>>>,
                            formalism::planning::Repository>;
extern template struct View<Index<formalism::planning::BinaryOperator<formalism::Le, Data<formalism::planning::FunctionExpression>>>,
                            formalism::planning::Repository>;
extern template struct View<Index<formalism::planning::BinaryOperator<formalism::Lt, Data<formalism::planning::FunctionExpression>>>,
                            formalism::planning::Repository>;
extern template struct View<Index<formalism::planning::BinaryOperator<formalism::Ge, Data<formalism::planning::FunctionExpression>>>,
                            formalism::planning::Repository>;
extern template struct View<Index<formalism::planning::BinaryOperator<formalism::Gt, Data<formalism::planning::FunctionExpression>>>,
                            formalism::planning::Repository>;

extern template struct View<Index<formalism::planning::MultiOperator<formalism::Add, Data<formalism::planning::FunctionExpression>>>,
                            formalism::planning::Repository>;
extern template struct View<Index<formalism::planning::MultiOperator<formalism::Mul, Data<formalism::planning::FunctionExpression>>>,
                            formalism::planning::Repository>;

// Operator index views: grounded
extern template struct View<Index<formalism::planning::UnaryOperator<formalism::Sub, Data<formalism::planning::GroundFunctionExpression>>>,
                            formalism::planning::Repository>;

extern template struct View<Index<formalism::planning::BinaryOperator<formalism::Add, Data<formalism::planning::GroundFunctionExpression>>>,
                            formalism::planning::Repository>;
extern template struct View<Index<formalism::planning::BinaryOperator<formalism::Sub, Data<formalism::planning::GroundFunctionExpression>>>,
                            formalism::planning::Repository>;
extern template struct View<Index<formalism::planning::BinaryOperator<formalism::Mul, Data<formalism::planning::GroundFunctionExpression>>>,
                            formalism::planning::Repository>;
extern template struct View<Index<formalism::planning::BinaryOperator<formalism::Div, Data<formalism::planning::GroundFunctionExpression>>>,
                            formalism::planning::Repository>;
extern template struct View<Index<formalism::planning::BinaryOperator<formalism::Eq, Data<formalism::planning::GroundFunctionExpression>>>,
                            formalism::planning::Repository>;
extern template struct View<Index<formalism::planning::BinaryOperator<formalism::Ne, Data<formalism::planning::GroundFunctionExpression>>>,
                            formalism::planning::Repository>;
extern template struct View<Index<formalism::planning::BinaryOperator<formalism::Le, Data<formalism::planning::GroundFunctionExpression>>>,
                            formalism::planning::Repository>;
extern template struct View<Index<formalism::planning::BinaryOperator<formalism::Lt, Data<formalism::planning::GroundFunctionExpression>>>,
                            formalism::planning::Repository>;
extern template struct View<Index<formalism::planning::BinaryOperator<formalism::Ge, Data<formalism::planning::GroundFunctionExpression>>>,
                            formalism::planning::Repository>;
extern template struct View<Index<formalism::planning::BinaryOperator<formalism::Gt, Data<formalism::planning::GroundFunctionExpression>>>,
                            formalism::planning::Repository>;

extern template struct View<Index<formalism::planning::MultiOperator<formalism::Add, Data<formalism::planning::GroundFunctionExpression>>>,
                            formalism::planning::Repository>;
extern template struct View<Index<formalism::planning::MultiOperator<formalism::Mul, Data<formalism::planning::GroundFunctionExpression>>>,
                            formalism::planning::Repository>;

// Numeric effect index views
extern template struct View<Index<formalism::planning::NumericEffect<formalism::Assign, formalism::FluentTag>>, formalism::planning::Repository>;
extern template struct View<Index<formalism::planning::NumericEffect<formalism::Increase, formalism::FluentTag>>, formalism::planning::Repository>;
extern template struct View<Index<formalism::planning::NumericEffect<formalism::Decrease, formalism::FluentTag>>, formalism::planning::Repository>;
extern template struct View<Index<formalism::planning::NumericEffect<formalism::ScaleUp, formalism::FluentTag>>, formalism::planning::Repository>;
extern template struct View<Index<formalism::planning::NumericEffect<formalism::ScaleDown, formalism::FluentTag>>, formalism::planning::Repository>;
extern template struct View<Index<formalism::planning::NumericEffect<formalism::Increase, formalism::AuxiliaryTag>>, formalism::planning::Repository>;

extern template struct View<Index<formalism::planning::GroundNumericEffect<formalism::Assign, formalism::FluentTag>>, formalism::planning::Repository>;
extern template struct View<Index<formalism::planning::GroundNumericEffect<formalism::Increase, formalism::FluentTag>>, formalism::planning::Repository>;
extern template struct View<Index<formalism::planning::GroundNumericEffect<formalism::Decrease, formalism::FluentTag>>, formalism::planning::Repository>;
extern template struct View<Index<formalism::planning::GroundNumericEffect<formalism::ScaleUp, formalism::FluentTag>>, formalism::planning::Repository>;
extern template struct View<Index<formalism::planning::GroundNumericEffect<formalism::ScaleDown, formalism::FluentTag>>, formalism::planning::Repository>;
extern template struct View<Index<formalism::planning::GroundNumericEffect<formalism::Increase, formalism::AuxiliaryTag>>, formalism::planning::Repository>;

// Data views
extern template struct View<Data<formalism::planning::ArithmeticOperator<Data<formalism::planning::FunctionExpression>>>, formalism::planning::Repository>;
extern template struct View<Data<formalism::planning::ArithmeticOperator<Data<formalism::planning::GroundFunctionExpression>>>,
                            formalism::planning::Repository>;
extern template struct View<Data<formalism::planning::BooleanOperator<Data<formalism::planning::FunctionExpression>>>, formalism::planning::Repository>;
extern template struct View<Data<formalism::planning::BooleanOperator<Data<formalism::planning::GroundFunctionExpression>>>, formalism::planning::Repository>;
extern template struct View<Data<formalism::planning::FunctionExpression>, formalism::planning::Repository>;
extern template struct View<Data<formalism::planning::GroundFunctionExpression>, formalism::planning::Repository>;
extern template struct View<Data<formalism::planning::NumericEffectOperator<formalism::FluentTag>>, formalism::planning::Repository>;
extern template struct View<Data<formalism::planning::NumericEffectOperator<formalism::AuxiliaryTag>>, formalism::planning::Repository>;
extern template struct View<Data<formalism::planning::GroundNumericEffectOperator<formalism::FluentTag>>, formalism::planning::Repository>;
extern template struct View<Data<formalism::planning::GroundNumericEffectOperator<formalism::AuxiliaryTag>>, formalism::planning::Repository>;
extern template struct View<Data<formalism::Term>, formalism::planning::Repository>;
extern template struct View<Data<formalism::planning::FDRFact<formalism::FluentTag>>, formalism::planning::Repository>;

// Pair views
extern template struct View<Index<formalism::RelationBinding<formalism::Predicate<formalism::StaticTag>>>, formalism::planning::Repository>;
extern template struct View<Index<formalism::RelationBinding<formalism::Predicate<formalism::FluentTag>>>, formalism::planning::Repository>;
extern template struct View<Index<formalism::RelationBinding<formalism::Predicate<formalism::DerivedTag>>>, formalism::planning::Repository>;

extern template struct View<Index<formalism::RelationBinding<formalism::Function<formalism::StaticTag>>>, formalism::planning::Repository>;
extern template struct View<Index<formalism::RelationBinding<formalism::Function<formalism::FluentTag>>>, formalism::planning::Repository>;
extern template struct View<Index<formalism::RelationBinding<formalism::Function<formalism::AuxiliaryTag>>>, formalism::planning::Repository>;

extern template struct View<Index<formalism::RelationBinding<formalism::planning::Action>>, formalism::planning::Repository>;
extern template struct View<Index<formalism::RelationBinding<formalism::planning::Axiom>>, formalism::planning::Repository>;
}

#endif

#endif
