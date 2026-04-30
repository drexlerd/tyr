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

#ifndef TYR_FORMALISM_DATALOG_DECLARATIONS_HPP_
#define TYR_FORMALISM_DATALOG_DECLARATIONS_HPP_

#include "tyr/common/config.hpp"
#include "tyr/common/declarations.hpp"
#include "tyr/common/types.hpp"
#include "tyr/formalism/declarations.hpp"

namespace tyr::formalism::datalog
{

/**
 * Formalism tag
 */

template<OpKind Op, typename T>
struct UnaryOperator
{
};

template<OpKind Op, typename T>
struct BinaryOperator
{
};

template<OpKind Op, typename T>
struct MultiOperator
{
};

template<typename T>
class BooleanOperator
{
};
template<typename T>
class ArithmeticOperator
{
};

template<FactKind T>
struct Atom
{
};

template<FactKind T>
struct Literal
{
};

template<FactKind T>
struct GroundAtom
{
};

template<FactKind T>
struct GroundLiteral
{
};

template<FactKind T>
struct FunctionTerm
{
};

struct FunctionExpression
{
};

template<FactKind T>
struct GroundFunctionTerm
{
};

struct GroundFunctionExpression
{
};

template<FactKind T>
struct GroundFunctionTermValue
{
};

struct OpAssign
{
    static constexpr int kind = 0;
    auto identifying_members() const noexcept { return std::tie(kind); }
};
struct OpIncrease
{
    static constexpr int kind = 1;
    auto identifying_members() const noexcept { return std::tie(kind); }
};
struct OpDecrease
{
    static constexpr int kind = 2;
    auto identifying_members() const noexcept { return std::tie(kind); }
};
struct OpScaleUp
{
    static constexpr int kind = 3;
    auto identifying_members() const noexcept { return std::tie(kind); }
};
struct OpScaleDown
{
    static constexpr int kind = 4;
    auto identifying_members() const noexcept { return std::tie(kind); }
};

template<typename T>
concept NumericEffectOpKind =
    std::same_as<T, OpAssign> || std::same_as<T, OpIncrease> || std::same_as<T, OpDecrease> || std::same_as<T, OpScaleUp> || std::same_as<T, OpScaleDown>;

template<NumericEffectOpKind Op, FactKind T>
struct NumericEffect
{
};
template<NumericEffectOpKind Op, FactKind T>
struct GroundNumericEffect
{
};

template<FactKind T>
struct NumericEffectOperator
{
};
template<FactKind T>
struct GroundNumericEffectOperator
{
};

struct ConjunctiveCondition
{
};

struct GroundConjunctiveCondition
{
};

struct Rule
{
};

struct GroundRule
{
};

struct Program
{
};

using CoreTypes = TypeList<Variable, Object>;
using PredicateTypes = MapTypeListT<Predicate, StaticFluentTags>;
using AtomTypes = MapTypeListT<Atom, StaticFluentTags>;
using GroundAtomTypes = MapTypeListT<GroundAtom, StaticFluentTags>;
using LiteralTypes = MapTypeListT<Literal, StaticFluentTags>;
using GroundLiteralTypes = MapTypeListT<GroundLiteral, StaticFluentTags>;
using FunctionTypes = MapTypeListT<Function, StaticFluentTags>;
using FunctionTermTypes = MapTypeListT<FunctionTerm, StaticFluentTags>;
using GroundFunctionTermTypes = MapTypeListT<GroundFunctionTerm, StaticFluentTags>;
using GroundFunctionTermValueTypes = MapTypeListT<GroundFunctionTermValue, StaticFluentTags>;
template<typename Op>
using FluentNumericEffectType = NumericEffect<Op, FluentTag>;
template<typename Op>
using GroundFluentNumericEffectType = GroundNumericEffect<Op, FluentTag>;
using NumericEffectTypes = MapTypeListT<FluentNumericEffectType, TypeList<OpAssign, OpIncrease, OpDecrease, OpScaleUp, OpScaleDown>>;
using GroundNumericEffectTypes = MapTypeListT<GroundFluentNumericEffectType, TypeList<OpAssign, OpIncrease, OpDecrease, OpScaleUp, OpScaleDown>>;
using NumericEffectOperatorTypes = TypeList<NumericEffectOperator<FluentTag>>;
using GroundNumericEffectOperatorTypes = TypeList<GroundNumericEffectOperator<FluentTag>>;

template<typename Op>
using LiftedUnaryOperatorType = UnaryOperator<Op, Data<FunctionExpression>>;

template<typename Op>
using LiftedBinaryOperatorType = BinaryOperator<Op, Data<FunctionExpression>>;

template<typename Op>
using LiftedMultiOperatorType = MultiOperator<Op, Data<FunctionExpression>>;

template<typename Op>
using GroundUnaryOperatorType = UnaryOperator<Op, Data<GroundFunctionExpression>>;

template<typename Op>
using GroundBinaryOperatorType = BinaryOperator<Op, Data<GroundFunctionExpression>>;

template<typename Op>
using GroundMultiOperatorType = MultiOperator<Op, Data<GroundFunctionExpression>>;

using LiftedArithmeticExpressionTypes = ConcatTypeListsT<MapTypeListT<LiftedUnaryOperatorType, UnaryArithmeticOpKinds>,
                                                        MapTypeListT<LiftedBinaryOperatorType, BinaryArithmeticOpKinds>,
                                                        MapTypeListT<LiftedMultiOperatorType, MultiArithmeticOpKinds>>;

using LiftedBooleanExpressionTypes = MapTypeListT<LiftedBinaryOperatorType, BooleanOpKinds>;

using GroundArithmeticExpressionTypes = ConcatTypeListsT<MapTypeListT<GroundUnaryOperatorType, UnaryArithmeticOpKinds>,
                                                        MapTypeListT<GroundBinaryOperatorType, BinaryArithmeticOpKinds>,
                                                        MapTypeListT<GroundMultiOperatorType, MultiArithmeticOpKinds>>;

using GroundBooleanExpressionTypes = MapTypeListT<GroundBinaryOperatorType, BooleanOpKinds>;

using ExpressionTypes = ConcatTypeListsT<LiftedArithmeticExpressionTypes, LiftedBooleanExpressionTypes, GroundArithmeticExpressionTypes, GroundBooleanExpressionTypes>;
using EffectTypes = ConcatTypeListsT<NumericEffectTypes, GroundNumericEffectTypes>;
using CompoundTypes = TypeList<ConjunctiveCondition, Rule, GroundConjunctiveCondition, GroundRule, Program>;

using SymbolRepositoryTypes = ConcatTypeListsT<CoreTypes,
                                               PredicateTypes,
                                               AtomTypes,
                                               GroundAtomTypes,
                                               LiteralTypes,
                                               GroundLiteralTypes,
                                               FunctionTypes,
                                               FunctionTermTypes,
                                               GroundFunctionTermTypes,
                                               GroundFunctionTermValueTypes,
                                               ExpressionTypes,
                                               EffectTypes,
                                               CompoundTypes>;

using RelationRepositoryTypes = ConcatTypeListsT<PredicateTypes, FunctionTypes, TypeList<Rule>>;
using BuilderTypes = ConcatTypeListsT<SymbolRepositoryTypes, MapTypeListT<RelationBinding, RelationRepositoryTypes>>;

/**
 * Context
 */

template<typename Repo, typename Tag>
concept RepositoryAccess = requires(const Repo& r, Index<Tag> idx) {
    requires CanonicalizableContext<Index<Tag>, Repo>;
    { r[idx] } -> std::same_as<const Data<Tag>&>;
};

template<typename Repo, typename... Tags>
constexpr bool repository_access_for_types(TypeList<Tags...>) noexcept
{
    return (RepositoryAccess<Repo, Tags> && ...);
}

template<typename T>
concept RepositoryConcept = repository_access_for_types<T>(SymbolRepositoryTypes {}) && repository_access_for_types<T>(RelationRepositoryTypes {});

template<typename T>
    requires RepositoryConcept<T>
inline const T& get_repository(const T& context) noexcept
{
    return context;
}

template<typename T>
concept Context = requires(const T& a) {
    { get_repository(a) } -> RepositoryConcept;
};

struct GrounderContext;
struct MergeContext;

}

#endif
