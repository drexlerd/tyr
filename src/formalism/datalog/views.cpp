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

#ifndef TYR_HEADER_INSTANTIATION

namespace tyr
{
/**
 * Views
 */

template struct View<Index<formalism::Variable>, formalism::datalog::Repository>;
template struct View<Index<formalism::Object>, formalism::datalog::Repository>;

template struct View<Index<formalism::Predicate<formalism::StaticTag>>, formalism::datalog::Repository>;
template struct View<Index<formalism::Predicate<formalism::FluentTag>>, formalism::datalog::Repository>;

template struct View<Index<formalism::datalog::Atom<formalism::StaticTag>>, formalism::datalog::Repository>;
template struct View<Index<formalism::datalog::Atom<formalism::FluentTag>>, formalism::datalog::Repository>;

template struct View<Index<formalism::datalog::GroundAtom<formalism::StaticTag>>, formalism::datalog::Repository>;
template struct View<Index<formalism::datalog::GroundAtom<formalism::FluentTag>>, formalism::datalog::Repository>;

template struct View<Index<formalism::datalog::Literal<formalism::StaticTag>>, formalism::datalog::Repository>;
template struct View<Index<formalism::datalog::Literal<formalism::FluentTag>>, formalism::datalog::Repository>;

template struct View<Index<formalism::datalog::GroundLiteral<formalism::StaticTag>>, formalism::datalog::Repository>;
template struct View<Index<formalism::datalog::GroundLiteral<formalism::FluentTag>>, formalism::datalog::Repository>;

template struct View<Index<formalism::Function<formalism::StaticTag>>, formalism::datalog::Repository>;
template struct View<Index<formalism::Function<formalism::FluentTag>>, formalism::datalog::Repository>;

template struct View<Index<formalism::datalog::FunctionTerm<formalism::StaticTag>>, formalism::datalog::Repository>;
template struct View<Index<formalism::datalog::FunctionTerm<formalism::FluentTag>>, formalism::datalog::Repository>;

template struct View<Index<formalism::datalog::GroundFunctionTerm<formalism::StaticTag>>, formalism::datalog::Repository>;
template struct View<Index<formalism::datalog::GroundFunctionTerm<formalism::FluentTag>>, formalism::datalog::Repository>;

template struct View<Index<formalism::datalog::GroundFunctionTermValue<formalism::StaticTag>>, formalism::datalog::Repository>;
template struct View<Index<formalism::datalog::GroundFunctionTermValue<formalism::FluentTag>>, formalism::datalog::Repository>;

template struct View<Index<formalism::datalog::UnaryOperator<formalism::Sub, Data<formalism::datalog::FunctionExpression>>>, formalism::datalog::Repository>;

template struct View<Index<formalism::datalog::BinaryOperator<formalism::Add, Data<formalism::datalog::FunctionExpression>>>, formalism::datalog::Repository>;
template struct View<Index<formalism::datalog::BinaryOperator<formalism::Sub, Data<formalism::datalog::FunctionExpression>>>, formalism::datalog::Repository>;
template struct View<Index<formalism::datalog::BinaryOperator<formalism::Mul, Data<formalism::datalog::FunctionExpression>>>, formalism::datalog::Repository>;
template struct View<Index<formalism::datalog::BinaryOperator<formalism::Div, Data<formalism::datalog::FunctionExpression>>>, formalism::datalog::Repository>;
template struct View<Index<formalism::datalog::BinaryOperator<formalism::Eq, Data<formalism::datalog::FunctionExpression>>>, formalism::datalog::Repository>;
template struct View<Index<formalism::datalog::BinaryOperator<formalism::Ne, Data<formalism::datalog::FunctionExpression>>>, formalism::datalog::Repository>;
template struct View<Index<formalism::datalog::BinaryOperator<formalism::Le, Data<formalism::datalog::FunctionExpression>>>, formalism::datalog::Repository>;
template struct View<Index<formalism::datalog::BinaryOperator<formalism::Lt, Data<formalism::datalog::FunctionExpression>>>, formalism::datalog::Repository>;
template struct View<Index<formalism::datalog::BinaryOperator<formalism::Ge, Data<formalism::datalog::FunctionExpression>>>, formalism::datalog::Repository>;
template struct View<Index<formalism::datalog::BinaryOperator<formalism::Gt, Data<formalism::datalog::FunctionExpression>>>, formalism::datalog::Repository>;

template struct View<Index<formalism::datalog::MultiOperator<formalism::Add, Data<formalism::datalog::FunctionExpression>>>, formalism::datalog::Repository>;
template struct View<Index<formalism::datalog::MultiOperator<formalism::Mul, Data<formalism::datalog::FunctionExpression>>>, formalism::datalog::Repository>;

template struct View<Index<formalism::datalog::UnaryOperator<formalism::Sub, Data<formalism::datalog::GroundFunctionExpression>>>,
                     formalism::datalog::Repository>;

template struct View<Index<formalism::datalog::BinaryOperator<formalism::Add, Data<formalism::datalog::GroundFunctionExpression>>>,
                     formalism::datalog::Repository>;
template struct View<Index<formalism::datalog::BinaryOperator<formalism::Sub, Data<formalism::datalog::GroundFunctionExpression>>>,
                     formalism::datalog::Repository>;
template struct View<Index<formalism::datalog::BinaryOperator<formalism::Mul, Data<formalism::datalog::GroundFunctionExpression>>>,
                     formalism::datalog::Repository>;
template struct View<Index<formalism::datalog::BinaryOperator<formalism::Div, Data<formalism::datalog::GroundFunctionExpression>>>,
                     formalism::datalog::Repository>;
template struct View<Index<formalism::datalog::BinaryOperator<formalism::Eq, Data<formalism::datalog::GroundFunctionExpression>>>,
                     formalism::datalog::Repository>;
template struct View<Index<formalism::datalog::BinaryOperator<formalism::Ne, Data<formalism::datalog::GroundFunctionExpression>>>,
                     formalism::datalog::Repository>;
template struct View<Index<formalism::datalog::BinaryOperator<formalism::Le, Data<formalism::datalog::GroundFunctionExpression>>>,
                     formalism::datalog::Repository>;
template struct View<Index<formalism::datalog::BinaryOperator<formalism::Lt, Data<formalism::datalog::GroundFunctionExpression>>>,
                     formalism::datalog::Repository>;
template struct View<Index<formalism::datalog::BinaryOperator<formalism::Ge, Data<formalism::datalog::GroundFunctionExpression>>>,
                     formalism::datalog::Repository>;
template struct View<Index<formalism::datalog::BinaryOperator<formalism::Gt, Data<formalism::datalog::GroundFunctionExpression>>>,
                     formalism::datalog::Repository>;

template struct View<Index<formalism::datalog::MultiOperator<formalism::Add, Data<formalism::datalog::GroundFunctionExpression>>>,
                     formalism::datalog::Repository>;
template struct View<Index<formalism::datalog::MultiOperator<formalism::Mul, Data<formalism::datalog::GroundFunctionExpression>>>,
                     formalism::datalog::Repository>;

template struct View<Index<formalism::datalog::NumericEffect<formalism::Assign, formalism::FluentTag>>, formalism::datalog::Repository>;
template struct View<Index<formalism::datalog::NumericEffect<formalism::Increase, formalism::FluentTag>>, formalism::datalog::Repository>;
template struct View<Index<formalism::datalog::NumericEffect<formalism::Decrease, formalism::FluentTag>>, formalism::datalog::Repository>;
template struct View<Index<formalism::datalog::NumericEffect<formalism::ScaleUp, formalism::FluentTag>>, formalism::datalog::Repository>;
template struct View<Index<formalism::datalog::NumericEffect<formalism::ScaleDown, formalism::FluentTag>>, formalism::datalog::Repository>;
template struct View<Index<formalism::datalog::GroundNumericEffect<formalism::Assign, formalism::FluentTag>>, formalism::datalog::Repository>;
template struct View<Index<formalism::datalog::GroundNumericEffect<formalism::Increase, formalism::FluentTag>>, formalism::datalog::Repository>;
template struct View<Index<formalism::datalog::GroundNumericEffect<formalism::Decrease, formalism::FluentTag>>, formalism::datalog::Repository>;
template struct View<Index<formalism::datalog::GroundNumericEffect<formalism::ScaleUp, formalism::FluentTag>>, formalism::datalog::Repository>;
template struct View<Index<formalism::datalog::GroundNumericEffect<formalism::ScaleDown, formalism::FluentTag>>, formalism::datalog::Repository>;
template struct View<Data<formalism::datalog::NumericEffectOperator<formalism::FluentTag>>, formalism::datalog::Repository>;
template struct View<Data<formalism::datalog::GroundNumericEffectOperator<formalism::FluentTag>>, formalism::datalog::Repository>;

template struct View<Data<formalism::datalog::ArithmeticOperator<Data<formalism::datalog::FunctionExpression>>>, formalism::datalog::Repository>;
template struct View<Data<formalism::datalog::ArithmeticOperator<Data<formalism::datalog::GroundFunctionExpression>>>, formalism::datalog::Repository>;

template struct View<Data<formalism::datalog::BooleanOperator<Data<formalism::datalog::FunctionExpression>>>, formalism::datalog::Repository>;
template struct View<Data<formalism::datalog::BooleanOperator<Data<formalism::datalog::GroundFunctionExpression>>>, formalism::datalog::Repository>;

template struct View<Data<formalism::datalog::FunctionExpression>, formalism::datalog::Repository>;
template struct View<Data<formalism::datalog::GroundFunctionExpression>, formalism::datalog::Repository>;

template struct View<Index<formalism::datalog::ConjunctiveCondition>, formalism::datalog::Repository>;
template struct View<Index<formalism::datalog::GroundConjunctiveCondition>, formalism::datalog::Repository>;

template struct View<Index<formalism::datalog::Rule>, formalism::datalog::Repository>;
template struct View<Index<formalism::datalog::GroundRule>, formalism::datalog::Repository>;
template struct View<Index<formalism::datalog::Program>, formalism::datalog::Repository>;

template struct View<Data<formalism::Term>, formalism::datalog::Repository>;

template struct View<Index<formalism::RelationBinding<formalism::Predicate<formalism::StaticTag>>>, formalism::datalog::Repository>;
template struct View<Index<formalism::RelationBinding<formalism::Predicate<formalism::FluentTag>>>, formalism::datalog::Repository>;

template struct View<Index<formalism::RelationBinding<formalism::Function<formalism::StaticTag>>>, formalism::datalog::Repository>;
template struct View<Index<formalism::RelationBinding<formalism::Function<formalism::FluentTag>>>, formalism::datalog::Repository>;

template struct View<Index<formalism::RelationBinding<formalism::datalog::Rule>>, formalism::datalog::Repository>;

}

#endif
