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

#ifndef TYR_HEADER_INSTANTIATION

namespace tyr
{
/**
 * Views
 */

// Views over indices
template struct View<Index<formalism::planning::Action>, formalism::planning::Repository>;
template struct View<Index<formalism::planning::Axiom>, formalism::planning::Repository>;
template struct View<Index<formalism::planning::ConditionalEffect>, formalism::planning::Repository>;
template struct View<Index<formalism::planning::ConjunctiveCondition>, formalism::planning::Repository>;
template struct View<Index<formalism::planning::ConjunctiveEffect>, formalism::planning::Repository>;
template struct View<Index<formalism::planning::Domain>, formalism::planning::Repository>;
template struct View<Index<formalism::planning::FDRTask>, formalism::planning::Repository>;
template struct View<Index<formalism::planning::GroundAction>, formalism::planning::Repository>;
template struct View<Index<formalism::planning::GroundAxiom>, formalism::planning::Repository>;
template struct View<Index<formalism::planning::GroundConditionalEffect>, formalism::planning::Repository>;
template struct View<Index<formalism::planning::GroundConjunctiveCondition>, formalism::planning::Repository>;
template struct View<Index<formalism::planning::GroundConjunctiveEffect>, formalism::planning::Repository>;
template struct View<Index<formalism::planning::Metric>, formalism::planning::Repository>;
template struct View<Index<formalism::Object>, formalism::planning::Repository>;
template struct View<Index<formalism::planning::Task>, formalism::planning::Repository>;
template struct View<Index<formalism::Variable>, formalism::planning::Repository>;

// FactKind-dependent index views
template struct View<Index<formalism::planning::Atom<formalism::StaticTag>>, formalism::planning::Repository>;
template struct View<Index<formalism::planning::Atom<formalism::FluentTag>>, formalism::planning::Repository>;
template struct View<Index<formalism::planning::Atom<formalism::DerivedTag>>, formalism::planning::Repository>;

template struct View<Index<formalism::planning::GroundAtom<formalism::StaticTag>>, formalism::planning::Repository>;
template struct View<Index<formalism::planning::GroundAtom<formalism::FluentTag>>, formalism::planning::Repository>;
template struct View<Index<formalism::planning::GroundAtom<formalism::DerivedTag>>, formalism::planning::Repository>;

template struct View<Index<formalism::planning::Literal<formalism::StaticTag>>, formalism::planning::Repository>;
template struct View<Index<formalism::planning::Literal<formalism::FluentTag>>, formalism::planning::Repository>;
template struct View<Index<formalism::planning::Literal<formalism::DerivedTag>>, formalism::planning::Repository>;

template struct View<Index<formalism::planning::GroundLiteral<formalism::StaticTag>>, formalism::planning::Repository>;
template struct View<Index<formalism::planning::GroundLiteral<formalism::FluentTag>>, formalism::planning::Repository>;
template struct View<Index<formalism::planning::GroundLiteral<formalism::DerivedTag>>, formalism::planning::Repository>;

template struct View<Index<formalism::Function<formalism::StaticTag>>, formalism::planning::Repository>;
template struct View<Index<formalism::Function<formalism::FluentTag>>, formalism::planning::Repository>;
template struct View<Index<formalism::Function<formalism::AuxiliaryTag>>, formalism::planning::Repository>;

template struct View<Index<formalism::planning::FunctionTerm<formalism::StaticTag>>, formalism::planning::Repository>;
template struct View<Index<formalism::planning::FunctionTerm<formalism::FluentTag>>, formalism::planning::Repository>;
template struct View<Index<formalism::planning::FunctionTerm<formalism::AuxiliaryTag>>, formalism::planning::Repository>;

template struct View<Index<formalism::planning::GroundFunctionTerm<formalism::StaticTag>>, formalism::planning::Repository>;
template struct View<Index<formalism::planning::GroundFunctionTerm<formalism::FluentTag>>, formalism::planning::Repository>;
template struct View<Index<formalism::planning::GroundFunctionTerm<formalism::AuxiliaryTag>>, formalism::planning::Repository>;

template struct View<Index<formalism::planning::GroundFunctionTermValue<formalism::StaticTag>>, formalism::planning::Repository>;
template struct View<Index<formalism::planning::GroundFunctionTermValue<formalism::FluentTag>>, formalism::planning::Repository>;
template struct View<Index<formalism::planning::GroundFunctionTermValue<formalism::AuxiliaryTag>>, formalism::planning::Repository>;

template struct View<Index<formalism::Predicate<formalism::StaticTag>>, formalism::planning::Repository>;
template struct View<Index<formalism::Predicate<formalism::FluentTag>>, formalism::planning::Repository>;
template struct View<Index<formalism::Predicate<formalism::DerivedTag>>, formalism::planning::Repository>;

template struct View<Index<formalism::planning::FDRVariable<formalism::FluentTag>>, formalism::planning::Repository>;

// Operator index views: lifted
template struct View<Index<formalism::planning::UnaryOperator<formalism::Sub, Data<formalism::planning::FunctionExpression>>>, formalism::planning::Repository>;

template struct View<Index<formalism::planning::BinaryOperator<formalism::Add, Data<formalism::planning::FunctionExpression>>>,
                     formalism::planning::Repository>;
template struct View<Index<formalism::planning::BinaryOperator<formalism::Sub, Data<formalism::planning::FunctionExpression>>>,
                     formalism::planning::Repository>;
template struct View<Index<formalism::planning::BinaryOperator<formalism::Mul, Data<formalism::planning::FunctionExpression>>>,
                     formalism::planning::Repository>;
template struct View<Index<formalism::planning::BinaryOperator<formalism::Div, Data<formalism::planning::FunctionExpression>>>,
                     formalism::planning::Repository>;
template struct View<Index<formalism::planning::BinaryOperator<formalism::Eq, Data<formalism::planning::FunctionExpression>>>, formalism::planning::Repository>;
template struct View<Index<formalism::planning::BinaryOperator<formalism::Ne, Data<formalism::planning::FunctionExpression>>>, formalism::planning::Repository>;
template struct View<Index<formalism::planning::BinaryOperator<formalism::Le, Data<formalism::planning::FunctionExpression>>>, formalism::planning::Repository>;
template struct View<Index<formalism::planning::BinaryOperator<formalism::Lt, Data<formalism::planning::FunctionExpression>>>, formalism::planning::Repository>;
template struct View<Index<formalism::planning::BinaryOperator<formalism::Ge, Data<formalism::planning::FunctionExpression>>>, formalism::planning::Repository>;
template struct View<Index<formalism::planning::BinaryOperator<formalism::Gt, Data<formalism::planning::FunctionExpression>>>, formalism::planning::Repository>;

template struct View<Index<formalism::planning::MultiOperator<formalism::Add, Data<formalism::planning::FunctionExpression>>>, formalism::planning::Repository>;
template struct View<Index<formalism::planning::MultiOperator<formalism::Mul, Data<formalism::planning::FunctionExpression>>>, formalism::planning::Repository>;

// Operator index views: grounded
template struct View<Index<formalism::planning::UnaryOperator<formalism::Sub, Data<formalism::planning::GroundFunctionExpression>>>,
                     formalism::planning::Repository>;

template struct View<Index<formalism::planning::BinaryOperator<formalism::Add, Data<formalism::planning::GroundFunctionExpression>>>,
                     formalism::planning::Repository>;
template struct View<Index<formalism::planning::BinaryOperator<formalism::Sub, Data<formalism::planning::GroundFunctionExpression>>>,
                     formalism::planning::Repository>;
template struct View<Index<formalism::planning::BinaryOperator<formalism::Mul, Data<formalism::planning::GroundFunctionExpression>>>,
                     formalism::planning::Repository>;
template struct View<Index<formalism::planning::BinaryOperator<formalism::Div, Data<formalism::planning::GroundFunctionExpression>>>,
                     formalism::planning::Repository>;
template struct View<Index<formalism::planning::BinaryOperator<formalism::Eq, Data<formalism::planning::GroundFunctionExpression>>>,
                     formalism::planning::Repository>;
template struct View<Index<formalism::planning::BinaryOperator<formalism::Ne, Data<formalism::planning::GroundFunctionExpression>>>,
                     formalism::planning::Repository>;
template struct View<Index<formalism::planning::BinaryOperator<formalism::Le, Data<formalism::planning::GroundFunctionExpression>>>,
                     formalism::planning::Repository>;
template struct View<Index<formalism::planning::BinaryOperator<formalism::Lt, Data<formalism::planning::GroundFunctionExpression>>>,
                     formalism::planning::Repository>;
template struct View<Index<formalism::planning::BinaryOperator<formalism::Ge, Data<formalism::planning::GroundFunctionExpression>>>,
                     formalism::planning::Repository>;
template struct View<Index<formalism::planning::BinaryOperator<formalism::Gt, Data<formalism::planning::GroundFunctionExpression>>>,
                     formalism::planning::Repository>;

template struct View<Index<formalism::planning::MultiOperator<formalism::Add, Data<formalism::planning::GroundFunctionExpression>>>,
                     formalism::planning::Repository>;
template struct View<Index<formalism::planning::MultiOperator<formalism::Mul, Data<formalism::planning::GroundFunctionExpression>>>,
                     formalism::planning::Repository>;

// Numeric effect index views
template struct View<Index<formalism::planning::NumericEffect<formalism::Assign, formalism::FluentTag>>, formalism::planning::Repository>;
template struct View<Index<formalism::planning::NumericEffect<formalism::Increase, formalism::FluentTag>>, formalism::planning::Repository>;
template struct View<Index<formalism::planning::NumericEffect<formalism::Decrease, formalism::FluentTag>>, formalism::planning::Repository>;
template struct View<Index<formalism::planning::NumericEffect<formalism::ScaleUp, formalism::FluentTag>>, formalism::planning::Repository>;
template struct View<Index<formalism::planning::NumericEffect<formalism::ScaleDown, formalism::FluentTag>>, formalism::planning::Repository>;
template struct View<Index<formalism::planning::NumericEffect<formalism::Increase, formalism::AuxiliaryTag>>, formalism::planning::Repository>;

template struct View<Index<formalism::planning::GroundNumericEffect<formalism::Assign, formalism::FluentTag>>, formalism::planning::Repository>;
template struct View<Index<formalism::planning::GroundNumericEffect<formalism::Increase, formalism::FluentTag>>, formalism::planning::Repository>;
template struct View<Index<formalism::planning::GroundNumericEffect<formalism::Decrease, formalism::FluentTag>>, formalism::planning::Repository>;
template struct View<Index<formalism::planning::GroundNumericEffect<formalism::ScaleUp, formalism::FluentTag>>, formalism::planning::Repository>;
template struct View<Index<formalism::planning::GroundNumericEffect<formalism::ScaleDown, formalism::FluentTag>>, formalism::planning::Repository>;
template struct View<Index<formalism::planning::GroundNumericEffect<formalism::Increase, formalism::AuxiliaryTag>>, formalism::planning::Repository>;

// Data views
template struct View<Data<formalism::planning::ArithmeticOperator<Data<formalism::planning::FunctionExpression>>>, formalism::planning::Repository>;
template struct View<Data<formalism::planning::ArithmeticOperator<Data<formalism::planning::GroundFunctionExpression>>>, formalism::planning::Repository>;
template struct View<Data<formalism::planning::BooleanOperator<Data<formalism::planning::FunctionExpression>>>, formalism::planning::Repository>;
template struct View<Data<formalism::planning::BooleanOperator<Data<formalism::planning::GroundFunctionExpression>>>, formalism::planning::Repository>;
template struct View<Data<formalism::planning::FunctionExpression>, formalism::planning::Repository>;
template struct View<Data<formalism::planning::GroundFunctionExpression>, formalism::planning::Repository>;
template struct View<Data<formalism::planning::NumericEffectOperator<formalism::FluentTag>>, formalism::planning::Repository>;
template struct View<Data<formalism::planning::NumericEffectOperator<formalism::AuxiliaryTag>>, formalism::planning::Repository>;
template struct View<Data<formalism::planning::GroundNumericEffectOperator<formalism::FluentTag>>, formalism::planning::Repository>;
template struct View<Data<formalism::planning::GroundNumericEffectOperator<formalism::AuxiliaryTag>>, formalism::planning::Repository>;
template struct View<Data<formalism::Term>, formalism::planning::Repository>;
template struct View<Data<formalism::planning::FDRFact<formalism::FluentTag>>, formalism::planning::Repository>;

// Pair views
template struct View<Index<formalism::RelationBinding<formalism::Predicate<formalism::StaticTag>>>, formalism::planning::Repository>;
template struct View<Index<formalism::RelationBinding<formalism::Predicate<formalism::FluentTag>>>, formalism::planning::Repository>;
template struct View<Index<formalism::RelationBinding<formalism::Predicate<formalism::DerivedTag>>>, formalism::planning::Repository>;

template struct View<Index<formalism::RelationBinding<formalism::Function<formalism::StaticTag>>>, formalism::planning::Repository>;
template struct View<Index<formalism::RelationBinding<formalism::Function<formalism::FluentTag>>>, formalism::planning::Repository>;
template struct View<Index<formalism::RelationBinding<formalism::Function<formalism::AuxiliaryTag>>>, formalism::planning::Repository>;

template struct View<Index<formalism::RelationBinding<formalism::planning::Action>>, formalism::planning::Repository>;
template struct View<Index<formalism::RelationBinding<formalism::planning::Axiom>>, formalism::planning::Repository>;
}

#endif
