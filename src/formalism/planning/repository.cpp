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

#include "tyr/formalism/planning/repository.hpp"

namespace tyr::formalism
{
#ifndef TYR_HEADER_INSTANTIATION

// BasicRelationRepository
template class BasicRelationRepository<Predicate<StaticTag>>;
template class BasicRelationRepository<Predicate<FluentTag>>;
template class BasicRelationRepository<Predicate<DerivedTag>>;
template class BasicRelationRepository<Function<StaticTag>>;
template class BasicRelationRepository<Function<FluentTag>>;
template class BasicRelationRepository<Function<AuxiliaryTag>>;
template class BasicRelationRepository<planning::Action>;
template class BasicRelationRepository<planning::Axiom>;

// BasicSymbolRepository
template class BasicSymbolRepository<Variable>;
template class BasicSymbolRepository<Object>;
template class BasicSymbolRepository<Predicate<StaticTag>>;
template class BasicSymbolRepository<Predicate<FluentTag>>;
template class BasicSymbolRepository<Predicate<DerivedTag>>;
template class BasicSymbolRepository<planning::Atom<StaticTag>>;
template class BasicSymbolRepository<planning::Atom<FluentTag>>;
template class BasicSymbolRepository<planning::Atom<DerivedTag>>;
template class BasicSymbolRepository<planning::GroundAtom<StaticTag>>;
template class BasicSymbolRepository<planning::GroundAtom<FluentTag>>;
template class BasicSymbolRepository<planning::GroundAtom<DerivedTag>>;
template class BasicSymbolRepository<planning::Literal<StaticTag>>;
template class BasicSymbolRepository<planning::Literal<FluentTag>>;
template class BasicSymbolRepository<planning::Literal<DerivedTag>>;
template class BasicSymbolRepository<planning::GroundLiteral<StaticTag>>;
template class BasicSymbolRepository<planning::GroundLiteral<FluentTag>>;
template class BasicSymbolRepository<planning::GroundLiteral<DerivedTag>>;
template class BasicSymbolRepository<Function<StaticTag>>;
template class BasicSymbolRepository<Function<FluentTag>>;
template class BasicSymbolRepository<Function<AuxiliaryTag>>;
template class BasicSymbolRepository<planning::FunctionTerm<StaticTag>>;
template class BasicSymbolRepository<planning::FunctionTerm<FluentTag>>;
template class BasicSymbolRepository<planning::FunctionTerm<AuxiliaryTag>>;
template class BasicSymbolRepository<planning::GroundFunctionTerm<StaticTag>>;
template class BasicSymbolRepository<planning::GroundFunctionTerm<FluentTag>>;
template class BasicSymbolRepository<planning::GroundFunctionTerm<AuxiliaryTag>>;
template class BasicSymbolRepository<planning::GroundFunctionTermValue<StaticTag>>;
template class BasicSymbolRepository<planning::GroundFunctionTermValue<FluentTag>>;
template class BasicSymbolRepository<planning::GroundFunctionTermValue<AuxiliaryTag>>;
template class BasicSymbolRepository<planning::UnaryOperator<Sub, Data<planning::FunctionExpression>>>;
template class BasicSymbolRepository<planning::BinaryOperator<Add, Data<planning::FunctionExpression>>>;
template class BasicSymbolRepository<planning::BinaryOperator<Sub, Data<planning::FunctionExpression>>>;
template class BasicSymbolRepository<planning::BinaryOperator<Mul, Data<planning::FunctionExpression>>>;
template class BasicSymbolRepository<planning::BinaryOperator<Div, Data<planning::FunctionExpression>>>;
template class BasicSymbolRepository<planning::MultiOperator<Add, Data<planning::FunctionExpression>>>;
template class BasicSymbolRepository<planning::MultiOperator<Mul, Data<planning::FunctionExpression>>>;
template class BasicSymbolRepository<planning::BinaryOperator<Eq, Data<planning::FunctionExpression>>>;
template class BasicSymbolRepository<planning::BinaryOperator<Ne, Data<planning::FunctionExpression>>>;
template class BasicSymbolRepository<planning::BinaryOperator<Le, Data<planning::FunctionExpression>>>;
template class BasicSymbolRepository<planning::BinaryOperator<Lt, Data<planning::FunctionExpression>>>;
template class BasicSymbolRepository<planning::BinaryOperator<Ge, Data<planning::FunctionExpression>>>;
template class BasicSymbolRepository<planning::BinaryOperator<Gt, Data<planning::FunctionExpression>>>;
template class BasicSymbolRepository<planning::UnaryOperator<Sub, Data<planning::GroundFunctionExpression>>>;
template class BasicSymbolRepository<planning::BinaryOperator<Add, Data<planning::GroundFunctionExpression>>>;
template class BasicSymbolRepository<planning::BinaryOperator<Sub, Data<planning::GroundFunctionExpression>>>;
template class BasicSymbolRepository<planning::BinaryOperator<Mul, Data<planning::GroundFunctionExpression>>>;
template class BasicSymbolRepository<planning::BinaryOperator<Div, Data<planning::GroundFunctionExpression>>>;
template class BasicSymbolRepository<planning::MultiOperator<Add, Data<planning::GroundFunctionExpression>>>;
template class BasicSymbolRepository<planning::MultiOperator<Mul, Data<planning::GroundFunctionExpression>>>;
template class BasicSymbolRepository<planning::BinaryOperator<Eq, Data<planning::GroundFunctionExpression>>>;
template class BasicSymbolRepository<planning::BinaryOperator<Ne, Data<planning::GroundFunctionExpression>>>;
template class BasicSymbolRepository<planning::BinaryOperator<Le, Data<planning::GroundFunctionExpression>>>;
template class BasicSymbolRepository<planning::BinaryOperator<Lt, Data<planning::GroundFunctionExpression>>>;
template class BasicSymbolRepository<planning::BinaryOperator<Ge, Data<planning::GroundFunctionExpression>>>;
template class BasicSymbolRepository<planning::BinaryOperator<Gt, Data<planning::GroundFunctionExpression>>>;
template class BasicSymbolRepository<planning::NumericEffect<Assign, FluentTag>>;
template class BasicSymbolRepository<planning::NumericEffect<Increase, FluentTag>>;
template class BasicSymbolRepository<planning::NumericEffect<Decrease, FluentTag>>;
template class BasicSymbolRepository<planning::NumericEffect<ScaleUp, FluentTag>>;
template class BasicSymbolRepository<planning::NumericEffect<ScaleDown, FluentTag>>;
template class BasicSymbolRepository<planning::NumericEffect<Increase, AuxiliaryTag>>;
template class BasicSymbolRepository<planning::GroundNumericEffect<Assign, FluentTag>>;
template class BasicSymbolRepository<planning::GroundNumericEffect<Increase, FluentTag>>;
template class BasicSymbolRepository<planning::GroundNumericEffect<Decrease, FluentTag>>;
template class BasicSymbolRepository<planning::GroundNumericEffect<ScaleUp, FluentTag>>;
template class BasicSymbolRepository<planning::GroundNumericEffect<ScaleDown, FluentTag>>;
template class BasicSymbolRepository<planning::GroundNumericEffect<Increase, AuxiliaryTag>>;
template class BasicSymbolRepository<planning::ConditionalEffect>;
template class BasicSymbolRepository<planning::GroundConditionalEffect>;
template class BasicSymbolRepository<planning::ConjunctiveEffect>;
template class BasicSymbolRepository<planning::GroundConjunctiveEffect>;
template class BasicSymbolRepository<planning::Action>;
template class BasicSymbolRepository<planning::GroundAction>;
template class BasicSymbolRepository<planning::Axiom>;
template class BasicSymbolRepository<planning::GroundAxiom>;
template class BasicSymbolRepository<planning::Metric>;
template class BasicSymbolRepository<planning::Domain>;
template class BasicSymbolRepository<planning::Task>;
template class BasicSymbolRepository<planning::FDRVariable<FluentTag>>;
template class BasicSymbolRepository<planning::ConjunctiveCondition>;
template class BasicSymbolRepository<planning::GroundConjunctiveCondition>;
template class BasicSymbolRepository<planning::FDRTask>;

// Outer repository
template class Repository<tyr::formalism::planning::SymbolRepository, tyr::formalism::planning::RelationRepository>;

#endif

namespace planning
{
static_assert(RepositoryConcept<Repository>);
static_assert(Context<Repository>);
}
}
