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

#ifndef TYR_FORMALISM_PLANNING_DECLARATIONS_HPP_
#define TYR_FORMALISM_PLANNING_DECLARATIONS_HPP_

#include "tyr/common/config.hpp"
#include "tyr/common/declarations.hpp"
#include "tyr/common/types.hpp"
#include "tyr/formalism/declarations.hpp"

namespace tyr::formalism::planning
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

using EffectFamilyList = std::vector<EffectFamily>;

inline bool is_compatible_effect_family(EffectFamily lhs, EffectFamily rhs)
{
    if (lhs == EffectFamily::NONE || rhs == EffectFamily::NONE)
        return true;  ///< first effect

    if (lhs == rhs)
        return lhs != EffectFamily::ASSIGN;  ///< disallow double assignment.

    return false;  ///< disallow mixing assign, additive, or multiplicative
}

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

struct ConditionalEffect
{
};
struct GroundConditionalEffect
{
};

struct ConjunctiveEffect
{
};
struct GroundConjunctiveEffect
{
};

struct Action
{
};
struct GroundAction
{
};

struct Axiom
{
};
struct GroundAxiom
{
};

struct Minimize
{
    static constexpr int kind = 0;
    auto identifying_members() const noexcept { return std::tie(kind); }
};
struct Maximize
{
    static constexpr int kind = 1;
    auto identifying_members() const noexcept { return std::tie(kind); }
};

template<typename T>
concept ObjectiveKind = std::same_as<T, Minimize> || std::same_as<T, Maximize>;

struct Metric
{
};

struct Task
{
};

struct Domain
{
};

template<FactKind T>
struct FDRVariable
{
};

template<FactKind T>
struct FDRFact
{
};

struct ConjunctiveCondition
{
};

struct GroundConjunctiveCondition
{
};

struct FDRAction
{
};

struct FDRAxiom
{
};

struct FDRTask
{
};

using CoreTypes = TypeList<Variable, Object>;
using PredicateTypes = MapTypeListT<Predicate, StaticFluentDerivedTags>;
using AtomTypes = MapTypeListT<Atom, StaticFluentDerivedTags>;
using GroundAtomTypes = MapTypeListT<GroundAtom, StaticFluentDerivedTags>;
using LiteralTypes = MapTypeListT<Literal, StaticFluentDerivedTags>;
using GroundLiteralTypes = MapTypeListT<GroundLiteral, StaticFluentDerivedTags>;
using FunctionTypes = MapTypeListT<Function, StaticFluentAuxiliaryTags>;
using FunctionTermTypes = MapTypeListT<FunctionTerm, StaticFluentAuxiliaryTags>;
using GroundFunctionTermTypes = MapTypeListT<GroundFunctionTerm, StaticFluentAuxiliaryTags>;
using GroundFunctionTermValueTypes = MapTypeListT<GroundFunctionTermValue, StaticFluentAuxiliaryTags>;
using FDRVariableTypes = MapTypeListT<FDRVariable, TypeList<FluentTag>>;
using FDRFactTypes = MapTypeListT<FDRFact, TypeList<FluentTag>>;

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

using ExpressionTypes =
    ConcatTypeListsT<LiftedArithmeticExpressionTypes, LiftedBooleanExpressionTypes, GroundArithmeticExpressionTypes, GroundBooleanExpressionTypes>;

using NumericEffectTypes = TypeList<NumericEffect<Assign, FluentTag>,
                                    NumericEffect<Increase, FluentTag>,
                                    NumericEffect<Decrease, FluentTag>,
                                    NumericEffect<ScaleUp, FluentTag>,
                                    NumericEffect<ScaleDown, FluentTag>,
                                    NumericEffect<Increase, AuxiliaryTag>>;

using GroundNumericEffectTypes = TypeList<GroundNumericEffect<Assign, FluentTag>,
                                          GroundNumericEffect<Increase, FluentTag>,
                                          GroundNumericEffect<Decrease, FluentTag>,
                                          GroundNumericEffect<ScaleUp, FluentTag>,
                                          GroundNumericEffect<ScaleDown, FluentTag>,
                                          GroundNumericEffect<Increase, AuxiliaryTag>>;

using NumericEffectOperatorTypes = TypeList<NumericEffectOperator<FluentTag>, NumericEffectOperator<AuxiliaryTag>>;
using GroundNumericEffectOperatorTypes = TypeList<GroundNumericEffectOperator<FluentTag>, GroundNumericEffectOperator<AuxiliaryTag>>;
using EffectTypes = ConcatTypeListsT<NumericEffectTypes, GroundNumericEffectTypes, NumericEffectOperatorTypes, GroundNumericEffectOperatorTypes>;
using OperatorEffectTypes = ConcatTypeListsT<NumericEffectTypes, GroundNumericEffectTypes>;
using ControlTypes = TypeList<ConditionalEffect, GroundConditionalEffect, ConjunctiveEffect, GroundConjunctiveEffect, Action, GroundAction, Axiom, GroundAxiom>;
using StructureTypes = TypeList<Action, Axiom>;
using ProblemTypes = TypeList<Metric, Domain, Task, FDRTask>;
using ConditionTypes = TypeList<ConjunctiveCondition, GroundConjunctiveCondition>;

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
                                               OperatorEffectTypes,
                                               ControlTypes,
                                               ProblemTypes,
                                               FDRVariableTypes,
                                               ConditionTypes>;

using RelationRepositoryTypes = ConcatTypeListsT<PredicateTypes, FunctionTypes, StructureTypes>;
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

class FDRContext;
using FDRContextPtr = std::shared_ptr<FDRContext>;

}

#endif
