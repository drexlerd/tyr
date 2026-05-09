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

#ifndef TYR_FORMALISM_DECLARATIONS_HPP_
#define TYR_FORMALISM_DECLARATIONS_HPP_

#include "tyr/common/config.hpp"
#include "tyr/common/declarations.hpp"
#include "tyr/common/type_list.hpp"
#include "tyr/common/types.hpp"

namespace tyr::formalism
{

/**
 * Tags to distinguish predicates and downstream types
 */

struct StaticTag
{
    static constexpr auto name = "Static";
};
struct FluentTag
{
    static constexpr auto name = "Fluent";
};
struct DerivedTag
{
    static constexpr auto name = "Derived";
};
struct AuxiliaryTag
{
    static constexpr auto name = "Auxiliary";
};

template<typename T>
concept FactKind = std::same_as<T, StaticTag> || std::same_as<T, FluentTag> || std::same_as<T, DerivedTag> || std::same_as<T, AuxiliaryTag>;

using StaticFluentTags = TypeList<StaticTag, FluentTag>;
using StaticFluentDerivedTags = TypeList<StaticTag, FluentTag, DerivedTag>;
using StaticFluentAuxiliaryTags = TypeList<StaticTag, FluentTag, AuxiliaryTag>;
using FluentDerivedTags = TypeList<FluentTag, DerivedTag>;

/**
 * Tags to dispatch operators
 */

struct Eq
{
    static constexpr int kind = 0;
    auto identifying_members() const noexcept { return std::tie(kind); }
};
struct Ne
{
    static constexpr int kind = 1;
    auto identifying_members() const noexcept { return std::tie(kind); }
};
struct Le
{
    static constexpr int kind = 2;
    auto identifying_members() const noexcept { return std::tie(kind); }
};
struct Lt
{
    static constexpr int kind = 3;
    auto identifying_members() const noexcept { return std::tie(kind); }
};
struct Ge
{
    static constexpr int kind = 4;
    auto identifying_members() const noexcept { return std::tie(kind); }
};
struct Gt
{
    static constexpr int kind = 5;
    auto identifying_members() const noexcept { return std::tie(kind); }
};
struct Add
{
    static constexpr int kind = 0;
    auto identifying_members() const noexcept { return std::tie(kind); }
};
struct Sub
{
    static constexpr int kind = 1;
    auto identifying_members() const noexcept { return std::tie(kind); }
};
struct Mul
{
    static constexpr int kind = 2;
    auto identifying_members() const noexcept { return std::tie(kind); }
};
struct Div
{
    static constexpr int kind = 3;
    auto identifying_members() const noexcept { return std::tie(kind); }
};

template<typename T>
concept BooleanOpKind = std::same_as<T, Eq> || std::same_as<T, Ne> || std::same_as<T, Le> || std::same_as<T, Lt> || std::same_as<T, Ge> || std::same_as<T, Gt>;

template<typename T>
concept ArithmeticOpKind = std::same_as<T, Add> || std::same_as<T, Mul> || std::same_as<T, Div> || std::same_as<T, Sub>;

template<typename T>
concept OpKind = BooleanOpKind<T> || ArithmeticOpKind<T>;

using BooleanOpKinds = TypeList<Eq, Ne, Le, Lt, Ge, Gt>;
using ArithmeticOpKinds = TypeList<Add, Sub, Mul, Div>;
using UnaryArithmeticOpKinds = TypeList<Sub>;
using BinaryArithmeticOpKinds = TypeList<Add, Sub, Mul, Div>;
using MultiArithmeticOpKinds = TypeList<Add, Mul>;

enum class EffectFamily
{
    NONE = 0,
    ASSIGN = 1,
    INCREASE_DECREASE = 2,
    SCALE_UP_SCALE_DOWN = 3,
};

struct Assign
{
    static constexpr EffectFamily family = EffectFamily::ASSIGN;
    static constexpr int kind = 0;
    auto identifying_members() const noexcept { return std::tie(kind); }
};
struct Increase
{
    static constexpr EffectFamily family = EffectFamily::INCREASE_DECREASE;
    static constexpr int kind = 1;
    auto identifying_members() const noexcept { return std::tie(kind); }
};
struct Decrease
{
    static constexpr EffectFamily family = EffectFamily::INCREASE_DECREASE;
    static constexpr int kind = 2;
    auto identifying_members() const noexcept { return std::tie(kind); }
};
struct ScaleUp
{
    static constexpr EffectFamily family = EffectFamily::SCALE_UP_SCALE_DOWN;
    static constexpr int kind = 3;
    auto identifying_members() const noexcept { return std::tie(kind); }
};
struct ScaleDown
{
    static constexpr EffectFamily family = EffectFamily::SCALE_UP_SCALE_DOWN;
    static constexpr int kind = 4;
    auto identifying_members() const noexcept { return std::tie(kind); }
};

template<typename T>
concept NumericEffectOpKind =
    std::same_as<T, Assign> || std::same_as<T, Increase> || std::same_as<T, Decrease> || std::same_as<T, ScaleUp> || std::same_as<T, ScaleDown>;

using NumericEffectOpKinds = TypeList<Assign, Increase, Decrease, ScaleUp, ScaleDown>;

/**
 * Formalism tag
 */

struct Variable
{
};

struct Object
{
};

struct Row
{
};

template<typename T>
struct RelationBinding
{
};

template<typename T>
struct is_relation_binding : std::false_type
{
};

template<typename T>
struct is_relation_binding<RelationBinding<T>> : std::true_type
{
};

template<typename T>
inline constexpr bool is_relation_binding_v = is_relation_binding<std::remove_cvref_t<T>>::value;

template<typename T>
concept RelationBindingConcept = is_relation_binding_v<T>;

template<typename T>
concept NonRelationBindingConcept = !RelationBindingConcept<T>;

struct Term
{
};

template<FactKind T>
struct Predicate
{
};

template<FactKind T>
struct Function
{
};

struct PositiveTag
{
};

struct NegativeTag
{
};

template<typename T>
concept PolarityKind = std::same_as<T, PositiveTag> || std::same_as<T, NegativeTag>;

}

#endif
