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
#include <sstream>
#include <vector>

namespace fmt
{

template<>
struct formatter<tyr::analysis::VariableDomain, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::analysis::VariableDomain& value, FormatContext& ctx) const
    {
        return fmt::format_to(ctx.out(), "{}", value.objects);
    }
};

template<typename Element, typename Payload>
struct formatter<tyr::analysis::Scoped<Element, Payload>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::analysis::Scoped<Element, Payload>& value, FormatContext& ctx) const
    {
        auto os = std::stringstream {};
        os << "ElementDomain(\n";
        {
            tyr::IndentScope scope(os);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "element = ", value.element);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "payload = ", value.payload);
        }
        os << tyr::print_indent << ")";
        return fmt::format_to(ctx.out(), "{}", os.str());
    }
};

template<typename Payload>
struct formatter<tyr::analysis::Scoped<tyr::formalism::planning::Axiom, Payload>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::analysis::Scoped<tyr::formalism::planning::Axiom, Payload>& value, FormatContext& ctx) const
    {
        auto os = std::stringstream {};
        os << "AxiomDomain(\n";
        {
            tyr::IndentScope scope(os);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "element = ", value.element);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "payload = ", value.payload);
        }
        os << tyr::print_indent << ")";
        return fmt::format_to(ctx.out(), "{}", os.str());
    }
};

template<typename Payload>
struct formatter<tyr::analysis::Scoped<tyr::formalism::datalog::Rule, Payload>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::analysis::Scoped<tyr::formalism::datalog::Rule, Payload>& value, FormatContext& ctx) const
    {
        auto os = std::stringstream {};
        os << "RuleDomain(\n";
        {
            tyr::IndentScope scope(os);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "element = ", value.element);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "payload = ", value.payload);
        }
        os << tyr::print_indent << ")";
        return fmt::format_to(ctx.out(), "{}", os.str());
    }
};

template<>
struct formatter<tyr::analysis::ConditionalEffectDomain, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::analysis::ConditionalEffectDomain& value, FormatContext& ctx) const
    {
        auto os = std::stringstream {};
        os << "ConditionalEffectDomain(\n";
        {
            tyr::IndentScope scope(os);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "element = ", value.element);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "condition domain = ", value.payload.condition_domain);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "effect domain = ", value.payload.effect_domain);
        }
        os << tyr::print_indent << ")";
        return fmt::format_to(ctx.out(), "{}", os.str());
    }
};

template<>
struct formatter<tyr::analysis::ActionDomain, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::analysis::ActionDomain& value, FormatContext& ctx) const
    {
        auto os = std::stringstream {};
        os << "ActionDomain(\n";
        {
            tyr::IndentScope scope(os);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "element = ", value.element);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "precondition domain = ", value.payload.precondition_domain);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "effect domains = ", value.payload.effect_domains);
        }
        os << tyr::print_indent << ")";
        return fmt::format_to(ctx.out(), "{}", os.str());
    }
};

template<>
struct formatter<tyr::analysis::ConditionalEffectDomainData, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::analysis::ConditionalEffectDomainData& value, FormatContext& ctx) const
    {
        auto os = std::stringstream {};
        os << "ConditionalEffectDomainData(\n";
        {
            tyr::IndentScope scope(os);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "condition domain = ", value.condition_domain);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "effect domain = ", value.effect_domain);
        }
        os << tyr::print_indent << ")";
        return fmt::format_to(ctx.out(), "{}", os.str());
    }
};

template<>
struct formatter<tyr::analysis::ActionDomainData, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::analysis::ActionDomainData& value, FormatContext& ctx) const
    {
        auto os = std::stringstream {};
        os << "ActionDomainData(\n";
        {
            tyr::IndentScope scope(os);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "precondition domain = ", value.precondition_domain);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "effect domains = ", value.effect_domains);
        }
        os << tyr::print_indent << ")";
        return fmt::format_to(ctx.out(), "{}", os.str());
    }
};

template<>
struct formatter<tyr::analysis::ProgramVariableDomains, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::analysis::ProgramVariableDomains& value, FormatContext& ctx) const
    {
        auto os = std::stringstream {};
        os << "ProgramVariableDomains(\n";
        {
            tyr::IndentScope scope(os);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "static predicate domains = ", value.static_predicate_domains);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "fluent predicate domains = ", value.fluent_predicate_domains);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "static function domains = ", value.static_function_domains);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "fluent function domains = ", value.fluent_function_domains);

            for (const auto& [rule, domain] : value.rule_domains)
            {
                os << tyr::print_indent;
                fmt::print(os, "rule {} domain = {}\n", rule, domain);
            }
        }
        os << tyr::print_indent << ")";
        return fmt::format_to(ctx.out(), "{}", os.str());
    }
};

template<>
struct formatter<tyr::analysis::TaskVariableDomains, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::analysis::TaskVariableDomains& value, FormatContext& ctx) const
    {
        auto os = std::stringstream {};
        os << "TaskVariableDomains(\n";
        {
            tyr::IndentScope scope(os);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "static predicate domains = ", value.static_predicate_domains);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "fluent predicate domains = ", value.fluent_predicate_domains);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "derived predicate domains = ", value.derived_predicate_domains);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "static function domains = ", value.static_function_domains);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "fluent function domains = ", value.fluent_function_domains);

            for (const auto& [action, domain] : value.action_domains)
            {
                os << tyr::print_indent;
                fmt::print(os, "action {} domain = {}\n", action, domain);
            }

            for (const auto& [axiom, domain] : value.axiom_domains)
            {
                os << tyr::print_indent;
                fmt::print(os, "axiom {} domain = {}\n", axiom, domain);
            }
        }
        os << tyr::print_indent << ")";
        return fmt::format_to(ctx.out(), "{}", os.str());
    }
};

template<typename C>
struct formatter<tyr::analysis::ScopedView<tyr::formalism::planning::ConditionalEffect, tyr::analysis::ConditionalEffectDomainViewData<C>, C>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::analysis::ScopedView<tyr::formalism::planning::ConditionalEffect, tyr::analysis::ConditionalEffectDomainViewData<C>, C>& value,
                FormatContext& ctx) const
    {
        auto os = std::stringstream {};
        os << "ConditionalEffectDomain(\n";
        {
            tyr::IndentScope scope(os);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "element = ", value.element.get_index());
            os << tyr::print_indent;
            fmt::print(os, "{}\n", value.payload.condition_domain);
            os << tyr::print_indent;
            fmt::print(os, "{}\n", value.payload.effect_domain);
        }
        os << tyr::print_indent << ")";
        return fmt::format_to(ctx.out(), "{}", os.str());
    }
};

template<typename C>
struct formatter<tyr::analysis::ScopedView<tyr::formalism::planning::Action, tyr::analysis::ActionDomainViewData<C>, C>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::analysis::ScopedView<tyr::formalism::planning::Action, tyr::analysis::ActionDomainViewData<C>, C>& value, FormatContext& ctx) const
    {
        auto os = std::stringstream {};
        os << "ActionDomain(\n";
        {
            tyr::IndentScope scope(os);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "element = ", value.element.get_index());
            os << tyr::print_indent;
            fmt::print(os, "{}\n", value.payload.precondition_domain);

            for (const auto& [conditional_effect, domain] : value.payload.effect_domains)
            {
                os << tyr::print_indent;
                fmt::print(os, "{}\n", domain);
            }
        }
        os << tyr::print_indent << ")";
        return fmt::format_to(ctx.out(), "{}", os.str());
    }
};

template<typename Element, typename Payload, typename C>
struct formatter<tyr::analysis::ScopedView<Element, Payload, C>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::analysis::ScopedView<Element, Payload, C>& value, FormatContext& ctx) const
    {
        auto os = std::stringstream {};
        os << "ElementDomain(\n";
        {
            tyr::IndentScope scope(os);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "element = ", value.element.get_index());
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "payload = ", value.payload);
        }
        os << tyr::print_indent << ")";
        return fmt::format_to(ctx.out(), "{}", os.str());
    }
};

template<typename Payload, typename C>
struct formatter<tyr::analysis::ScopedView<tyr::formalism::planning::ConjunctiveCondition, Payload, C>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::analysis::ScopedView<tyr::formalism::planning::ConjunctiveCondition, Payload, C>& value, FormatContext& ctx) const
    {
        auto os = std::stringstream {};
        os << "ConjunctiveConditionDomain(\n";
        {
            tyr::IndentScope scope(os);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "element = ", value.element.get_index());
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "payload = ", value.payload);
        }
        os << tyr::print_indent << ")";
        return fmt::format_to(ctx.out(), "{}", os.str());
    }
};

template<typename Payload, typename C>
struct formatter<tyr::analysis::ScopedView<tyr::formalism::planning::ConjunctiveEffect, Payload, C>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::analysis::ScopedView<tyr::formalism::planning::ConjunctiveEffect, Payload, C>& value, FormatContext& ctx) const
    {
        auto os = std::stringstream {};
        os << "ConjunctiveEffectDomain(\n";
        {
            tyr::IndentScope scope(os);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "element = ", value.element.get_index());
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "payload = ", value.payload);
        }
        os << tyr::print_indent << ")";
        return fmt::format_to(ctx.out(), "{}", os.str());
    }
};

template<typename Payload, typename C>
struct formatter<tyr::analysis::ScopedView<tyr::formalism::planning::Axiom, Payload, C>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::analysis::ScopedView<tyr::formalism::planning::Axiom, Payload, C>& value, FormatContext& ctx) const
    {
        auto os = std::stringstream {};
        os << "AxiomDomain(\n";
        {
            tyr::IndentScope scope(os);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "element = ", value.element.get_index());
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "payload = ", value.payload);
        }
        os << tyr::print_indent << ")";
        return fmt::format_to(ctx.out(), "{}", os.str());
    }
};

template<typename Payload, typename C>
struct formatter<tyr::analysis::ScopedView<tyr::formalism::datalog::Rule, Payload, C>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::analysis::ScopedView<tyr::formalism::datalog::Rule, Payload, C>& value, FormatContext& ctx) const
    {
        auto os = std::stringstream {};
        os << "RuleDomain(\n";
        {
            tyr::IndentScope scope(os);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "element = ", value.element.get_index());
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "payload = ", value.payload);
        }
        os << tyr::print_indent << ")";
        return fmt::format_to(ctx.out(), "{}", os.str());
    }
};

template<typename C>
struct formatter<tyr::analysis::VariableDomainView<C>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::analysis::VariableDomainView<C>& value, FormatContext& ctx) const
    {
        return fmt::format_to(ctx.out(), "{}", value.objects);
    }
};

template<>
struct formatter<tyr::analysis::ProgramVariableDomainsView, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::analysis::ProgramVariableDomainsView& value, FormatContext& ctx) const
    {
        auto os = std::stringstream {};
        os << "ProgramVariableDomains(\n";
        {
            tyr::IndentScope scope(os);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "static predicate domains = ", value.static_predicate_domains);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "fluent predicate domains = ", value.fluent_predicate_domains);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "static function domains = ", value.static_function_domains);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "fluent function domains = ", value.fluent_function_domains);

            for (const auto& [rule, domain] : value.rule_domains)
            {
                os << tyr::print_indent;
                fmt::print(os, "{}\n", domain);
            }
        }
        os << tyr::print_indent << ")";
        return fmt::format_to(ctx.out(), "{}", os.str());
    }
};

template<>
struct formatter<tyr::analysis::TaskVariableDomainsView, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::analysis::TaskVariableDomainsView& value, FormatContext& ctx) const
    {
        auto os = std::stringstream {};
        os << "TaskVariableDomains(\n";
        {
            tyr::IndentScope scope(os);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "static predicate domains = ", value.static_predicate_domains);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "fluent predicate domains = ", value.fluent_predicate_domains);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "derived predicate domains = ", value.derived_predicate_domains);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "static function domains = ", value.static_function_domains);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "fluent function domains = ", value.fluent_function_domains);

            for (const auto& [action, domain] : value.action_domains)
            {
                os << tyr::print_indent;
                fmt::print(os, "{}\n", domain);
            }

            for (const auto& [axiom, domain] : value.axiom_domains)
            {
                os << tyr::print_indent;
                fmt::print(os, "{}\n", domain);
            }
        }
        os << tyr::print_indent << ")";
        return fmt::format_to(ctx.out(), "{}", os.str());
    }
};

}  // namespace fmt

#endif
