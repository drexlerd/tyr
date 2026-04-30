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

#include "tyr/analysis/listeners.hpp"

#include "tyr/analysis/stratification.hpp"                       // for RuleStrata
#include "tyr/common/index_mixins.hpp"                           // for operator!=
#include "tyr/common/types.hpp"                                  // for make_view
#include "tyr/common/vector.hpp"                                 // for View
#include "tyr/formalism/datalog/atom_view.hpp"                   // for View
#include "tyr/formalism/datalog/conjunctive_condition_view.hpp"  // for View
#include "tyr/formalism/datalog/literal_index.hpp"               // for Index
#include "tyr/formalism/datalog/literal_view.hpp"                // for View
#include "tyr/formalism/datalog/repository.hpp"                  // for Repository
#include "tyr/formalism/datalog/rule_view.hpp"                   // for View
#include "tyr/formalism/predicate_view.hpp"                      // for View

#include <cista/containers/vector.h>  // for basic_vector
#include <gtl/phmap.hpp>              // for flat_hash_set
#include <type_traits>
#include <utility>                    // for move

namespace f = tyr::formalism;
namespace fd = tyr::formalism::datalog;

namespace tyr::analysis
{
namespace
{
void add_function_listeners(fd::FunctionExpressionView expression, Index<fd::Rule> rule, ListenerStratum& listeners);

void add_function_listeners(float_t, Index<fd::Rule>, ListenerStratum&)
{
}

template<f::OpKind O>
void add_function_listeners(fd::LiftedUnaryOperatorView<O> expression, Index<fd::Rule> rule, ListenerStratum& listeners)
{
    add_function_listeners(expression.get_arg(), rule, listeners);
}

template<f::OpKind O>
void add_function_listeners(fd::LiftedBinaryOperatorView<O> expression, Index<fd::Rule> rule, ListenerStratum& listeners)
{
    add_function_listeners(expression.get_lhs(), rule, listeners);
    add_function_listeners(expression.get_rhs(), rule, listeners);
}

template<f::OpKind O>
void add_function_listeners(fd::LiftedMultiOperatorView<O> expression, Index<fd::Rule> rule, ListenerStratum& listeners)
{
    for (const auto arg : expression.get_args())
        add_function_listeners(arg, rule, listeners);
}

template<f::FactKind T>
void add_function_listeners(fd::FunctionTermView<T>, Index<fd::Rule>, ListenerStratum&)
{
}

void add_function_listeners(fd::FunctionTermView<f::FluentTag> term, Index<fd::Rule> rule, ListenerStratum& listeners)
{
    listeners.functions[term.get_function().get_index()].insert(rule);
}

void add_function_listeners(fd::LiftedArithmeticOperatorView expression, Index<fd::Rule> rule, ListenerStratum& listeners)
{
    visit([&](auto&& arg) { add_function_listeners(arg, rule, listeners); }, expression.get_variant());
}

void add_function_listeners(fd::FunctionExpressionView expression, Index<fd::Rule> rule, ListenerStratum& listeners)
{
    visit([&](auto&& arg) { add_function_listeners(arg, rule, listeners); }, expression.get_variant());
}

void add_function_listeners(fd::LiftedBooleanOperatorView expression, Index<fd::Rule> rule, ListenerStratum& listeners)
{
    visit(
        [&](auto&& arg)
        {
            add_function_listeners(arg.get_lhs(), rule, listeners);
            add_function_listeners(arg.get_rhs(), rule, listeners);
        },
        expression.get_variant());
}

template<fd::NumericEffectOpKind Op>
void add_numeric_effect_head_listeners(fd::NumericEffectView<Op, f::FluentTag> effect, Index<fd::Rule> rule, ListenerStratum& listeners)
{
    if constexpr (!std::is_same_v<Op, fd::OpAssign>)
        add_function_listeners(effect.get_fterm(), rule, listeners);

    add_function_listeners(effect.get_fexpr(), rule, listeners);
}
}

ListenerStrata compute_listeners(const RuleStrata& strata, const fd::Repository& context)
{
    auto listeners = ListenerStrata();

    for (const auto& stratum : strata.data)
    {
        auto listeners_in_stratum = ListenerStratum {};

        for (const auto rule : stratum)
        {
            const auto rule_view = make_view(rule, context);
            for (const auto literal : make_view(rule, context).get_body().get_literals<f::FluentTag>())
                if (literal.get_polarity())
                    listeners_in_stratum.predicates[literal.get_atom().get_predicate().get_index()].insert(rule);

            for (const auto constraint : rule_view.get_body().get_numeric_constraints())
                add_function_listeners(constraint, rule, listeners_in_stratum);

            visit(
                [&](auto&& head)
                {
                    using Head = std::decay_t<decltype(head)>;

                    if constexpr (std::is_same_v<Head, fd::NumericEffectOperatorView<f::FluentTag>>)
                        visit([&](auto&& effect) { add_numeric_effect_head_listeners(effect, rule, listeners_in_stratum); }, head.get_variant());
                },
                rule_view.get_head());
        }

        listeners.data.push_back(std::move(listeners_in_stratum));
    }

    // std::cout << listeners.data << std::endl;

    return listeners;
}
}
