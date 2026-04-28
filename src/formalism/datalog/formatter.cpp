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

#include "tyr/formalism/datalog/formatter.hpp"

#include "tyr/common/formatter.hpp"
#include "tyr/common/iostream.hpp"
#include "tyr/formalism/datalog/datas.hpp"
#include "tyr/formalism/datalog/variable_dependency_graph.hpp"
#include "tyr/formalism/datalog/views.hpp"
#include "tyr/formalism/formatter.hpp"

#include <fmt/core.h>
#include <fmt/ostream.h>
#include <fmt/ranges.h>
#include <ostream>

fmt::format_context::iterator fmt::formatter<tyr::formalism::datalog::VariableDependencyGraph, char>::format(const tyr::formalism::datalog::VariableDependencyGraph& value,
                                                                                   fmt::format_context& ctx) const
{
    auto os = std::stringstream {};
    os << "graph {\n";
    const auto k = value.k();
    for (tyr::uint_t i = 0; i < k; ++i)
        fmt::print(os, "n{} [label=\"V{}\"];\n", i, i);
    for (tyr::uint_t i = 0; i < k; ++i)
        for (tyr::uint_t j = i + 1; j < k; ++j)
            if (value.binary().has_dependency(i, j))
                fmt::print(os, "n{} -- n{};\n", i, j);
    os << "}\n";
    return fmt::format_to(ctx.out(), "{}", os.str());
}

fmt::format_context::iterator fmt::formatter<tyr::Data<tyr::formalism::datalog::FunctionExpression>, char>::format(
    const tyr::Data<tyr::formalism::datalog::FunctionExpression>& value, fmt::format_context& ctx) const
{
    return fmt::format_to(ctx.out(), "{}", value.value);
}

fmt::format_context::iterator fmt::formatter<tyr::formalism::datalog::FunctionExpressionView, char>::format(const tyr::formalism::datalog::FunctionExpressionView& value,
                                                                                  fmt::format_context& ctx) const
{
    return fmt::format_to(ctx.out(), "{}", value.get_variant());
}

fmt::format_context::iterator fmt::formatter<tyr::Data<tyr::formalism::datalog::GroundFunctionExpression>, char>::format(
    const tyr::Data<tyr::formalism::datalog::GroundFunctionExpression>& value, fmt::format_context& ctx) const
{
    return fmt::format_to(ctx.out(), "{}", value.value);
}

fmt::format_context::iterator fmt::formatter<tyr::formalism::datalog::GroundFunctionExpressionView, char>::format(
    const tyr::formalism::datalog::GroundFunctionExpressionView& value, fmt::format_context& ctx) const
{
    return fmt::format_to(ctx.out(), "{}", value.get_variant());
}

fmt::format_context::iterator fmt::formatter<tyr::Data<tyr::formalism::datalog::ConjunctiveCondition>, char>::format(
    const tyr::Data<tyr::formalism::datalog::ConjunctiveCondition>& value, fmt::format_context& ctx) const
{
    auto os = std::stringstream {};
    os << "ConjunctiveCondition(\n";
    {
        tyr::IndentScope scope(os);
        os << tyr::print_indent;
        fmt::print(os, "{}{}\n", "index = ", value.index);
        os << tyr::print_indent;
        fmt::print(os, "{}{}\n", "variables = ", value.variables);
        os << tyr::print_indent;
        fmt::print(os, "{}{}\n", "static literals = ", value.static_literals);
        os << tyr::print_indent;
        fmt::print(os, "{}{}\n", "fluent literals = ", value.fluent_literals);
        os << tyr::print_indent;
        fmt::print(os, "{}{}\n", "numeric constraints = ", value.numeric_constraints);
    }
    os << tyr::print_indent << ")";
    return fmt::format_to(ctx.out(), "{}", os.str());
}

fmt::format_context::iterator fmt::formatter<tyr::formalism::datalog::ConjunctiveConditionView, char>::format(const tyr::formalism::datalog::ConjunctiveConditionView& value,
                                                                                    fmt::format_context& ctx) const
{
    auto os = std::stringstream {};
    os << "ConjunctiveCondition(\n";
    {
        tyr::IndentScope scope(os);
        os << tyr::print_indent;
        fmt::print(os, "{}{}\n", "index = ", value.get_index());
        os << tyr::print_indent;
        fmt::print(os, "{}{}\n", "variables = ", value.get_variables());
        os << tyr::print_indent;
        fmt::print(os, "{}{}\n", "static literals = ", value.template get_literals<tyr::formalism::StaticTag>());
        os << tyr::print_indent;
        fmt::print(os, "{}{}\n", "fluent literals = ", value.template get_literals<tyr::formalism::FluentTag>());
        os << tyr::print_indent;
        fmt::print(os, "{}{}\n", "numeric constraints = ", value.get_numeric_constraints());
    }
    os << tyr::print_indent << ")";
    return fmt::format_to(ctx.out(), "{}", os.str());
}

fmt::format_context::iterator fmt::formatter<tyr::Data<tyr::formalism::datalog::Rule>, char>::format(const tyr::Data<tyr::formalism::datalog::Rule>& value,
                                                                            fmt::format_context& ctx) const
{
    auto os = std::stringstream {};
    os << "Rule(\n";
    {
        tyr::IndentScope scope(os);
        os << tyr::print_indent;
        fmt::print(os, "{}{}\n", "index = ", value.index);
        os << tyr::print_indent;
        fmt::print(os, "{}{}\n", "variables = ", value.variables);
        os << tyr::print_indent;
        fmt::print(os, "{}{}\n", "head = ", value.head);
        os << tyr::print_indent;
        fmt::print(os, "{}{}\n", "body = ", value.body);
        os << tyr::print_indent;
        fmt::print(os, "{}{}\n", "cost = ", value.cost);
    }
    os << tyr::print_indent << ")";
    return fmt::format_to(ctx.out(), "{}", os.str());
}

fmt::format_context::iterator fmt::formatter<tyr::formalism::datalog::RuleView, char>::format(const tyr::formalism::datalog::RuleView& value, fmt::format_context& ctx) const
{
    auto os = std::stringstream {};
    os << "Rule(\n";
    {
        tyr::IndentScope scope(os);
        os << tyr::print_indent;
        fmt::print(os, "{}{}\n", "index = ", value.get_index());
        os << tyr::print_indent;
        fmt::print(os, "{}{}\n", "variables = ", value.get_variables());
        os << tyr::print_indent;
        fmt::print(os, "{}{}\n", "head = ", value.get_head());
        os << tyr::print_indent;
        fmt::print(os, "{}{}\n", "body = ", value.get_body());
        os << tyr::print_indent;
        fmt::print(os, "{}{}\n", "cost = ", value.get_cost());
    }
    os << tyr::print_indent << ")";
    return fmt::format_to(ctx.out(), "{}", os.str());
}

fmt::format_context::iterator fmt::formatter<tyr::Data<tyr::formalism::datalog::GroundConjunctiveCondition>, char>::format(
    const tyr::Data<tyr::formalism::datalog::GroundConjunctiveCondition>& value, fmt::format_context& ctx) const
{
    auto os = std::stringstream {};
    os << "GroundConjunctiveCondition(\n";
    {
        tyr::IndentScope scope(os);
        os << tyr::print_indent;
        fmt::print(os, "{}{}\n", "index = ", value.index);
        os << tyr::print_indent;
        fmt::print(os, "{}{}\n", "static literals = ", value.static_literals);
        os << tyr::print_indent;
        fmt::print(os, "{}{}\n", "fluent literals = ", value.fluent_literals);
        os << tyr::print_indent;
        fmt::print(os, "{}{}\n", "numeric constraints = ", value.numeric_constraints);
    }
    os << tyr::print_indent << ")";
    return fmt::format_to(ctx.out(), "{}", os.str());
}

fmt::format_context::iterator fmt::formatter<tyr::formalism::datalog::GroundConjunctiveConditionView, char>::format(
    const tyr::formalism::datalog::GroundConjunctiveConditionView& value, fmt::format_context& ctx) const
{
    auto os = std::stringstream {};
    os << "GroundConjunctiveCondition(\n";
    {
        tyr::IndentScope scope(os);
        os << tyr::print_indent;
        fmt::print(os, "{}{}\n", "index = ", value.get_index());
        os << tyr::print_indent;
        fmt::print(os, "{}{}\n", "static literals = ", value.template get_literals<tyr::formalism::StaticTag>());
        os << tyr::print_indent;
        fmt::print(os, "{}{}\n", "fluent literals = ", value.template get_literals<tyr::formalism::FluentTag>());
        os << tyr::print_indent;
        fmt::print(os, "{}{}\n", "numeric constraints = ", value.get_numeric_constraints());
    }
    os << tyr::print_indent << ")";
    return fmt::format_to(ctx.out(), "{}", os.str());
}

fmt::format_context::iterator fmt::formatter<tyr::Data<tyr::formalism::datalog::GroundRule>, char>::format(const tyr::Data<tyr::formalism::datalog::GroundRule>& value,
                                                                                  fmt::format_context& ctx) const
{
    auto os = std::stringstream {};
    os << "GroundRule(\n";
    {
        tyr::IndentScope scope(os);
        os << tyr::print_indent;
        fmt::print(os, "{}{}\n", "index = ", value.index);
        os << tyr::print_indent;
        fmt::print(os, "{}{}\n", "head = ", value.head);
        os << tyr::print_indent;
        fmt::print(os, "{}{}\n", "body = ", value.body);
    }
    os << tyr::print_indent << ")";
    return fmt::format_to(ctx.out(), "{}", os.str());
}

fmt::format_context::iterator fmt::formatter<tyr::formalism::datalog::GroundRuleView, char>::format(const tyr::formalism::datalog::GroundRuleView& value,
                                                                           fmt::format_context& ctx) const
{
    auto os = std::stringstream {};
    os << "GroundRule(\n";
    {
        tyr::IndentScope scope(os);
        os << tyr::print_indent;
        fmt::print(os, "{}{}\n", "index = ", value.get_index());
        os << tyr::print_indent;
        fmt::print(os, "{}{}\n", "head = ", value.get_head());
        os << tyr::print_indent;
        fmt::print(os, "{}{}\n", "body = ", value.get_body());
    }
    os << tyr::print_indent << ")";
    return fmt::format_to(ctx.out(), "{}", os.str());
}

fmt::format_context::iterator fmt::formatter<tyr::Data<tyr::formalism::datalog::Program>, char>::format(const tyr::Data<tyr::formalism::datalog::Program>& value,
                                                                               fmt::format_context& ctx) const
{
    auto os = std::stringstream {};
    os << "Program(\n";
    {
        tyr::IndentScope scope(os);
        os << tyr::print_indent;
        fmt::print(os, "{}{}\n", "index = ", value.index);
        os << tyr::print_indent;
        fmt::print(os, "{}{}\n", "static predicates =", value.static_predicates);
        os << tyr::print_indent;
        fmt::print(os, "{}{}\n", "fluent predicates = ", value.fluent_predicates);
        os << tyr::print_indent;
        fmt::print(os, "{}{}\n", "static functions = ", value.static_functions);
        os << tyr::print_indent;
        fmt::print(os, "{}{}\n", "fluent functions = ", value.fluent_functions);
        os << tyr::print_indent;
        fmt::print(os, "{}{}\n", "objects = ", value.objects);
        os << tyr::print_indent;
        fmt::print(os, "{}{}\n", "static atoms = ", value.static_atoms);
        os << tyr::print_indent;
        fmt::print(os, "{}{}\n", "fluent atoms = ", value.fluent_atoms);
        os << tyr::print_indent;
        fmt::print(os, "{}{}\n", "static fterms = ", value.static_fterm_values);
        os << tyr::print_indent;
        fmt::print(os, "{}{}\n", "fluent fterms = ", value.fluent_fterm_values);
        os << tyr::print_indent;
        fmt::print(os, "{}{}\n", "rules = ", value.rules);
    }
    os << tyr::print_indent << ")";
    return fmt::format_to(ctx.out(), "{}", os.str());
}

fmt::format_context::iterator fmt::formatter<tyr::formalism::datalog::ProgramView, char>::format(const tyr::formalism::datalog::ProgramView& value, fmt::format_context& ctx) const
{
    auto os = std::stringstream {};
    os << "Program(\n";
    {
        tyr::IndentScope scope(os);
        os << tyr::print_indent;
        fmt::print(os, "{}{}\n", "index = ", value.get_index());
        os << tyr::print_indent;
        fmt::print(os, "{}{}\n", "static predicates =", value.template get_predicates<tyr::formalism::StaticTag>());
        os << tyr::print_indent;
        fmt::print(os, "{}{}\n", "fluent predicates = ", value.template get_predicates<tyr::formalism::FluentTag>());
        os << tyr::print_indent;
        fmt::print(os, "{}{}\n", "static functions = ", value.template get_functions<tyr::formalism::StaticTag>());
        os << tyr::print_indent;
        fmt::print(os, "{}{}\n", "fluent functions = ", value.template get_functions<tyr::formalism::FluentTag>());
        os << tyr::print_indent;
        fmt::print(os, "{}{}\n", "objects = ", value.get_objects());
        os << tyr::print_indent;
        fmt::print(os, "{}{}\n", "static atoms = ", value.template get_atoms<tyr::formalism::StaticTag>());
        os << tyr::print_indent;
        fmt::print(os, "{}{}\n", "fluent atoms = ", value.template get_atoms<tyr::formalism::FluentTag>());
        os << tyr::print_indent;
        fmt::print(os, "{}{}\n", "static fterms = ", value.template get_fterm_values<tyr::formalism::StaticTag>());
        os << tyr::print_indent;
        fmt::print(os, "{}{}\n", "fluent fterms = ", value.template get_fterm_values<tyr::formalism::FluentTag>());
        os << tyr::print_indent;
        fmt::print(os, "{}{}\n", "rules = ", value.get_rules());
    }
    os << tyr::print_indent << ")";
    return fmt::format_to(ctx.out(), "{}", os.str());
}
