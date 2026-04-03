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

namespace tyr
{

std::ostream& print(std::ostream& os, const formalism::datalog::VariableDependencyGraph& el)
{
    os << "graph {\n";

    const auto k = el.k();

    for (uint_t i = 0; i < k; ++i)
    {
        fmt::print(os, "n{} [label=\"V{}\"];\n", i, i);
    }

    for (uint_t i = 0; i < k; ++i)
    {
        for (uint_t j = i + 1; j < k; ++j)
        {
            if (el.has_dependency<formalism::StaticTag>(i, j))
            {
                fmt::print(os, "n{} -- n{} [color=blue, key=1];\n", i, j);
            }
            if (el.has_dependency<formalism::FluentTag>(i, j))
            {
                fmt::print(os, "n{} -- n{} [color=red, key=2];\n", i, j);
            }
        }
    }

    os << "}\n";

    return os;
}

std::ostream& print(std::ostream& os, const Data<formalism::datalog::FunctionExpression>& el)
{
    fmt::print(os, "{}", to_string(el.value));
    return os;
}

std::ostream& print(std::ostream& os, const formalism::datalog::FunctionExpressionView& el)
{
    fmt::print(os, "{}", to_string(el.get_variant()));
    return os;
}

std::ostream& print(std::ostream& os, const Data<formalism::datalog::GroundFunctionExpression>& el)
{
    fmt::print(os, "{}", to_string(el.value));
    return os;
}

std::ostream& print(std::ostream& os, const formalism::datalog::GroundFunctionExpressionView& el)
{
    fmt::print(os, "{}", to_string(el.get_variant()));
    return os;
}

std::ostream& print(std::ostream& os, const Data<formalism::datalog::ConjunctiveCondition>& el)
{
    os << "ConjunctiveCondition(\n";
    {
        IndentScope scope(os);

        os << print_indent << "variables = " << el.variables << "\n";

        os << print_indent << "static literals = " << el.static_literals << "\n";

        os << print_indent << "fluent literals = " << el.fluent_literals << "\n";

        os << print_indent << "numeric constraints = " << el.numeric_constraints << "\n";
    }
    os << print_indent << ")";

    return os;
}

std::ostream& print(std::ostream& os, const formalism::datalog::ConjunctiveConditionView& el)
{
    os << "ConjunctiveCondition(\n";
    {
        IndentScope scope(os);

        os << print_indent << "variables = " << el.get_variables() << "\n";

        os << print_indent << "static literals = " << el.template get_literals<formalism::StaticTag>() << "\n";

        os << print_indent << "fluent literals = " << el.template get_literals<formalism::FluentTag>() << "\n";

        os << print_indent << "numeric constraints = " << el.get_numeric_constraints() << "\n";
    }
    os << print_indent << ")";

    return os;
}

std::ostream& print(std::ostream& os, const Data<formalism::datalog::Rule>& el)
{
    os << "Rule(\n";
    {
        IndentScope scope(os);

        os << print_indent << "index = " << el.index << "\n";

        os << print_indent << "variables = " << el.variables << "\n";

        os << print_indent << "head = " << el.head << "\n";

        os << print_indent << "body = " << el.body << "\n";

        os << print_indent << "cost = " << el.cost << "\n";
    }
    os << print_indent << ")";

    return os;
}

std::ostream& print(std::ostream& os, const formalism::datalog::RuleView& el)
{
    os << "Rule(\n";
    {
        IndentScope scope(os);

        os << print_indent << "index = " << el.get_index() << "\n";

        os << print_indent << "variables = " << el.get_variables() << "\n";

        os << print_indent << "head = " << el.get_head() << "\n";

        os << print_indent << "body = " << el.get_body() << "\n";

        os << print_indent << "cost = " << el.get_cost() << "\n";
    }
    os << print_indent << ")";

    return os;
}

std::ostream& print(std::ostream& os, const Data<formalism::datalog::GroundConjunctiveCondition>& el)
{
    os << "GroundConjunctiveCondition(\n";
    {
        IndentScope scope(os);

        os << print_indent << "static literals = " << el.static_literals << "\n";

        os << print_indent << "fluent literals = " << el.fluent_literals << "\n";

        os << print_indent << "numeric constraints = " << el.numeric_constraints << "\n";
    }
    os << print_indent << ")";

    return os;
}

std::ostream& print(std::ostream& os, const formalism::datalog::GroundConjunctiveConditionView& el)
{
    os << "GroundConjunctiveCondition(\n";
    {
        IndentScope scope(os);

        os << print_indent << "static literals = " << el.template get_literals<formalism::StaticTag>() << "\n";

        os << print_indent << "fluent literals = " << el.template get_literals<formalism::FluentTag>() << "\n";

        os << print_indent << "numeric constraints = " << el.get_numeric_constraints() << "\n";
    }
    os << print_indent << ")";

    return os;
}

std::ostream& print(std::ostream& os, const Data<formalism::datalog::GroundRule>& el)
{
    os << "GroundRule(\n";
    {
        IndentScope scope(os);

        os << print_indent << "head = " << el.head << "\n";

        os << print_indent << "body = " << el.body << "\n";
    }
    os << print_indent << ")";

    return os;
}

std::ostream& print(std::ostream& os, const formalism::datalog::GroundRuleView& el)
{
    os << "GroundRule(\n";
    {
        IndentScope scope(os);

        os << print_indent << "head = " << el.get_head() << "\n";

        os << print_indent << "body = " << el.get_body() << "\n";
    }
    os << print_indent << ")";

    return os;
}

std::ostream& print(std::ostream& os, const Data<formalism::datalog::Program>& el)
{
    os << "Program(\n";
    {
        IndentScope scope(os);

        os << print_indent << "static predicates =" << el.static_predicates << "\n";

        os << print_indent << "fluent predicates = " << el.fluent_predicates << "\n";

        os << print_indent << "static functions = " << el.static_functions << "\n";

        os << print_indent << "fluent functions = " << el.fluent_functions << "\n";

        os << print_indent << "objects = " << el.objects << "\n";

        os << print_indent << "static atoms = " << el.static_atoms << "\n";

        os << print_indent << "fluent atoms = " << el.fluent_atoms << "\n";

        os << print_indent << "static fterms = " << el.static_fterm_values << "\n";

        os << print_indent << "fluent fterms = " << el.fluent_fterm_values << "\n";

        os << print_indent << "rules = " << el.rules << "\n";
    }
    os << print_indent << ")";

    return os;
}

std::ostream& print(std::ostream& os, const formalism::datalog::ProgramView& el)
{
    os << "Program(\n";
    {
        IndentScope scope(os);

        os << print_indent << "static predicates =" << el.template get_predicates<formalism::StaticTag>() << "\n";

        os << print_indent << "fluent predicates = " << el.template get_predicates<formalism::FluentTag>() << "\n";

        os << print_indent << "static functions = " << el.template get_functions<formalism::StaticTag>() << "\n";

        os << print_indent << "fluent functions = " << el.template get_functions<formalism::FluentTag>() << "\n";

        os << print_indent << "objects = " << el.get_objects() << "\n";

        os << print_indent << "static atoms = " << el.template get_atoms<formalism::StaticTag>() << "\n";

        os << print_indent << "fluent atoms = " << el.template get_atoms<formalism::FluentTag>() << "\n";

        os << print_indent << "static fterms = " << el.template get_fterm_values<formalism::StaticTag>() << "\n";

        os << print_indent << "fluent fterms = " << el.template get_fterm_values<formalism::FluentTag>() << "\n";

        os << print_indent << "rules = " << el.get_rules() << "\n";
    }
    os << print_indent << ")";

    return os;
}

namespace formalism::datalog
{

std::ostream& operator<<(std::ostream& os, const VariableDependencyGraph& el) { return tyr::print(os, el); }

std::ostream& operator<<(std::ostream& os, const Data<FunctionExpression>& el) { return tyr::print(os, el); }

std::ostream& operator<<(std::ostream& os, const FunctionExpressionView& el) { return tyr::print(os, el); }

std::ostream& operator<<(std::ostream& os, const Data<GroundFunctionExpression>& el) { return tyr::print(os, el); }

std::ostream& operator<<(std::ostream& os, const GroundFunctionExpressionView& el) { return tyr::print(os, el); }

std::ostream& operator<<(std::ostream& os, const Data<ConjunctiveCondition>& el) { return tyr::print(os, el); }

std::ostream& operator<<(std::ostream& os, const ConjunctiveConditionView& el) { return tyr::print(os, el); }

std::ostream& operator<<(std::ostream& os, const Data<Rule>& el) { return tyr::print(os, el); }

std::ostream& operator<<(std::ostream& os, const RuleView& el) { return tyr::print(os, el); }

std::ostream& operator<<(std::ostream& os, const Data<GroundConjunctiveCondition>& el) { return tyr::print(os, el); }

std::ostream& operator<<(std::ostream& os, const GroundConjunctiveConditionView& el) { return tyr::print(os, el); }

std::ostream& operator<<(std::ostream& os, const Data<GroundRule>& el) { return tyr::print(os, el); }

std::ostream& operator<<(std::ostream& os, const GroundRuleView& el) { return tyr::print(os, el); }

std::ostream& operator<<(std::ostream& os, const Data<Program>& el) { return tyr::print(os, el); }

std::ostream& operator<<(std::ostream& os, const ProgramView& el) { return tyr::print(os, el); }

}
}
