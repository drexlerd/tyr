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

#ifndef TYR_FORMALISM_PLANNING_FORMATTER_HPP_
#define TYR_FORMALISM_PLANNING_FORMATTER_HPP_

#include "tyr/common/formatter.hpp"
#include "tyr/common/iostream.hpp"
#include "tyr/formalism/formatter.hpp"
#include "tyr/formalism/planning/declarations.hpp"
#include "tyr/formalism/planning/planning_domain.hpp"
#include "tyr/formalism/planning/planning_fdr_task.hpp"
#include "tyr/formalism/planning/planning_task.hpp"
#include "tyr/formalism/planning/repository.hpp"

#include <fmt/core.h>
#include <fmt/ostream.h>
#include <fmt/ranges.h>
#include <ostream>
#include <sstream>

namespace tyr::formalism::planning
{
struct PlanFormatting
{
};
}  // namespace tyr::formalism::planning

template<>
struct fmt::formatter<tyr::formalism::planning::FDRValue, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template<typename FormatContext>
    auto format(const tyr::formalism::planning::FDRValue& value, FormatContext& ctx) const
    {
        return fmt::format_to(ctx.out(), "{}", tyr::uint_t(value));
    }
};

template<>
struct fmt::formatter<tyr::formalism::planning::OpAssign, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::formalism::planning::OpAssign&, FormatContext& ctx) const { return fmt::format_to(ctx.out(), "assign"); }
};

template<>
struct fmt::formatter<tyr::formalism::planning::OpIncrease, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::formalism::planning::OpIncrease&, FormatContext& ctx) const { return fmt::format_to(ctx.out(), "increase"); }
};

template<>
struct fmt::formatter<tyr::formalism::planning::OpDecrease, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::formalism::planning::OpDecrease&, FormatContext& ctx) const { return fmt::format_to(ctx.out(), "decrease"); }
};

template<>
struct fmt::formatter<tyr::formalism::planning::OpScaleUp, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::formalism::planning::OpScaleUp&, FormatContext& ctx) const { return fmt::format_to(ctx.out(), "scale-up"); }
};

template<>
struct fmt::formatter<tyr::formalism::planning::OpScaleDown, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::formalism::planning::OpScaleDown&, FormatContext& ctx) const { return fmt::format_to(ctx.out(), "scale-down"); }
};

template<>
struct fmt::formatter<tyr::formalism::planning::Minimize, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::formalism::planning::Minimize&, FormatContext& ctx) const { return fmt::format_to(ctx.out(), "minimize"); }
};

template<>
struct fmt::formatter<tyr::formalism::planning::Maximize, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::formalism::planning::Maximize&, FormatContext& ctx) const { return fmt::format_to(ctx.out(), "maximize"); }
};

template<tyr::formalism::OpKind Op, typename T>
struct fmt::formatter<tyr::Data<tyr::formalism::planning::UnaryOperator<Op, T>>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::Data<tyr::formalism::planning::UnaryOperator<Op, T>>& value, FormatContext& ctx) const
    {
        return fmt::format_to(ctx.out(), "({} {})", tyr::to_string(Op {}), tyr::to_string(value.arg));
    }
};

template<tyr::formalism::OpKind Op, typename T>
struct fmt::formatter<tyr::Data<tyr::formalism::planning::BinaryOperator<Op, T>>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::Data<tyr::formalism::planning::BinaryOperator<Op, T>>& value, FormatContext& ctx) const
    {
        return fmt::format_to(ctx.out(), "({} {} {})", tyr::to_string(Op {}), tyr::to_string(value.lhs), tyr::to_string(value.rhs));
    }
};

template<tyr::formalism::OpKind Op, typename T>
struct fmt::formatter<tyr::Data<tyr::formalism::planning::MultiOperator<Op, T>>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::Data<tyr::formalism::planning::MultiOperator<Op, T>>& value, FormatContext& ctx) const
    {
        return fmt::format_to(ctx.out(), "({} {})", tyr::to_string(Op {}), fmt::join(tyr::to_strings(value.args), " "));
    }
};

template<typename T>
struct fmt::formatter<tyr::Data<tyr::formalism::planning::ArithmeticOperator<T>>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::Data<tyr::formalism::planning::ArithmeticOperator<T>>& value, FormatContext& ctx) const
    {
        return fmt::format_to(ctx.out(), "{}", tyr::to_string(value.value));
    }
};

template<typename T>
struct fmt::formatter<tyr::Data<tyr::formalism::planning::BooleanOperator<T>>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::Data<tyr::formalism::planning::BooleanOperator<T>>& value, FormatContext& ctx) const
    {
        return fmt::format_to(ctx.out(), "{}", tyr::to_string(value.value));
    }
};

template<tyr::formalism::FactKind T>
struct fmt::formatter<tyr::Data<tyr::formalism::planning::Atom<T>>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::Data<tyr::formalism::planning::Atom<T>>& value, FormatContext& ctx) const
    {
        return fmt::format_to(ctx.out(), "({} {})", tyr::to_string(value.predicate), fmt::join(tyr::to_strings(value.terms), " "));
    }
};

template<tyr::formalism::FactKind T>
struct fmt::formatter<tyr::Data<tyr::formalism::planning::Literal<T>>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::Data<tyr::formalism::planning::Literal<T>>& value, FormatContext& ctx) const
    {
        if (value.polarity)
        {
            return fmt::format_to(ctx.out(), "{}", tyr::to_string(value.atom));
        }
        return fmt::format_to(ctx.out(), "(not {})", tyr::to_string(value.atom));
    }
};

template<tyr::formalism::FactKind T>
struct fmt::formatter<tyr::Data<tyr::formalism::planning::GroundAtom<T>>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::Data<tyr::formalism::planning::GroundAtom<T>>& value, FormatContext& ctx) const
    {
        return fmt::format_to(ctx.out(), "({})", tyr::to_string(value.binding));
    }
};

template<tyr::formalism::FactKind T>
struct fmt::formatter<tyr::Data<tyr::formalism::planning::GroundLiteral<T>>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::Data<tyr::formalism::planning::GroundLiteral<T>>& value, FormatContext& ctx) const
    {
        if (value.polarity)
        {
            return fmt::format_to(ctx.out(), "{}", tyr::to_string(value.atom));
        }
        return fmt::format_to(ctx.out(), "(not {})", tyr::to_string(value.atom));
    }
};

template<tyr::formalism::FactKind T>
struct fmt::formatter<tyr::Data<tyr::formalism::planning::FunctionTerm<T>>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::Data<tyr::formalism::planning::FunctionTerm<T>>& value, FormatContext& ctx) const
    {
        return fmt::format_to(ctx.out(), "({} {})", tyr::to_string(value.function), fmt::join(tyr::to_strings(value.terms), " "));
    }
};

template<tyr::formalism::FactKind T>
struct fmt::formatter<tyr::Data<tyr::formalism::planning::GroundFunctionTerm<T>>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::Data<tyr::formalism::planning::GroundFunctionTerm<T>>& value, FormatContext& ctx) const
    {
        return fmt::format_to(ctx.out(), "({})", tyr::to_string(value.binding));
    }
};

template<tyr::formalism::FactKind T>
struct fmt::formatter<tyr::Data<tyr::formalism::planning::GroundFunctionTermValue<T>>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::Data<tyr::formalism::planning::GroundFunctionTermValue<T>>& value, FormatContext& ctx) const
    {
        return fmt::format_to(ctx.out(), "(= {} {})", tyr::to_string(value.fterm), tyr::to_string(value.value));
    }
};

template<tyr::formalism::planning::NumericEffectOpKind Op, tyr::formalism::FactKind T>
struct fmt::formatter<tyr::Data<tyr::formalism::planning::NumericEffect<Op, T>>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::Data<tyr::formalism::planning::NumericEffect<Op, T>>& value, FormatContext& ctx) const
    {
        return fmt::format_to(ctx.out(), "({} {} {})", tyr::to_string(Op {}), tyr::to_string(value.fterm), tyr::to_string(value.fexpr));
    }
};

template<tyr::formalism::planning::NumericEffectOpKind Op, tyr::formalism::FactKind T>
struct fmt::formatter<tyr::Data<tyr::formalism::planning::GroundNumericEffect<Op, T>>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::Data<tyr::formalism::planning::GroundNumericEffect<Op, T>>& value, FormatContext& ctx) const
    {
        return fmt::format_to(ctx.out(), "({} {} {})", tyr::to_string(Op {}), tyr::to_string(value.fterm), tyr::to_string(value.fexpr));
    }
};

template<tyr::formalism::FactKind T>
struct fmt::formatter<tyr::Data<tyr::formalism::planning::NumericEffectOperator<T>>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::Data<tyr::formalism::planning::NumericEffectOperator<T>>& value, FormatContext& ctx) const
    {
        return fmt::format_to(ctx.out(), "{}", value.value);
    }
};

template<tyr::formalism::FactKind T>
struct fmt::formatter<tyr::Data<tyr::formalism::planning::GroundNumericEffectOperator<T>>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::Data<tyr::formalism::planning::GroundNumericEffectOperator<T>>& value, FormatContext& ctx) const
    {
        return fmt::format_to(ctx.out(), "{}", value.value);
    }
};

template<tyr::formalism::FactKind T>
struct fmt::formatter<tyr::Data<tyr::formalism::planning::FDRVariable<T>>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::Data<tyr::formalism::planning::FDRVariable<T>>& value, FormatContext& ctx) const
    {
        auto os = std::stringstream {};
        os << "FDRVariable(\n";
        {
            tyr::IndentScope scope(os);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "index = ", value.index);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "atoms = ", value.atoms);
        }
        os << tyr::print_indent << ")";
        return fmt::format_to(ctx.out(), "{}", os.str());
    }
};

template<tyr::formalism::FactKind T>
struct fmt::formatter<tyr::Data<tyr::formalism::planning::FDRFact<T>>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::Data<tyr::formalism::planning::FDRFact<T>>& value, FormatContext& ctx) const
    {
        return fmt::format_to(ctx.out(), "<{},{}>", tyr::to_string(value.variable), tyr::to_string(value.value));
    }
};

template<>
struct fmt::formatter<tyr::Data<tyr::formalism::planning::FunctionExpression>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::Data<tyr::formalism::planning::FunctionExpression>& value, FormatContext& ctx) const
    {
        return fmt::format_to(ctx.out(), "{}", tyr::to_string(value.value));
    }
};

template<>
struct fmt::formatter<tyr::Data<tyr::formalism::planning::GroundFunctionExpression>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::Data<tyr::formalism::planning::GroundFunctionExpression>& value, FormatContext& ctx) const
    {
        return fmt::format_to(ctx.out(), "{}", tyr::to_string(value.value));
    }
};

template<>
struct fmt::formatter<tyr::Data<tyr::formalism::planning::ConjunctiveCondition>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::Data<tyr::formalism::planning::ConjunctiveCondition>& value, FormatContext& ctx) const
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
            fmt::print(os, "{}{}\n", "derived literals = ", value.derived_literals);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "numeric constraints = ", value.numeric_constraints);
        }
        os << tyr::print_indent << ")";
        return fmt::format_to(ctx.out(), "{}", os.str());
    }
};

template<>
struct fmt::formatter<tyr::Data<tyr::formalism::planning::GroundConjunctiveCondition>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::Data<tyr::formalism::planning::GroundConjunctiveCondition>& value, FormatContext& ctx) const
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
            fmt::print(os, "{}{}\n", "derived literals = ", value.derived_literals);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "positive facts = ", value.positive_facts);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "negative facts = ", value.negative_facts);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "numeric constraints = ", value.numeric_constraints);
        }
        os << tyr::print_indent << ")";
        return fmt::format_to(ctx.out(), "{}", os.str());
    }
};

template<>
struct fmt::formatter<tyr::Data<tyr::formalism::planning::ConditionalEffect>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::Data<tyr::formalism::planning::ConditionalEffect>& value, FormatContext& ctx) const
    {
        auto os = std::stringstream {};
        os << "ConditionalEffect(\n";
        {
            tyr::IndentScope scope(os);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "index = ", value.index);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "variables = ", value.variables);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "condition = ", value.condition);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "effect = ", value.effect);
        }
        os << tyr::print_indent << ")";
        return fmt::format_to(ctx.out(), "{}", os.str());
    }
};

template<>
struct fmt::formatter<tyr::Data<tyr::formalism::planning::GroundConditionalEffect>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::Data<tyr::formalism::planning::GroundConditionalEffect>& value, FormatContext& ctx) const
    {
        auto os = std::stringstream {};
        os << "GroundConditionalEffect(\n";
        {
            tyr::IndentScope scope(os);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "index = ", value.index);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "condition = ", value.condition);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "effect = ", value.effect);
        }
        os << tyr::print_indent << ")";
        return fmt::format_to(ctx.out(), "{}", os.str());
    }
};

template<>
struct fmt::formatter<tyr::Data<tyr::formalism::planning::ConjunctiveEffect>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::Data<tyr::formalism::planning::ConjunctiveEffect>& value, FormatContext& ctx) const
    {
        auto os = std::stringstream {};
        os << "ConjunctiveEffect(\n";
        {
            tyr::IndentScope scope(os);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "index = ", value.index);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "fluent literals = ", value.literals);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "fluent numeric effects = ", value.numeric_effects);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "auxiliary numeric effect = ", value.auxiliary_numeric_effect);
        }
        os << tyr::print_indent << ")";
        return fmt::format_to(ctx.out(), "{}", os.str());
    }
};

template<>
struct fmt::formatter<tyr::Data<tyr::formalism::planning::GroundConjunctiveEffect>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::Data<tyr::formalism::planning::GroundConjunctiveEffect>& value, FormatContext& ctx) const
    {
        auto os = std::stringstream {};
        os << "GroundConjunctiveEffect(\n";
        {
            tyr::IndentScope scope(os);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "index = ", value.index);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "add facts = ", value.add_facts);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "del facts = ", value.del_facts);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "fluent numeric effects = ", value.numeric_effects);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "auxiliary numeric effect = ", value.auxiliary_numeric_effect);
        }
        os << tyr::print_indent << ")";
        return fmt::format_to(ctx.out(), "{}", os.str());
    }
};

template<>
struct fmt::formatter<tyr::Data<tyr::formalism::planning::Action>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::Data<tyr::formalism::planning::Action>& value, FormatContext& ctx) const
    {
        auto os = std::stringstream {};
        os << "Action(\n";
        {
            tyr::IndentScope scope(os);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "index = ", value.index);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "name = ", value.name);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "variables = ", value.variables);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "condition = ", value.condition);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "effects = ", value.effects);
        }
        os << tyr::print_indent << ")";
        return fmt::format_to(ctx.out(), "{}", os.str());
    }
};

template<>
struct fmt::formatter<tyr::Data<tyr::formalism::planning::GroundAction>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::Data<tyr::formalism::planning::GroundAction>& value, FormatContext& ctx) const
    {
        auto os = std::stringstream {};
        os << "GroundAction(\n";
        {
            tyr::IndentScope scope(os);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "index = ", value.index);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "binding = ", value.binding);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "condition = ", value.condition);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "effects = ", value.effects);
        }
        os << tyr::print_indent << ")";
        return fmt::format_to(ctx.out(), "{}", os.str());
    }
};

template<>
struct fmt::formatter<tyr::Data<tyr::formalism::planning::Axiom>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::Data<tyr::formalism::planning::Axiom>& value, FormatContext& ctx) const
    {
        auto os = std::stringstream {};
        os << "Axiom(\n";
        {
            tyr::IndentScope scope(os);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "index = ", value.index);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "variables = ", value.variables);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "body = ", value.body);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "head = ", value.head);
        }
        os << tyr::print_indent << ")";
        return fmt::format_to(ctx.out(), "{}", os.str());
    }
};

template<>
struct fmt::formatter<tyr::Data<tyr::formalism::planning::GroundAxiom>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::Data<tyr::formalism::planning::GroundAxiom>& value, FormatContext& ctx) const
    {
        auto os = std::stringstream {};
        os << "GroundAxiom(\n";
        {
            tyr::IndentScope scope(os);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "index = ", value.index);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "binding = ", value.binding);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "body = ", value.body);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "head = ", value.head);
        }
        os << tyr::print_indent << ")";
        return fmt::format_to(ctx.out(), "{}", os.str());
    }
};

template<>
struct fmt::formatter<tyr::Data<tyr::formalism::planning::Metric>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::Data<tyr::formalism::planning::Metric>& value, FormatContext& ctx) const
    {
        return fmt::format_to(ctx.out(), "({} {})", tyr::to_string(value.objective), tyr::to_string(value.fexpr));
    }
};

template<>
struct fmt::formatter<tyr::Data<tyr::formalism::planning::Task>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::Data<tyr::formalism::planning::Task>& value, FormatContext& ctx) const
    {
        auto os = std::stringstream {};
        os << "Task(\n";
        {
            tyr::IndentScope scope(os);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "index = ", value.index);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "name = ", value.name);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "derived predicates = ", value.derived_predicates);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "objects = ", value.objects);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "static atoms = ", value.static_atoms);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "fluent atoms = ", value.fluent_atoms);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "static numeric variables = ", value.static_fterm_values);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "fluent numeric variables = ", value.fluent_fterm_values);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "auxiliary numeric variable = ", value.auxiliary_fterm_value);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "goal = ", value.goal);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "metric = ", value.metric);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "axioms = ", value.axioms);
        }
        os << tyr::print_indent << ")";
        return fmt::format_to(ctx.out(), "{}", os.str());
    }
};

template<>
struct fmt::formatter<tyr::Data<tyr::formalism::planning::Domain>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::Data<tyr::formalism::planning::Domain>& value, FormatContext& ctx) const
    {
        auto os = std::stringstream {};
        os << "Domain(\n";
        {
            tyr::IndentScope scope(os);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "index = ", value.index);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "name = ", value.name);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "static predicates = ", value.static_predicates);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "fluent predicates = ", value.fluent_predicates);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "derived predicates = ", value.derived_predicates);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "static functions = ", value.static_functions);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "fluent functions = ", value.fluent_functions);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "auxiliary function = ", value.auxiliary_function);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "constants = ", value.constants);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "actions = ", value.actions);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "axioms = ", value.axioms);
        }
        os << tyr::print_indent << ")";
        return fmt::format_to(ctx.out(), "{}", os.str());
    }
};

template<>
struct fmt::formatter<tyr::Data<tyr::formalism::planning::FDRTask>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::Data<tyr::formalism::planning::FDRTask>& value, FormatContext& ctx) const
    {
        auto os = std::stringstream {};
        os << "FDRTask(\n";
        {
            tyr::IndentScope scope(os);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "index = ", value.index);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "name = ", value.name);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "derived predicates = ", value.derived_predicates);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "objects = ", value.objects);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "static atoms = ", value.static_atoms);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "fluent atoms = ", value.fluent_atoms);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "derived atoms = ", value.derived_atoms);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "static numeric variables = ", value.static_fterm_values);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "fluent numeric variables = ", value.fluent_fterm_values);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "auxiliary numeric variable = ", value.auxiliary_fterm_value);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "goal = ", value.goal);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "metric = ", value.metric);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "axioms = ", value.axioms);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "fluent variables = ", value.fluent_variables);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "fluent facts = ", value.fluent_facts);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "goal = ", value.goal);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "ground actions = ", value.ground_actions);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "ground axioms = ", value.ground_axioms);
        }
        os << tyr::print_indent << ")";
        return fmt::format_to(ctx.out(), "{}", os.str());
    }
};

template<tyr::formalism::OpKind Op, typename T>
struct fmt::formatter<tyr::formalism::planning::UnaryOperatorView<Op, T>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::formalism::planning::UnaryOperatorView<Op, T>& value, FormatContext& ctx) const
    {
        return fmt::format_to(ctx.out(), "({} {})", tyr::to_string(Op {}), tyr::to_string(value.get_arg()));
    }
};

template<tyr::formalism::OpKind Op, typename T>
struct fmt::formatter<tyr::formalism::planning::BinaryOperatorView<Op, T>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::formalism::planning::BinaryOperatorView<Op, T>& value, FormatContext& ctx) const
    {
        return fmt::format_to(ctx.out(), "({} {} {})", tyr::to_string(Op {}), tyr::to_string(value.get_lhs()), tyr::to_string(value.get_rhs()));
    }
};

template<tyr::formalism::OpKind Op, typename T>
struct fmt::formatter<tyr::formalism::planning::MultiOperatorView<Op, T>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::formalism::planning::MultiOperatorView<Op, T>& value, FormatContext& ctx) const
    {
        return fmt::format_to(ctx.out(), "({} {})", tyr::to_string(Op {}), fmt::join(tyr::to_strings(value.get_args()), " "));
    }
};

template<typename T>
struct fmt::formatter<tyr::formalism::planning::ArithmeticOperatorView<T>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::formalism::planning::ArithmeticOperatorView<T>& value, FormatContext& ctx) const
    {
        return fmt::format_to(ctx.out(), "{}", tyr::to_string(value.get_variant()));
    }
};

template<typename T>
struct fmt::formatter<tyr::formalism::planning::BooleanOperatorView<T>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::formalism::planning::BooleanOperatorView<T>& value, FormatContext& ctx) const
    {
        return fmt::format_to(ctx.out(), "{}", tyr::to_string(value.get_variant()));
    }
};

template<tyr::formalism::FactKind T>
struct fmt::formatter<tyr::formalism::planning::AtomView<T>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::formalism::planning::AtomView<T>& value, FormatContext& ctx) const
    {
        return fmt::format_to(ctx.out(), "({} {})", tyr::to_string(value.get_predicate().get_name()), fmt::join(tyr::to_strings(value.get_terms()), " "));
    }
};

template<tyr::formalism::FactKind T>
struct fmt::formatter<tyr::formalism::planning::LiteralView<T>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::formalism::planning::LiteralView<T>& value, FormatContext& ctx) const
    {
        if (value.get_polarity())
        {
            return fmt::format_to(ctx.out(), "{}", tyr::to_string(value.get_atom()));
        }
        return fmt::format_to(ctx.out(), "(not {})", tyr::to_string(value.get_atom()));
    }
};

template<tyr::formalism::FactKind T>
struct fmt::formatter<tyr::formalism::planning::GroundAtomView<T>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::formalism::planning::GroundAtomView<T>& value, FormatContext& ctx) const
    {
        return fmt::format_to(ctx.out(), "({} {})", tyr::to_string(value.get_predicate().get_name()), fmt::join(tyr::to_strings(value.get_row().get_objects()), " "));
    }
};

template<tyr::formalism::FactKind T>
struct fmt::formatter<tyr::formalism::planning::GroundLiteralView<T>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::formalism::planning::GroundLiteralView<T>& value, FormatContext& ctx) const
    {
        if (value.get_polarity())
        {
            return fmt::format_to(ctx.out(), "{}", tyr::to_string(value.get_atom()));
        }
        return fmt::format_to(ctx.out(), "(not {})", tyr::to_string(value.get_atom()));
    }
};

template<tyr::formalism::FactKind T>
struct fmt::formatter<tyr::formalism::planning::FunctionTermView<T>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::formalism::planning::FunctionTermView<T>& value, FormatContext& ctx) const
    {
        return fmt::format_to(ctx.out(), "({} {})", tyr::to_string(value.get_function().get_name()), fmt::join(tyr::to_strings(value.get_terms()), " "));
    }
};

template<tyr::formalism::FactKind T>
struct fmt::formatter<tyr::formalism::planning::GroundFunctionTermView<T>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::formalism::planning::GroundFunctionTermView<T>& value, FormatContext& ctx) const
    {
        return fmt::format_to(ctx.out(), "({} {})", tyr::to_string(value.get_function().get_name()), fmt::join(tyr::to_strings(value.get_row().get_objects()), " "));
    }
};

template<tyr::formalism::FactKind T>
struct fmt::formatter<tyr::formalism::planning::GroundFunctionTermValueView<T>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::formalism::planning::GroundFunctionTermValueView<T>& value, FormatContext& ctx) const
    {
        return fmt::format_to(ctx.out(), "(= {} {})", tyr::to_string(value.get_fterm()), tyr::to_string(value.get_value()));
    }
};

template<>
struct fmt::formatter<tyr::formalism::planning::FunctionExpressionView, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::formalism::planning::FunctionExpressionView& value, FormatContext& ctx) const
    {
        return fmt::format_to(ctx.out(), "{}", tyr::to_string(value.get_variant()));
    }
};

template<>
struct fmt::formatter<tyr::formalism::planning::GroundFunctionExpressionView, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::formalism::planning::GroundFunctionExpressionView& value, FormatContext& ctx) const
    {
        return fmt::format_to(ctx.out(), "{}", tyr::to_string(value.get_variant()));
    }
};

template<tyr::formalism::planning::NumericEffectOpKind Op, tyr::formalism::FactKind T>
struct fmt::formatter<tyr::formalism::planning::NumericEffectView<Op, T>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::formalism::planning::NumericEffectView<Op, T>& value, FormatContext& ctx) const
    {
        return fmt::format_to(ctx.out(), "({} {} {})", tyr::to_string(Op {}), tyr::to_string(value.get_fterm()), tyr::to_string(value.get_fexpr()));
    }
};

template<tyr::formalism::planning::NumericEffectOpKind Op, tyr::formalism::FactKind T>
struct fmt::formatter<tyr::formalism::planning::GroundNumericEffectView<Op, T>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::formalism::planning::GroundNumericEffectView<Op, T>& value, FormatContext& ctx) const
    {
        return fmt::format_to(ctx.out(), "({} {} {})", tyr::to_string(Op {}), tyr::to_string(value.get_fterm()), tyr::to_string(value.get_fexpr()));
    }
};

template<tyr::formalism::FactKind T>
struct fmt::formatter<tyr::formalism::planning::NumericEffectOperatorView<T>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::formalism::planning::NumericEffectOperatorView<T>& value, FormatContext& ctx) const
    {
        return fmt::format_to(ctx.out(), "{}", value.get_variant());
    }
};

template<tyr::formalism::FactKind T>
struct fmt::formatter<tyr::formalism::planning::GroundNumericEffectOperatorView<T>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::formalism::planning::GroundNumericEffectOperatorView<T>& value, FormatContext& ctx) const
    {
        return fmt::format_to(ctx.out(), "{}", value.get_variant());
    }
};

template<>
struct fmt::formatter<tyr::formalism::planning::ConjunctiveConditionView, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::formalism::planning::ConjunctiveConditionView& value, FormatContext& ctx) const
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
            fmt::print(os, "{}{}\n", "derived literals = ", value.template get_literals<tyr::formalism::DerivedTag>());
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "numeric constraints = ", value.get_numeric_constraints());
        }
        os << tyr::print_indent << ")";
        return fmt::format_to(ctx.out(), "{}", os.str());
    }
};

template<>
struct fmt::formatter<tyr::formalism::planning::GroundConjunctiveConditionView, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::formalism::planning::GroundConjunctiveConditionView& value, FormatContext& ctx) const
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
            fmt::print(os, "{}{}\n", "derived literals = ", value.template get_literals<tyr::formalism::DerivedTag>());
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "positive facts = ", value.template get_facts<tyr::formalism::PositiveTag>());
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "negative facts = ", value.template get_facts<tyr::formalism::NegativeTag>());
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "numeric constraints = ", value.get_numeric_constraints());
        }
        os << tyr::print_indent << ")";
        return fmt::format_to(ctx.out(), "{}", os.str());
    }
};

template<>
struct fmt::formatter<tyr::formalism::planning::ConditionalEffectView, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::formalism::planning::ConditionalEffectView& value, FormatContext& ctx) const
    {
        auto os = std::stringstream {};
        os << "ConditionalEffect(\n";
        {
            tyr::IndentScope scope(os);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "index = ", value.get_index());
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "variables = ", value.get_variables());
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "condition = ", value.get_condition());
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "effect = ", value.get_effect());
        }
        os << tyr::print_indent << ")";
        return fmt::format_to(ctx.out(), "{}", os.str());
    }
};

template<>
struct fmt::formatter<tyr::formalism::planning::GroundConditionalEffectView, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::formalism::planning::GroundConditionalEffectView& value, FormatContext& ctx) const
    {
        auto os = std::stringstream {};
        os << "GroundConditionalEffect(\n";
        {
            tyr::IndentScope scope(os);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "index = ", value.get_index());
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "condition = ", value.get_condition());
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "effect = ", value.get_effect());
        }
        os << tyr::print_indent << ")";
        return fmt::format_to(ctx.out(), "{}", os.str());
    }
};

template<>
struct fmt::formatter<tyr::formalism::planning::ConjunctiveEffectView, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::formalism::planning::ConjunctiveEffectView& value, FormatContext& ctx) const
    {
        auto os = std::stringstream {};
        os << "ConjunctiveEffect(\n";
        {
            tyr::IndentScope scope(os);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "index = ", value.get_index());
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "fluent literals = ", value.get_literals());
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "fluent numeric effects = ", value.get_numeric_effects());
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "auxiliary numeric effect = ", value.get_auxiliary_numeric_effect());
        }
        os << tyr::print_indent << ")";
        return fmt::format_to(ctx.out(), "{}", os.str());
    }
};

template<>
struct fmt::formatter<tyr::formalism::planning::GroundConjunctiveEffectView, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::formalism::planning::GroundConjunctiveEffectView& value, FormatContext& ctx) const
    {
        auto os = std::stringstream {};
        os << "GroundConjunctiveEffect(\n";
        {
            tyr::IndentScope scope(os);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "index = ", value.get_index());
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "add facts = ", value.template get_facts<tyr::formalism::PositiveTag>());
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "del facts = ", value.template get_facts<tyr::formalism::NegativeTag>());
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "fluent numeric effects = ", value.get_numeric_effects());
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "auxiliary numeric effect = ", value.get_auxiliary_numeric_effect());
        }
        os << tyr::print_indent << ")";
        return fmt::format_to(ctx.out(), "{}", os.str());
    }
};

template<>
struct fmt::formatter<tyr::formalism::planning::ActionView, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::formalism::planning::ActionView& value, FormatContext& ctx) const
    {
        auto os = std::stringstream {};
        os << "Action(\n";
        {
            tyr::IndentScope scope(os);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "index = ", value.get_index());
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "name = ", value.get_name());
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "variables = ", value.get_variables());
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "condition = ", value.get_condition());
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "effects = ", value.get_effects());
        }
        os << tyr::print_indent << ")";
        return fmt::format_to(ctx.out(), "{}", os.str());
    }
};

template<>
struct fmt::formatter<tyr::formalism::planning::GroundActionView, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::formalism::planning::GroundActionView& value, FormatContext& ctx) const
    {
        auto os = std::stringstream {};
        os << "GroundAction(\n";
        {
            tyr::IndentScope scope(os);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "index = ", value.get_index());
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "binding = ", value.get_row());
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "action index = ", value.get_action().get_index());
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "condition = ", value.get_condition());
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "effects = ", value.get_effects());
        }
        os << tyr::print_indent << ")";
        return fmt::format_to(ctx.out(), "{}", os.str());
    }
};

template<>
struct fmt::formatter<std::pair<tyr::formalism::planning::GroundActionView, tyr::formalism::planning::PlanFormatting>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const std::pair<tyr::formalism::planning::GroundActionView, tyr::formalism::planning::PlanFormatting>& value, FormatContext& ctx) const
    {
        auto out = fmt::format_to(ctx.out(), "({}", tyr::to_string(value.first.get_action().get_name()));
        for (size_t i = 0; i < value.first.get_action().get_original_arity(); ++i)
        {
            out = fmt::format_to(out, " {}", value.first.get_row().get_objects()[i]);
        }
        return fmt::format_to(out, ")");
    }
};

template<>
struct fmt::formatter<tyr::formalism::planning::AxiomView, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::formalism::planning::AxiomView& value, FormatContext& ctx) const
    {
        auto os = std::stringstream {};
        os << "Axiom(\n";
        {
            tyr::IndentScope scope(os);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "index = ", value.get_index());
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "variables = ", value.get_variables());
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "body = ", value.get_body());
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "head = ", value.get_head());
        }
        os << tyr::print_indent << ")";
        return fmt::format_to(ctx.out(), "{}", os.str());
    }
};

template<>
struct fmt::formatter<tyr::formalism::planning::GroundAxiomView, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::formalism::planning::GroundAxiomView& value, FormatContext& ctx) const
    {
        auto os = std::stringstream {};
        os << "GroundAxiom(\n";
        {
            tyr::IndentScope scope(os);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "index = ", value.get_index());
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "binding = ", value.get_row());
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "axiom index = ", value.get_axiom().get_index());
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "body = ", value.get_body());
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "head = ", value.get_head());
        }
        os << tyr::print_indent << ")";
        return fmt::format_to(ctx.out(), "{}", os.str());
    }
};

template<>
struct fmt::formatter<tyr::formalism::planning::ActionBindingView, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::formalism::planning::ActionBindingView& value, FormatContext& ctx) const
    {
        return fmt::format_to(ctx.out(), "{}", fmt::join(tyr::to_strings(value.get_objects()), " "));
    }
};

template<>
struct fmt::formatter<tyr::formalism::planning::AxiomBindingView, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::formalism::planning::AxiomBindingView& value, FormatContext& ctx) const
    {
        return fmt::format_to(ctx.out(), "{}", fmt::join(tyr::to_strings(value.get_objects()), " "));
    }
};

template<>
struct fmt::formatter<tyr::formalism::planning::MetricView, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::formalism::planning::MetricView& value, FormatContext& ctx) const
    {
        return fmt::format_to(ctx.out(), "({} {})", tyr::to_string(value.get_objective()), tyr::to_string(value.get_fexpr()));
    }
};

template<>
struct fmt::formatter<tyr::formalism::planning::TaskView, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::formalism::planning::TaskView& value, FormatContext& ctx) const
    {
        auto os = std::stringstream {};
        os << "Task(\n";
        {
            tyr::IndentScope scope(os);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "index = ", value.get_index());
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "name = ", value.get_name());
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "derived predicates = ", value.get_derived_predicates());
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "objects = ", value.get_objects());
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "static atoms = ", value.template get_atoms<tyr::formalism::StaticTag>());
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "fluent atoms = ", value.template get_atoms<tyr::formalism::FluentTag>());
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "static numeric variables = ", value.template get_fterm_values<tyr::formalism::StaticTag>());
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "fluent numeric variables = ", value.template get_fterm_values<tyr::formalism::FluentTag>());
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "auxiliary numeric variable = ", value.get_auxiliary_fterm_value());
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "goal = ", value.get_goal());
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "metric = ", value.get_metric());
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "axioms = ", value.get_axioms());
        }
        os << tyr::print_indent << ")";
        return fmt::format_to(ctx.out(), "{}", os.str());
    }
};

template<>
struct fmt::formatter<tyr::formalism::planning::DomainView, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::formalism::planning::DomainView& value, FormatContext& ctx) const
    {
        auto os = std::stringstream {};
        os << "Domain(\n";
        {
            tyr::IndentScope scope(os);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "index = ", value.get_index());
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "name = ", value.get_name());
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "static predicates = ", value.template get_predicates<tyr::formalism::StaticTag>());
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "fluent predicates = ", value.template get_predicates<tyr::formalism::FluentTag>());
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "derived predicates = ", value.template get_predicates<tyr::formalism::DerivedTag>());
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "static functions = ", value.template get_functions<tyr::formalism::StaticTag>());
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "fluent functions = ", value.template get_functions<tyr::formalism::FluentTag>());
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "auxiliary function = ", value.get_auxiliary_function());
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "constants = ", value.get_constants());
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "actions = ", value.get_actions());
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "axioms = ", value.get_axioms());
        }
        os << tyr::print_indent << ")";
        return fmt::format_to(ctx.out(), "{}", os.str());
    }
};

template<tyr::formalism::FactKind T>
struct fmt::formatter<tyr::formalism::planning::FDRVariableView<T>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::formalism::planning::FDRVariableView<T>& value, FormatContext& ctx) const
    {
        auto os = std::stringstream {};
        os << "FDRVariable(\n";
        {
            tyr::IndentScope scope(os);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "index = ", value.get_index());
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "atoms = ", value.get_atoms());
        }
        os << tyr::print_indent << ")";
        return fmt::format_to(ctx.out(), "{}", os.str());
    }
};

template<tyr::formalism::FactKind T>
struct fmt::formatter<tyr::formalism::planning::FDRFactView<T>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::formalism::planning::FDRFactView<T>& value, FormatContext& ctx) const
    {
        if (value.get_value() == tyr::formalism::planning::FDRValue::none())
        {
            return fmt::format_to(ctx.out(),
                                  "<{},{}>: (none-of {})",
                                  tyr::to_string(value.get_variable().get_index()),
                                  tyr::to_string(value.get_value()),
                                  fmt::join(tyr::to_strings(value.get_variable().get_atoms()), " "));
        }
        return fmt::format_to(ctx.out(),
                              "<{},{}>: {}",
                              tyr::to_string(value.get_variable().get_index()),
                              tyr::to_string(value.get_value()),
                              tyr::to_string(value.get_variable().get_atoms()[tyr::uint_t(value.get_value()) - 1]));
    }
};

template<>
struct fmt::formatter<tyr::formalism::planning::FDRTaskView, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::formalism::planning::FDRTaskView& value, FormatContext& ctx) const
    {
        auto os = std::stringstream {};
        os << "FDRTask(\n";
        {
            tyr::IndentScope scope(os);
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "index = ", value.get_index());
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "name = ", value.get_name());
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "derived predicates = ", value.get_derived_predicates());
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "objects = ", value.get_objects());
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "static atoms = ", value.template get_atoms<tyr::formalism::StaticTag>());
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "fluent atoms = ", value.template get_atoms<tyr::formalism::FluentTag>());
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "derived atoms = ", value.template get_atoms<tyr::formalism::DerivedTag>());
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "static numeric variables = ", value.template get_fterm_values<tyr::formalism::StaticTag>());
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "fluent numeric variables = ", value.template get_fterm_values<tyr::formalism::FluentTag>());
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "auxiliary numeric variable = ", value.get_auxiliary_fterm_value());
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "goal = ", value.get_goal());
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "metric = ", value.get_metric());
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "axioms = ", value.get_axioms());
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "fluent variables = ", value.get_fluent_variables());
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "fluent facts = ", value.get_fluent_facts());
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "goal = ", value.get_goal());
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "ground actions = ", value.get_ground_actions());
            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "ground axioms = ", value.get_ground_axioms());
        }
        os << tyr::print_indent << ")";
        return fmt::format_to(ctx.out(), "{}", os.str());
    }
};

template<>
struct fmt::formatter<tyr::formalism::planning::PlanningDomain, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template<typename FormatContext>
    auto format(const tyr::formalism::planning::PlanningDomain& value, FormatContext& ctx) const
    {
        return fmt::format_to(ctx.out(), "{}", value.get_domain());
    }
};

template<>
struct fmt::formatter<tyr::formalism::planning::PlanningTask, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template<typename FormatContext>
    auto format(const tyr::formalism::planning::PlanningTask& value, FormatContext& ctx) const
    {
        return fmt::format_to(ctx.out(), "{}", value.get_task());
    }
};

template<>
struct fmt::formatter<tyr::formalism::planning::PlanningFDRTask, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template<typename FormatContext>
    auto format(const tyr::formalism::planning::PlanningFDRTask& value, FormatContext& ctx) const
    {
        return fmt::format_to(ctx.out(), "{}", value.get_task());
    }
};

#endif
