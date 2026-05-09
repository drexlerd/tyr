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

#include "tyr/formalism/datalog/repository.hpp"

namespace tyr::formalism
{
#ifndef TYR_HEADER_INSTANTIATION

// BasicRelationRepository
template class BasicRelationRepository<Predicate<StaticTag>>;
template class BasicRelationRepository<Predicate<FluentTag>>;
template class BasicRelationRepository<Function<StaticTag>>;
template class BasicRelationRepository<Function<FluentTag>>;
template class BasicRelationRepository<datalog::Rule>;

// BasicSymbolRepository
template class BasicSymbolRepository<Variable>;
template class BasicSymbolRepository<Object>;
template class BasicSymbolRepository<Predicate<StaticTag>>;
template class BasicSymbolRepository<Predicate<FluentTag>>;
template class BasicSymbolRepository<datalog::Atom<StaticTag>>;
template class BasicSymbolRepository<datalog::Atom<FluentTag>>;
template class BasicSymbolRepository<datalog::GroundAtom<StaticTag>>;
template class BasicSymbolRepository<datalog::GroundAtom<FluentTag>>;
template class BasicSymbolRepository<datalog::Literal<StaticTag>>;
template class BasicSymbolRepository<datalog::Literal<FluentTag>>;
template class BasicSymbolRepository<datalog::GroundLiteral<StaticTag>>;
template class BasicSymbolRepository<datalog::GroundLiteral<FluentTag>>;
template class BasicSymbolRepository<Function<StaticTag>>;
template class BasicSymbolRepository<Function<FluentTag>>;
template class BasicSymbolRepository<datalog::FunctionTerm<StaticTag>>;
template class BasicSymbolRepository<datalog::FunctionTerm<FluentTag>>;
template class BasicSymbolRepository<datalog::GroundFunctionTerm<StaticTag>>;
template class BasicSymbolRepository<datalog::GroundFunctionTerm<FluentTag>>;
template class BasicSymbolRepository<datalog::GroundFunctionTermValue<StaticTag>>;
template class BasicSymbolRepository<datalog::GroundFunctionTermValue<FluentTag>>;
template class BasicSymbolRepository<datalog::UnaryOperator<Sub, Data<datalog::FunctionExpression>>>;
template class BasicSymbolRepository<datalog::BinaryOperator<Add, Data<datalog::FunctionExpression>>>;
template class BasicSymbolRepository<datalog::BinaryOperator<Sub, Data<datalog::FunctionExpression>>>;
template class BasicSymbolRepository<datalog::BinaryOperator<Mul, Data<datalog::FunctionExpression>>>;
template class BasicSymbolRepository<datalog::BinaryOperator<Div, Data<datalog::FunctionExpression>>>;
template class BasicSymbolRepository<datalog::MultiOperator<Add, Data<datalog::FunctionExpression>>>;
template class BasicSymbolRepository<datalog::MultiOperator<Mul, Data<datalog::FunctionExpression>>>;
template class BasicSymbolRepository<datalog::BinaryOperator<Eq, Data<datalog::FunctionExpression>>>;
template class BasicSymbolRepository<datalog::BinaryOperator<Ne, Data<datalog::FunctionExpression>>>;
template class BasicSymbolRepository<datalog::BinaryOperator<Le, Data<datalog::FunctionExpression>>>;
template class BasicSymbolRepository<datalog::BinaryOperator<Lt, Data<datalog::FunctionExpression>>>;
template class BasicSymbolRepository<datalog::BinaryOperator<Ge, Data<datalog::FunctionExpression>>>;
template class BasicSymbolRepository<datalog::BinaryOperator<Gt, Data<datalog::FunctionExpression>>>;
template class BasicSymbolRepository<datalog::UnaryOperator<Sub, Data<datalog::GroundFunctionExpression>>>;
template class BasicSymbolRepository<datalog::BinaryOperator<Add, Data<datalog::GroundFunctionExpression>>>;
template class BasicSymbolRepository<datalog::BinaryOperator<Sub, Data<datalog::GroundFunctionExpression>>>;
template class BasicSymbolRepository<datalog::BinaryOperator<Mul, Data<datalog::GroundFunctionExpression>>>;
template class BasicSymbolRepository<datalog::BinaryOperator<Div, Data<datalog::GroundFunctionExpression>>>;
template class BasicSymbolRepository<datalog::MultiOperator<Add, Data<datalog::GroundFunctionExpression>>>;
template class BasicSymbolRepository<datalog::MultiOperator<Mul, Data<datalog::GroundFunctionExpression>>>;
template class BasicSymbolRepository<datalog::BinaryOperator<Eq, Data<datalog::GroundFunctionExpression>>>;
template class BasicSymbolRepository<datalog::BinaryOperator<Ne, Data<datalog::GroundFunctionExpression>>>;
template class BasicSymbolRepository<datalog::BinaryOperator<Le, Data<datalog::GroundFunctionExpression>>>;
template class BasicSymbolRepository<datalog::BinaryOperator<Lt, Data<datalog::GroundFunctionExpression>>>;
template class BasicSymbolRepository<datalog::BinaryOperator<Ge, Data<datalog::GroundFunctionExpression>>>;
template class BasicSymbolRepository<datalog::BinaryOperator<Gt, Data<datalog::GroundFunctionExpression>>>;
template class BasicSymbolRepository<datalog::NumericEffect<Assign, FluentTag>>;
template class BasicSymbolRepository<datalog::NumericEffect<Increase, FluentTag>>;
template class BasicSymbolRepository<datalog::NumericEffect<Decrease, FluentTag>>;
template class BasicSymbolRepository<datalog::NumericEffect<ScaleUp, FluentTag>>;
template class BasicSymbolRepository<datalog::NumericEffect<ScaleDown, FluentTag>>;
template class BasicSymbolRepository<datalog::GroundNumericEffect<Assign, FluentTag>>;
template class BasicSymbolRepository<datalog::GroundNumericEffect<Increase, FluentTag>>;
template class BasicSymbolRepository<datalog::GroundNumericEffect<Decrease, FluentTag>>;
template class BasicSymbolRepository<datalog::GroundNumericEffect<ScaleUp, FluentTag>>;
template class BasicSymbolRepository<datalog::GroundNumericEffect<ScaleDown, FluentTag>>;
template class BasicSymbolRepository<datalog::ConjunctiveCondition>;
template class BasicSymbolRepository<datalog::Rule>;
template class BasicSymbolRepository<datalog::GroundConjunctiveCondition>;
template class BasicSymbolRepository<datalog::GroundRule>;
template class BasicSymbolRepository<datalog::Program>;

// Outer repository
template class Repository<tyr::formalism::datalog::SymbolRepository, tyr::formalism::datalog::RelationRepository>;

#endif

namespace datalog
{
static_assert(RepositoryConcept<Repository>);
static_assert(Context<Repository>);
}
}
