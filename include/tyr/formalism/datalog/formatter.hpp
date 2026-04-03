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

namespace tyr
{
/**
 * Forward declarations
 */
namespace formalism::datalog
{
std::ostream& operator<<(std::ostream& os, const VariableDependencyGraph& el);

template<OpKind Op, typename T>
std::ostream& operator<<(std::ostream& os, const Data<UnaryOperator<Op, T>>& el);

template<OpKind Op, typename T>
std::ostream& operator<<(std::ostream& os, const UnaryOperatorView<Op, T>& el);

template<OpKind Op, typename T>
std::ostream& operator<<(std::ostream& os, const Data<BinaryOperator<Op, T>>& el);

template<OpKind Op, typename T>
std::ostream& operator<<(std::ostream& os, const BinaryOperatorView<Op, T>& el);

template<OpKind Op, typename T>
std::ostream& operator<<(std::ostream& os, const Data<MultiOperator<Op, T>>& el);

template<OpKind Op, typename T>
std::ostream& operator<<(std::ostream& os, const MultiOperatorView<Op, T>& el);

template<typename T>
std::ostream& operator<<(std::ostream& os, const Data<ArithmeticOperator<T>>& el);

template<typename T>
std::ostream& operator<<(std::ostream& os, const ArithmeticOperatorView<T>& el);

template<typename T>
std::ostream& operator<<(std::ostream& os, const Data<BooleanOperator<T>>& el);

template<typename T>
std::ostream& operator<<(std::ostream& os, const BooleanOperatorView<T>& el);

template<FactKind T>
std::ostream& operator<<(std::ostream& os, const Data<Atom<T>>& el);

template<FactKind T>
std::ostream& operator<<(std::ostream& os, const AtomView<T>& el);

template<FactKind T>
std::ostream& operator<<(std::ostream& os, const Data<Literal<T>>& el);

template<FactKind T>
std::ostream& operator<<(std::ostream& os, const LiteralView<T>& el);

template<FactKind T>
std::ostream& operator<<(std::ostream& os, const Data<GroundAtom<T>>& el);

template<FactKind T>
std::ostream& operator<<(std::ostream& os, const GroundAtomView<T>& el);

template<FactKind T>
std::ostream& operator<<(std::ostream& os, const Data<GroundLiteral<T>>& el);

template<FactKind T>
std::ostream& operator<<(std::ostream& os, const GroundLiteralView<T>& el);

template<FactKind T>
std::ostream& operator<<(std::ostream& os, const Data<FunctionTerm<T>>& el);

template<FactKind T>
std::ostream& operator<<(std::ostream& os, const FunctionTermView<T>& el);

template<FactKind T>
std::ostream& operator<<(std::ostream& os, const Data<GroundFunctionTerm<T>>& el);

template<FactKind T>
std::ostream& operator<<(std::ostream& os, const GroundFunctionTermView<T>& el);

template<FactKind T>
std::ostream& operator<<(std::ostream& os, const Data<GroundFunctionTermValue<T>>& el);

template<FactKind T>
std::ostream& operator<<(std::ostream& os, const GroundFunctionTermValueView<T>& el);

std::ostream& operator<<(std::ostream& os, const Data<FunctionExpression>& el);

std::ostream& operator<<(std::ostream& os, const FunctionExpressionView& el);

std::ostream& operator<<(std::ostream& os, const Data<GroundFunctionExpression>& el);

std::ostream& operator<<(std::ostream& os, const GroundFunctionExpressionView& el);

std::ostream& operator<<(std::ostream& os, const Data<ConjunctiveCondition>& el);

std::ostream& operator<<(std::ostream& os, const ConjunctiveConditionView& el);

std::ostream& operator<<(std::ostream& os, const Data<Rule>& el);

std::ostream& operator<<(std::ostream& os, const RuleView& el);

std::ostream& operator<<(std::ostream& os, const Data<GroundConjunctiveCondition>& el);

std::ostream& operator<<(std::ostream& os, const GroundConjunctiveConditionView& el);

std::ostream& operator<<(std::ostream& os, const Data<GroundRule>& el);

std::ostream& operator<<(std::ostream& os, const GroundRuleView& el);

std::ostream& operator<<(std::ostream& os, const Data<Program>& el);

std::ostream& operator<<(std::ostream& os, const ProgramView& el);

}  // end namespace formalism

/**
 * Definitions
 */

template<formalism::OpKind Op, typename T>
std::ostream& print(std::ostream& os, const Data<formalism::datalog::UnaryOperator<Op, T>>& el)
{
    fmt::print(os, "({} {})", to_string(Op {}), to_string(el.arg));
    return os;
}

template<formalism::OpKind Op, typename T>
std::ostream& print(std::ostream& os, const formalism::datalog::UnaryOperatorView<Op, T>& el)
{
    fmt::print(os, "({} {})", to_string(Op {}), to_string(el.get_arg()));
    return os;
}

template<formalism::OpKind Op, typename T>
std::ostream& print(std::ostream& os, const Data<formalism::datalog::BinaryOperator<Op, T>>& el)
{
    fmt::print(os, "({} {} {})", to_string(Op {}), to_string(el.lhs), to_string(el.rhs));
    return os;
}

template<formalism::OpKind Op, typename T>
std::ostream& print(std::ostream& os, const formalism::datalog::BinaryOperatorView<Op, T>& el)
{
    fmt::print(os, "({} {} {})", to_string(Op {}), to_string(el.get_lhs()), to_string(el.get_rhs()));
    return os;
}

template<formalism::OpKind Op, typename T>
std::ostream& print(std::ostream& os, const Data<formalism::datalog::MultiOperator<Op, T>>& el)
{
    fmt::print(os, "({} {})", to_string(Op {}), fmt::format("{}", fmt::join(to_strings(el.args), " ")));
    return os;
}

template<formalism::OpKind Op, typename T>
std::ostream& print(std::ostream& os, const formalism::datalog::MultiOperatorView<Op, T>& el)
{
    fmt::print(os, "({} {})", to_string(Op {}), fmt::format("{}", fmt::join(to_strings(el.get_args()), " ")));
    return os;
}

template<typename T>
std::ostream& print(std::ostream& os, const Data<formalism::datalog::ArithmeticOperator<T>>& el)
{
    fmt::print(os, "{}", to_string(el.value));
    return os;
}

template<typename T>
std::ostream& print(std::ostream& os, const formalism::datalog::ArithmeticOperatorView<T>& el)
{
    fmt::print(os, "{}", to_string(el.get_variant()));
    return os;
}

template<typename T>
std::ostream& print(std::ostream& os, const Data<formalism::datalog::BooleanOperator<T>>& el)
{
    fmt::print(os, "{}", to_string(el.value));
    return os;
}

template<typename T>
std::ostream& print(std::ostream& os, const formalism::datalog::BooleanOperatorView<T>& el)
{
    fmt::print(os, "{}", to_string(el.get_variant()));
    return os;
}

template<formalism::FactKind T>
std::ostream& print(std::ostream& os, const Data<formalism::datalog::Atom<T>>& el)
{
    fmt::print(os, "({} {})", to_string(el.predicate), fmt::format("{}", fmt::join(to_strings(el.terms), " ")));
    return os;
}

template<formalism::FactKind T>
std::ostream& print(std::ostream& os, const formalism::datalog::AtomView<T>& el)
{
    fmt::print(os, "({} {})", to_string(el.get_predicate().get_name()), fmt::format("{}", fmt::join(to_strings(el.get_terms()), " ")));
    return os;
}

template<formalism::FactKind T>
std::ostream& print(std::ostream& os, const Data<formalism::datalog::Literal<T>>& el)
{
    if (el.polarity)
        print(os, to_string(el.atom));
    else
        fmt::print(os, "(not {})", to_string(el.atom));
    return os;
}

template<formalism::FactKind T>
std::ostream& print(std::ostream& os, const formalism::datalog::LiteralView<T>& el)
{
    if (el.get_polarity())
        print(os, to_string(el.get_atom()));
    else
        fmt::print(os, "(not {})", to_string(el.get_atom()));
    return os;
}

template<formalism::FactKind T>
std::ostream& print(std::ostream& os, const Data<formalism::datalog::GroundAtom<T>>& el)
{
    fmt::print(os, "({} {})", to_string(el.index.group), fmt::format("{}", fmt::join(to_strings(el.objects), " ")));
    return os;
}

template<formalism::FactKind T>
std::ostream& print(std::ostream& os, const formalism::datalog::GroundAtomView<T>& el)
{
    fmt::print(os, "({} {})", to_string(el.get_predicate().get_name()), fmt::format("{}", fmt::join(to_strings(el.get_row().get_objects()), " ")));
    return os;
}

template<formalism::FactKind T>
std::ostream& print(std::ostream& os, const Data<formalism::datalog::GroundLiteral<T>>& el)
{
    if (el.polarity)
        print(os, to_string(el.atom));
    else
        fmt::print(os, "(not {})", to_string(el.atom));
    return os;
}

template<formalism::FactKind T>
std::ostream& print(std::ostream& os, const formalism::datalog::GroundLiteralView<T>& el)
{
    if (el.get_polarity())
        print(os, to_string(el.get_atom()));
    else
        fmt::print(os, "(not {})", to_string(el.get_atom()));
    return os;
}

template<formalism::FactKind T>
std::ostream& print(std::ostream& os, const Data<formalism::datalog::FunctionTerm<T>>& el)
{
    fmt::print(os, "({} {})", to_string(el.function), fmt::format("{}", fmt::join(to_strings(el.terms), " ")));
    return os;
}

template<formalism::FactKind T>
std::ostream& print(std::ostream& os, const formalism::datalog::FunctionTermView<T>& el)
{
    fmt::print(os, "({} {})", to_string(el.get_function().get_name()), fmt::format("{}", fmt::join(to_strings(el.get_terms()), " ")));
    return os;
}

template<formalism::FactKind T>
std::ostream& print(std::ostream& os, const Data<formalism::datalog::GroundFunctionTerm<T>>& el)
{
    fmt::print(os, "({} {})", to_string(el.index.group), fmt::format("{}", fmt::join(to_strings(el.objects), " ")));
    return os;
}

template<formalism::FactKind T>
std::ostream& print(std::ostream& os, const formalism::datalog::GroundFunctionTermView<T>& el)
{
    fmt::print(os, "({} {})", to_string(el.get_function().get_name()), fmt::format("{}", fmt::join(to_strings(el.get_row().get_objects()), " ")));
    return os;
}

template<formalism::FactKind T>
std::ostream& print(std::ostream& os, const Data<formalism::datalog::GroundFunctionTermValue<T>>& el)
{
    fmt::print(os, "(= {} {})", to_string(el.fterm), to_string(el.value));
    return os;
}

template<formalism::FactKind T>
std::ostream& print(std::ostream& os, const formalism::datalog::GroundFunctionTermValueView<T>& el)
{
    fmt::print(os, "(= {} {})", to_string(el.get_fterm()), to_string(el.get_value()));
    return os;
}

namespace formalism::datalog
{

template<OpKind Op, typename T>
std::ostream& operator<<(std::ostream& os, const Data<UnaryOperator<Op, T>>& el)
{
    return tyr::print(os, el);
}

template<OpKind Op, typename T>
std::ostream& operator<<(std::ostream& os, const UnaryOperatorView<Op, T>& el)
{
    return tyr::print(os, el);
}

template<OpKind Op, typename T>
std::ostream& operator<<(std::ostream& os, const Data<BinaryOperator<Op, T>>& el)
{
    return tyr::print(os, el);
}

template<OpKind Op, typename T>
std::ostream& operator<<(std::ostream& os, const BinaryOperatorView<Op, T>& el)
{
    return tyr::print(os, el);
}

template<OpKind Op, typename T>
std::ostream& operator<<(std::ostream& os, const Data<MultiOperator<Op, T>>& el)
{
    return tyr::print(os, el);
}

template<OpKind Op, typename T>
std::ostream& operator<<(std::ostream& os, const MultiOperatorView<Op, T>& el)
{
    return tyr::print(os, el);
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const Data<ArithmeticOperator<T>>& el)
{
    return tyr::print(os, el);
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const ArithmeticOperatorView<T>& el)
{
    return tyr::print(os, el);
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const Data<BooleanOperator<T>>& el)
{
    return tyr::print(os, el);
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const BooleanOperatorView<T>& el)
{
    return tyr::print(os, el);
}

template<FactKind T>
std::ostream& operator<<(std::ostream& os, const Data<Atom<T>>& el)
{
    return tyr::print(os, el);
}

template<FactKind T>
std::ostream& operator<<(std::ostream& os, const AtomView<T>& el)
{
    return tyr::print(os, el);
}

template<FactKind T>
std::ostream& operator<<(std::ostream& os, const Data<Literal<T>>& el)
{
    return tyr::print(os, el);
}

template<FactKind T>
std::ostream& operator<<(std::ostream& os, const LiteralView<T>& el)
{
    return tyr::print(os, el);
}

template<FactKind T>
std::ostream& operator<<(std::ostream& os, const Data<GroundAtom<T>>& el)
{
    return tyr::print(os, el);
}

template<FactKind T>
std::ostream& operator<<(std::ostream& os, const GroundAtomView<T>& el)
{
    return tyr::print(os, el);
}

template<FactKind T>
std::ostream& operator<<(std::ostream& os, const Data<GroundLiteral<T>>& el)
{
    return tyr::print(os, el);
}

template<FactKind T>
std::ostream& operator<<(std::ostream& os, const GroundLiteralView<T>& el)
{
    return tyr::print(os, el);
}

template<FactKind T>
std::ostream& operator<<(std::ostream& os, const Data<FunctionTerm<T>>& el)
{
    return tyr::print(os, el);
}

template<FactKind T>
std::ostream& operator<<(std::ostream& os, const FunctionTermView<T>& el)
{
    return tyr::print(os, el);
}

template<FactKind T>
std::ostream& operator<<(std::ostream& os, const Data<GroundFunctionTerm<T>>& el)
{
    return tyr::print(os, el);
}

template<FactKind T>
std::ostream& operator<<(std::ostream& os, const GroundFunctionTermView<T>& el)
{
    return tyr::print(os, el);
}

template<FactKind T>
std::ostream& operator<<(std::ostream& os, const Data<GroundFunctionTermValue<T>>& el)
{
    return tyr::print(os, el);
}

template<FactKind T>
std::ostream& operator<<(std::ostream& os, const GroundFunctionTermValueView<T>& el)
{
    return tyr::print(os, el);
}

}
}
#endif
