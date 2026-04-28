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

#ifndef TYR_FORMALISM_DATALOG_FORMATTER_HPP_
#define TYR_FORMALISM_DATALOG_FORMATTER_HPP_

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

namespace fmt
{

template<tyr::formalism::OpKind Op, typename T>
struct formatter<tyr::Data<tyr::formalism::datalog::UnaryOperator<Op, T>>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::Data<tyr::formalism::datalog::UnaryOperator<Op, T>>& value, FormatContext& ctx) const
    {
        return fmt::format_to(ctx.out(), "({} {})", Op {}, value.arg);
    }
};

template<tyr::formalism::OpKind Op, typename T>
struct formatter<tyr::formalism::datalog::UnaryOperatorView<Op, T>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::formalism::datalog::UnaryOperatorView<Op, T>& value, FormatContext& ctx) const
    {
        return fmt::format_to(ctx.out(), "({} {})", Op {}, value.get_arg());
    }
};

template<tyr::formalism::OpKind Op, typename T>
struct formatter<tyr::Data<tyr::formalism::datalog::BinaryOperator<Op, T>>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::Data<tyr::formalism::datalog::BinaryOperator<Op, T>>& value, FormatContext& ctx) const
    {
        return fmt::format_to(ctx.out(), "({} {} {})", Op {}, value.lhs, value.rhs);
    }
};

template<tyr::formalism::OpKind Op, typename T>
struct formatter<tyr::formalism::datalog::BinaryOperatorView<Op, T>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::formalism::datalog::BinaryOperatorView<Op, T>& value, FormatContext& ctx) const
    {
        return fmt::format_to(ctx.out(), "({} {} {})", Op {}, value.get_lhs(), value.get_rhs());
    }
};

template<tyr::formalism::OpKind Op, typename T>
struct formatter<tyr::Data<tyr::formalism::datalog::MultiOperator<Op, T>>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::Data<tyr::formalism::datalog::MultiOperator<Op, T>>& value, FormatContext& ctx) const
    {
        return fmt::format_to(ctx.out(), "({} {})", Op {}, fmt::join(tyr::to_strings(value.args), " "));
    }
};

template<tyr::formalism::OpKind Op, typename T>
struct formatter<tyr::formalism::datalog::MultiOperatorView<Op, T>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::formalism::datalog::MultiOperatorView<Op, T>& value, FormatContext& ctx) const
    {
        return fmt::format_to(ctx.out(), "({} {})", Op {}, fmt::join(tyr::to_strings(value.get_args()), " "));
    }
};

template<typename T>
struct formatter<tyr::Data<tyr::formalism::datalog::ArithmeticOperator<T>>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::Data<tyr::formalism::datalog::ArithmeticOperator<T>>& value, FormatContext& ctx) const
    {
        return fmt::format_to(ctx.out(), "{}", value.value);
    }
};

template<typename T>
struct formatter<tyr::formalism::datalog::ArithmeticOperatorView<T>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::formalism::datalog::ArithmeticOperatorView<T>& value, FormatContext& ctx) const
    {
        return fmt::format_to(ctx.out(), "{}", value.get_variant());
    }
};

template<typename T>
struct formatter<tyr::Data<tyr::formalism::datalog::BooleanOperator<T>>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::Data<tyr::formalism::datalog::BooleanOperator<T>>& value, FormatContext& ctx) const
    {
        return fmt::format_to(ctx.out(), "{}", value.value);
    }
};

template<typename T>
struct formatter<tyr::formalism::datalog::BooleanOperatorView<T>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::formalism::datalog::BooleanOperatorView<T>& value, FormatContext& ctx) const
    {
        return fmt::format_to(ctx.out(), "{}", value.get_variant());
    }
};

template<tyr::formalism::FactKind T>
struct formatter<tyr::Data<tyr::formalism::datalog::Atom<T>>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::Data<tyr::formalism::datalog::Atom<T>>& value, FormatContext& ctx) const
    {
        return fmt::format_to(ctx.out(), "({} {})", value.predicate, fmt::join(tyr::to_strings(value.terms), " "));
    }
};

template<tyr::formalism::FactKind T>
struct formatter<tyr::formalism::datalog::AtomView<T>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::formalism::datalog::AtomView<T>& value, FormatContext& ctx) const
    {
        return fmt::format_to(ctx.out(), "({} {})", value.get_predicate().get_name(), fmt::join(tyr::to_strings(value.get_terms()), " "));
    }
};

template<tyr::formalism::FactKind T>
struct formatter<tyr::Data<tyr::formalism::datalog::Literal<T>>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::Data<tyr::formalism::datalog::Literal<T>>& value, FormatContext& ctx) const
    {
        if (value.polarity)
            return fmt::format_to(ctx.out(), "{}", value.atom);
        return fmt::format_to(ctx.out(), "(not {})", value.atom);
    }
};

template<tyr::formalism::FactKind T>
struct formatter<tyr::formalism::datalog::LiteralView<T>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::formalism::datalog::LiteralView<T>& value, FormatContext& ctx) const
    {
        if (value.get_polarity())
            return fmt::format_to(ctx.out(), "{}", value.get_atom());
        return fmt::format_to(ctx.out(), "(not {})", value.get_atom());
    }
};

template<tyr::formalism::FactKind T>
struct formatter<tyr::Data<tyr::formalism::datalog::GroundAtom<T>>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::Data<tyr::formalism::datalog::GroundAtom<T>>& value, FormatContext& ctx) const
    {
        return fmt::format_to(ctx.out(), "({} {})", value.index.group, fmt::join(tyr::to_strings(value.objects), " "));
    }
};

template<tyr::formalism::FactKind T>
struct formatter<tyr::formalism::datalog::GroundAtomView<T>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::formalism::datalog::GroundAtomView<T>& value, FormatContext& ctx) const
    {
        return fmt::format_to(ctx.out(), "({} {})", value.get_predicate().get_name(), fmt::join(tyr::to_strings(value.get_row().get_objects()), " "));
    }
};

template<tyr::formalism::FactKind T>
struct formatter<tyr::Data<tyr::formalism::datalog::GroundLiteral<T>>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::Data<tyr::formalism::datalog::GroundLiteral<T>>& value, FormatContext& ctx) const
    {
        if (value.polarity)
            return fmt::format_to(ctx.out(), "{}", value.atom);
        return fmt::format_to(ctx.out(), "(not {})", value.atom);
    }
};

template<tyr::formalism::FactKind T>
struct formatter<tyr::formalism::datalog::GroundLiteralView<T>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::formalism::datalog::GroundLiteralView<T>& value, FormatContext& ctx) const
    {
        if (value.get_polarity())
            return fmt::format_to(ctx.out(), "{}", value.get_atom());
        return fmt::format_to(ctx.out(), "(not {})", value.get_atom());
    }
};

template<tyr::formalism::FactKind T>
struct formatter<tyr::Data<tyr::formalism::datalog::FunctionTerm<T>>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::Data<tyr::formalism::datalog::FunctionTerm<T>>& value, FormatContext& ctx) const
    {
        return fmt::format_to(ctx.out(), "({} {})", value.function, fmt::join(tyr::to_strings(value.terms), " "));
    }
};

template<tyr::formalism::FactKind T>
struct formatter<tyr::formalism::datalog::FunctionTermView<T>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::formalism::datalog::FunctionTermView<T>& value, FormatContext& ctx) const
    {
        return fmt::format_to(ctx.out(), "({} {})", value.get_function().get_name(), fmt::join(tyr::to_strings(value.get_terms()), " "));
    }
};

template<tyr::formalism::FactKind T>
struct formatter<tyr::Data<tyr::formalism::datalog::GroundFunctionTerm<T>>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::Data<tyr::formalism::datalog::GroundFunctionTerm<T>>& value, FormatContext& ctx) const
    {
        return fmt::format_to(ctx.out(), "({} {})", value.index.group, fmt::join(tyr::to_strings(value.objects), " "));
    }
};

template<tyr::formalism::FactKind T>
struct formatter<tyr::formalism::datalog::GroundFunctionTermView<T>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::formalism::datalog::GroundFunctionTermView<T>& value, FormatContext& ctx) const
    {
        return fmt::format_to(ctx.out(), "({} {})", value.get_function().get_name(), fmt::join(tyr::to_strings(value.get_row().get_objects()), " "));
    }
};

template<tyr::formalism::FactKind T>
struct formatter<tyr::Data<tyr::formalism::datalog::GroundFunctionTermValue<T>>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::Data<tyr::formalism::datalog::GroundFunctionTermValue<T>>& value, FormatContext& ctx) const
    {
        return fmt::format_to(ctx.out(), "(= {} {})", value.fterm, value.value);
    }
};

template<tyr::formalism::FactKind T>
struct formatter<tyr::formalism::datalog::GroundFunctionTermValueView<T>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const tyr::formalism::datalog::GroundFunctionTermValueView<T>& value, FormatContext& ctx) const
    {
        return fmt::format_to(ctx.out(), "(= {} {})", value.get_fterm(), value.get_value());
    }
};

template<>
struct formatter<tyr::formalism::datalog::VariableDependencyGraph, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    auto format(const tyr::formalism::datalog::VariableDependencyGraph& value, format_context& ctx) const -> format_context::iterator;
};

template<>
struct formatter<tyr::Data<tyr::formalism::datalog::FunctionExpression>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    auto format(const tyr::Data<tyr::formalism::datalog::FunctionExpression>& value, format_context& ctx) const -> format_context::iterator;
};

template<>
struct formatter<tyr::formalism::datalog::FunctionExpressionView, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    auto format(const tyr::formalism::datalog::FunctionExpressionView& value, format_context& ctx) const -> format_context::iterator;
};

template<>
struct formatter<tyr::Data<tyr::formalism::datalog::GroundFunctionExpression>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    auto format(const tyr::Data<tyr::formalism::datalog::GroundFunctionExpression>& value, format_context& ctx) const -> format_context::iterator;
};

template<>
struct formatter<tyr::formalism::datalog::GroundFunctionExpressionView, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    auto format(const tyr::formalism::datalog::GroundFunctionExpressionView& value, format_context& ctx) const -> format_context::iterator;
};

template<>
struct formatter<tyr::Data<tyr::formalism::datalog::ConjunctiveCondition>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    auto format(const tyr::Data<tyr::formalism::datalog::ConjunctiveCondition>& value, format_context& ctx) const -> format_context::iterator;
};

template<>
struct formatter<tyr::formalism::datalog::ConjunctiveConditionView, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    auto format(const tyr::formalism::datalog::ConjunctiveConditionView& value, format_context& ctx) const -> format_context::iterator;
};

template<>
struct formatter<tyr::Data<tyr::formalism::datalog::Rule>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    auto format(const tyr::Data<tyr::formalism::datalog::Rule>& value, format_context& ctx) const -> format_context::iterator;
};

template<>
struct formatter<tyr::formalism::datalog::RuleView, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    auto format(const tyr::formalism::datalog::RuleView& value, format_context& ctx) const -> format_context::iterator;
};

template<>
struct formatter<tyr::Data<tyr::formalism::datalog::GroundConjunctiveCondition>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    auto format(const tyr::Data<tyr::formalism::datalog::GroundConjunctiveCondition>& value, format_context& ctx) const -> format_context::iterator;
};

template<>
struct formatter<tyr::formalism::datalog::GroundConjunctiveConditionView, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    auto format(const tyr::formalism::datalog::GroundConjunctiveConditionView& value, format_context& ctx) const -> format_context::iterator;
};

template<>
struct formatter<tyr::Data<tyr::formalism::datalog::GroundRule>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    auto format(const tyr::Data<tyr::formalism::datalog::GroundRule>& value, format_context& ctx) const -> format_context::iterator;
};

template<>
struct formatter<tyr::formalism::datalog::GroundRuleView, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    auto format(const tyr::formalism::datalog::GroundRuleView& value, format_context& ctx) const -> format_context::iterator;
};

template<>
struct formatter<tyr::Data<tyr::formalism::datalog::Program>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    auto format(const tyr::Data<tyr::formalism::datalog::Program>& value, format_context& ctx) const -> format_context::iterator;
};

template<>
struct formatter<tyr::formalism::datalog::ProgramView, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    auto format(const tyr::formalism::datalog::ProgramView& value, format_context& ctx) const -> format_context::iterator;
};

}  // namespace fmt
#endif
