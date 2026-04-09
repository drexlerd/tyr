/*
 * Copyright (C) 2025-2026 Dominik Drexler
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#ifndef TYR_ANALYSIS_FORMATTER_HPP_
#define TYR_ANALYSIS_FORMATTER_HPP_

#include "tyr/analysis/declarations.hpp"
#include "tyr/common/formatter.hpp"
#include "tyr/common/iostream.hpp"
#include "tyr/formalism/datalog/formatter.hpp"
#include "tyr/formalism/planning/formatter.hpp"

#include <algorithm>
#include <fmt/core.h>
#include <fmt/ostream.h>
#include <fmt/ranges.h>
#include <ostream>
#include <vector>

namespace tyr
{
namespace analysis
{
inline std::ostream& operator<<(std::ostream& os, const VariableDomain& el);

template<typename Element, typename Payload>
inline std::ostream& operator<<(std::ostream& os, const Scoped<Element, Payload>& el);

inline std::ostream& operator<<(std::ostream& os, const ConditionalEffectDomain& el);
inline std::ostream& operator<<(std::ostream& os, const ActionDomain& el);
inline std::ostream& operator<<(std::ostream& os, const ProgramVariableDomains& el);
inline std::ostream& operator<<(std::ostream& os, const TaskVariableDomains& el);

template<typename C>
inline std::ostream& operator<<(std::ostream& os, const VariableDomainView<C>& el);

template<typename Element, typename Payload, typename C>
inline std::ostream& operator<<(std::ostream& os, const ScopedView<Element, Payload, C>& el);

template<typename C>
inline std::ostream& operator<<(std::ostream& os, const ConditionalEffectDomainView<C>& el);

template<typename C>
inline std::ostream& operator<<(std::ostream& os, const ActionDomainView<C>& el);

inline std::ostream& operator<<(std::ostream& os, const ProgramVariableDomainsView& el);
inline std::ostream& operator<<(std::ostream& os, const TaskVariableDomainsView& el);
}  // namespace analysis

inline std::ostream& print(std::ostream& os, const analysis::VariableDomain& el)
{
    os << el.objects;
    return os;
}

template<typename Element, typename Payload>
inline std::ostream& print(std::ostream& os, const analysis::Scoped<Element, Payload>& el)
{
    os << "ElementDomain(\n";
    {
        IndentScope scope(os);

        os << print_indent << "element = " << el.element << "\n";

        os << print_indent << "payload = " << el.payload << "\n";
    }
    os << print_indent << ")";
    return os;
}

template<typename Element, typename Payload>
inline std::ostream& print(std::ostream& os, const analysis::Scoped<formalism::planning::Axiom, Payload>& el)
{
    os << "AxiomDomain(\n";
    {
        IndentScope scope(os);

        os << print_indent << "element = " << el.element << "\n";

        os << print_indent << "payload = " << el.payload << "\n";
    }
    os << print_indent << ")";
    return os;
}

template<typename Element, typename Payload>
inline std::ostream& print(std::ostream& os, const analysis::Scoped<formalism::datalog::Rule, Payload>& el)
{
    os << "RuleDomain(\n";
    {
        IndentScope scope(os);

        os << print_indent << "element = " << el.element << "\n";

        os << print_indent << "payload = " << el.payload << "\n";
    }
    os << print_indent << ")";
    return os;
}

inline std::ostream& print(std::ostream& os, const analysis::ConditionalEffectDomain& el)
{
    os << "ConditionalEffectDomain(\n";
    {
        IndentScope scope(os);

        os << print_indent << "element = " << el.element << "\n";

        os << print_indent << "condition domain = " << el.payload.condition_domain << "\n";

        os << print_indent << "effect domain = " << el.payload.effect_domain << "\n";
    }
    os << print_indent << ")";
    return os;
}

inline std::ostream& print(std::ostream& os, const analysis::ActionDomain& el)
{
    os << "ActionDomain(\n";
    {
        IndentScope scope(os);

        os << print_indent << "element = " << el.element << "\n";

        os << print_indent << "precondition domain = " << el.payload.precondition_domain << "\n";

        os << print_indent << "effect domains = " << el.payload.effect_domains << "\n";
    }
    os << print_indent << ")";
    return os;
}

inline std::ostream& print(std::ostream& os, const analysis::ProgramVariableDomains& el)
{
    os << "ProgramVariableDomains(\n";
    {
        IndentScope scope(os);

        os << print_indent << "static predicate domains = " << el.static_predicate_domains << "\n";

        os << print_indent << "fluent predicate domains = " << el.fluent_predicate_domains << "\n";

        os << print_indent << "static function domains = " << el.static_function_domains << "\n";

        os << print_indent << "fluent function domains = " << el.fluent_function_domains << "\n";

        for (const auto& [rule, domain] : el.rule_domains)
            os << print_indent << "rule " << rule << " domain = " << domain << "\n";
    }
    os << print_indent << ")";
    return os;
}

inline std::ostream& print(std::ostream& os, const analysis::TaskVariableDomains& el)
{
    os << "TaskVariableDomains(\n";
    {
        IndentScope scope(os);

        os << print_indent << "static predicate domains = " << el.static_predicate_domains << "\n";

        os << print_indent << "fluent predicate domains = " << el.fluent_predicate_domains << "\n";

        os << print_indent << "derived predicate domains = " << el.derived_predicate_domains << "\n";

        os << print_indent << "static function domains = " << el.static_function_domains << "\n";

        os << print_indent << "fluent function domains = " << el.fluent_function_domains << "\n";

        for (const auto& [action, domain] : el.action_domains)
            os << print_indent << "action " << action << " domain = " << domain << "\n";

        for (const auto& [axiom, domain] : el.axiom_domains)
            os << print_indent << "axiom " << axiom << " domain = " << domain << "\n";
    }
    os << print_indent << ")";
    return os;
}

template<typename C>
inline std::ostream& print(std::ostream& os, const analysis::VariableDomainView<C>& el)
{
    os << el.objects;
    return os;
}

template<typename Element, typename Payload, typename C>
inline std::ostream& print(std::ostream& os, const analysis::ScopedView<Element, Payload, C>& el)
{
    os << "ElementDomain(\n";
    {
        IndentScope scope(os);

        os << print_indent << "element = " << el.element.get_index() << "\n";

        os << print_indent << "payload = " << el.payload << "\n";
    }
    os << print_indent << ")";
    return os;
}

template<typename Payload, typename C>
inline std::ostream& print(std::ostream& os, const analysis::ScopedView<formalism::planning::ConjunctiveCondition, Payload, C>& el)
{
    os << "ConjunctiveConditionDomain(\n";
    {
        IndentScope scope(os);

        os << print_indent << "element = " << el.element.get_index() << "\n";

        os << print_indent << "payload = " << el.payload << "\n";
    }
    os << print_indent << ")";
    return os;
}

template<typename Payload, typename C>
inline std::ostream& print(std::ostream& os, const analysis::ScopedView<formalism::planning::ConjunctiveEffect, Payload, C>& el)
{
    os << "ConjunctiveEffectDomain(\n";
    {
        IndentScope scope(os);

        os << print_indent << "element = " << el.element.get_index() << "\n";

        os << print_indent << "payload = " << el.payload << "\n";
    }
    os << print_indent << ")";
    return os;
}

template<typename Payload, typename C>
inline std::ostream& print(std::ostream& os, const analysis::ScopedView<formalism::planning::Axiom, Payload, C>& el)
{
    os << "AxiomDomain(\n";
    {
        IndentScope scope(os);

        os << print_indent << "element = " << el.element.get_index() << "\n";

        os << print_indent << "payload = " << el.payload << "\n";
    }
    os << print_indent << ")";
    return os;
}

template<typename Payload, typename C>
inline std::ostream& print(std::ostream& os, const analysis::ScopedView<formalism::datalog::Rule, Payload, C>& el)
{
    os << "RuleDomain(\n";
    {
        IndentScope scope(os);

        os << print_indent << "element = " << el.element.get_index() << "\n";

        os << print_indent << "payload = " << el.payload << "\n";
    }
    os << print_indent << ")";
    return os;
}

template<typename C>
inline std::ostream& print(std::ostream& os, const analysis::ConditionalEffectDomainView<C>& el)
{
    os << "ConditionalEffectDomain(\n";
    {
        IndentScope scope(os);

        os << print_indent << "element = " << el.element.get_index() << "\n";

        os << print_indent << el.payload.condition_domain << "\n";

        os << print_indent << el.payload.effect_domain << "\n";
    }
    os << print_indent << ")";
    return os;
}

template<typename C>
inline std::ostream& print(std::ostream& os, const analysis::ActionDomainView<C>& el)
{
    os << "ActionDomain(\n";
    {
        IndentScope scope(os);

        os << print_indent << "element = " << el.element.get_index() << "\n";

        os << print_indent << el.payload.precondition_domain << "\n";

        for (const auto& [conditional_effect, domain] : el.payload.effect_domains)
            os << print_indent << domain << "\n";
    }
    os << print_indent << ")";
    return os;
}

inline std::ostream& print(std::ostream& os, const analysis::ProgramVariableDomainsView& el)
{
    os << "ProgramVariableDomains(\n";
    {
        IndentScope scope(os);

        os << print_indent << "static predicate domains = " << el.static_predicate_domains << "\n";

        os << print_indent << "fluent predicate domains = " << el.fluent_predicate_domains << "\n";

        os << print_indent << "static function domains = " << el.static_function_domains << "\n";

        os << print_indent << "fluent function domains = " << el.fluent_function_domains << "\n";

        for (const auto& [rule, domain] : el.rule_domains)
            os << print_indent << domain << "\n";
    }
    os << print_indent << ")";
    return os;
}

inline std::ostream& print(std::ostream& os, const analysis::TaskVariableDomainsView& el)
{
    os << "TaskVariableDomains(\n";
    {
        IndentScope scope(os);

        os << print_indent << "static predicate domains = " << el.static_predicate_domains << "\n";

        os << print_indent << "fluent predicate domains = " << el.fluent_predicate_domains << "\n";

        os << print_indent << "derived predicate domains = " << el.derived_predicate_domains << "\n";

        os << print_indent << "static function domains = " << el.static_function_domains << "\n";

        os << print_indent << "fluent function domains = " << el.fluent_function_domains << "\n";

        for (const auto& [action, domain] : el.action_domains)
            os << print_indent << domain << "\n";

        for (const auto& [axiom, domain] : el.axiom_domains)
            os << print_indent << domain << "\n";
    }
    os << print_indent << ")";
    return os;
}

namespace analysis
{
inline std::ostream& operator<<(std::ostream& os, const VariableDomain& el) { return tyr::print(os, el); }

template<typename Element, typename Payload>
inline std::ostream& operator<<(std::ostream& os, const Scoped<Element, Payload>& el)
{
    return tyr::print(os, el);
}

inline std::ostream& operator<<(std::ostream& os, const ConditionalEffectDomain& el) { return tyr::print(os, el); }

inline std::ostream& operator<<(std::ostream& os, const ActionDomain& el) { return tyr::print(os, el); }

inline std::ostream& operator<<(std::ostream& os, const ProgramVariableDomains& el) { return tyr::print(os, el); }

inline std::ostream& operator<<(std::ostream& os, const TaskVariableDomains& el) { return tyr::print(os, el); }

template<typename C>
inline std::ostream& operator<<(std::ostream& os, const VariableDomainView<C>& el)
{
    return tyr::print(os, el);
}

template<typename Element, typename Payload, typename C>
inline std::ostream& operator<<(std::ostream& os, const ScopedView<Element, Payload, C>& el)
{
    return tyr::print(os, el);
}

template<typename C>
inline std::ostream& operator<<(std::ostream& os, const ConditionalEffectDomainView<C>& el)
{
    return tyr::print(os, el);
}

template<typename C>
inline std::ostream& operator<<(std::ostream& os, const ActionDomainView<C>& el)
{
    return tyr::print(os, el);
}

inline std::ostream& operator<<(std::ostream& os, const ProgramVariableDomainsView& el) { return tyr::print(os, el); }

inline std::ostream& operator<<(std::ostream& os, const TaskVariableDomainsView& el) { return tyr::print(os, el); }

}  // namespace analysis
}  // namespace tyr

#endif