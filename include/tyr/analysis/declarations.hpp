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

#ifndef TYR_ANALYSIS_DECLARATIONS_HPP_
#define TYR_ANALYSIS_DECLARATIONS_HPP_

#include "tyr/common/declarations.hpp"
#include "tyr/common/equal_to.hpp"
#include "tyr/common/hash.hpp"
#include "tyr/formalism/datalog/indices.hpp"
#include "tyr/formalism/datalog/repository.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/object_index.hpp"
#include "tyr/formalism/planning/indices.hpp"
#include "tyr/formalism/planning/repository.hpp"

#include <vector>

namespace tyr::analysis
{

/**
 * Index based internal representation of variable domains.
 */

template<typename Element, typename Payload>
struct Scoped
{
    Index<Element> element;
    Payload payload;
};

struct VariableDomain
{
    std::vector<Index<formalism::Object>> objects;

    auto begin() noexcept { return objects.begin(); }
    auto end() noexcept { return objects.end(); }
    auto begin() const noexcept { return objects.begin(); }
    auto end() const noexcept { return objects.end(); }

    auto size() const noexcept { return objects.size(); }
    bool empty() const noexcept { return objects.empty(); }

    auto& operator[](std::size_t i) noexcept { return objects[i]; }
    const auto& operator[](std::size_t i) const noexcept { return objects[i]; }
};

using VariableDomainList = std::vector<VariableDomain>;

template<typename Element>
using SimpleScopedDomain = Scoped<Element, VariableDomainList>;

template<typename Element>
using SimpleScopedDomainMap = UnorderedMap<Index<Element>, VariableDomainList>;

template<typename Element>
using ScopedDomainMap = UnorderedMap<Index<Element>, SimpleScopedDomain<Element>>;

template<formalism::FactKind T>
using PredicateDomainMap = SimpleScopedDomainMap<formalism::Predicate<T>>;

template<formalism::FactKind T>
using FunctionDomainMap = SimpleScopedDomainMap<formalism::Function<T>>;

using RuleDomainMap = ScopedDomainMap<formalism::datalog::Rule>;

using AxiomDomainMap = ScopedDomainMap<formalism::planning::Axiom>;

using ConjunctiveConditionDomain = SimpleScopedDomain<formalism::planning::ConjunctiveCondition>;

using ConjunctiveEffectDomain = SimpleScopedDomain<formalism::planning::ConjunctiveEffect>;

struct ConditionalEffectDomainData
{
    ConjunctiveConditionDomain condition_domain;
    ConjunctiveEffectDomain effect_domain;
};

using ConditionalEffectDomain = Scoped<formalism::planning::ConditionalEffect, ConditionalEffectDomainData>;

using ConditionalEffectDomainMap = UnorderedMap<Index<formalism::planning::ConditionalEffect>, ConditionalEffectDomain>;

struct ActionDomainData
{
    ConjunctiveConditionDomain precondition_domain;
    ConditionalEffectDomainMap effect_domains;
};

using ActionDomain = Scoped<formalism::planning::Action, ActionDomainData>;

using ActionDomainMap = UnorderedMap<Index<formalism::planning::Action>, ActionDomain>;

struct ProgramVariableDomains
{
    PredicateDomainMap<formalism::StaticTag> static_predicate_domains;
    PredicateDomainMap<formalism::FluentTag> fluent_predicate_domains;
    FunctionDomainMap<formalism::StaticTag> static_function_domains;
    FunctionDomainMap<formalism::FluentTag> fluent_function_domains;
    RuleDomainMap rule_domains;
};

struct TaskVariableDomains
{
    PredicateDomainMap<formalism::StaticTag> static_predicate_domains;
    PredicateDomainMap<formalism::FluentTag> fluent_predicate_domains;
    PredicateDomainMap<formalism::DerivedTag> derived_predicate_domains;
    FunctionDomainMap<formalism::StaticTag> static_function_domains;
    FunctionDomainMap<formalism::FluentTag> fluent_function_domains;
    ActionDomainMap action_domains;
    AxiomDomainMap axiom_domains;
};

/**
 * View based external representation of variable domains.
 */

template<typename Element, typename Payload, typename C>
struct ScopedView
{
    View<Index<Element>, C> element;
    Payload payload;
};

template<typename C>
struct VariableDomainView
{
    std::vector<View<Index<formalism::Object>, C>> objects;

    auto begin() noexcept { return objects.begin(); }
    auto end() noexcept { return objects.end(); }
    auto begin() const noexcept { return objects.begin(); }
    auto end() const noexcept { return objects.end(); }

    auto size() const noexcept { return objects.size(); }
    bool empty() const noexcept { return objects.empty(); }

    auto& operator[](std::size_t i) noexcept { return objects[i]; }
    const auto& operator[](std::size_t i) const noexcept { return objects[i]; }
};

template<typename C>
using VariableDomainViewList = std::vector<VariableDomainView<C>>;

template<typename Element, typename C>
using SimpleScopedDomainView = ScopedView<Element, VariableDomainViewList<C>, C>;

template<typename Element, typename C>
using SimpleScopedDomainViewMap = UnorderedMap<View<Index<Element>, C>, VariableDomainViewList<C>>;

template<typename Element, typename C>
using ScopedDomainViewMap = UnorderedMap<View<Index<Element>, C>, SimpleScopedDomainView<Element, C>>;

template<formalism::FactKind T, typename C>
using PredicateDomainViewMap = SimpleScopedDomainViewMap<formalism::Predicate<T>, C>;

template<formalism::FactKind T, typename C>
using FunctionDomainViewMap = SimpleScopedDomainViewMap<formalism::Function<T>, C>;

template<typename C>
using RuleDomainViewMap = ScopedDomainViewMap<formalism::datalog::Rule, C>;

template<typename C>
using AxiomDomainViewMap = ScopedDomainViewMap<formalism::planning::Axiom, C>;

template<typename C>
using ConjunctiveConditionDomainView = SimpleScopedDomainView<formalism::planning::ConjunctiveCondition, C>;

template<typename C>
using ConjunctiveEffectDomainView = SimpleScopedDomainView<formalism::planning::ConjunctiveEffect, C>;

template<typename C>
struct ConditionalEffectDomainViewData
{
    ConjunctiveConditionDomainView<C> condition_domain;
    ConjunctiveEffectDomainView<C> effect_domain;
};

template<typename C>
using ConditionalEffectDomainView = ScopedView<formalism::planning::ConditionalEffect, ConditionalEffectDomainViewData<C>, C>;

template<typename C>
using ConditionalEffectDomainViewMap = UnorderedMap<View<Index<formalism::planning::ConditionalEffect>, C>, ConditionalEffectDomainView<C>>;

template<typename C>
struct ActionDomainViewData
{
    ConjunctiveConditionDomainView<C> precondition_domain;
    ConditionalEffectDomainViewMap<C> effect_domains;
};

template<typename C>
using ActionDomainView = ScopedView<formalism::planning::Action, ActionDomainViewData<C>, C>;

template<typename C>
using ActionDomainViewMap = UnorderedMap<View<Index<formalism::planning::Action>, C>, ActionDomainView<C>>;

struct ProgramVariableDomainsView
{
    using C = formalism::datalog::Repository;

    PredicateDomainViewMap<formalism::StaticTag, C> static_predicate_domains;
    PredicateDomainViewMap<formalism::FluentTag, C> fluent_predicate_domains;
    FunctionDomainViewMap<formalism::StaticTag, C> static_function_domains;
    FunctionDomainViewMap<formalism::FluentTag, C> fluent_function_domains;
    RuleDomainViewMap<C> rule_domains;
};

struct TaskVariableDomainsView
{
    using C = formalism::planning::Repository;

    PredicateDomainViewMap<formalism::StaticTag, C> static_predicate_domains;
    PredicateDomainViewMap<formalism::FluentTag, C> fluent_predicate_domains;
    PredicateDomainViewMap<formalism::DerivedTag, C> derived_predicate_domains;
    FunctionDomainViewMap<formalism::StaticTag, C> static_function_domains;
    FunctionDomainViewMap<formalism::FluentTag, C> fluent_function_domains;
    ActionDomainViewMap<C> action_domains;
    AxiomDomainViewMap<C> axiom_domains;
};

/**
 * Stratification
 */

struct RuleStrata;
}

#endif
