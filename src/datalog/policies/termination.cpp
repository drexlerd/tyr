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

#include "tyr/datalog/policies/termination.hpp"

#include "tyr/common/closed_interval.hpp"
#include "tyr/common/config.hpp"
#include "tyr/common/dynamic_bitset.hpp"
#include "tyr/datalog/assignment.hpp"
#include "tyr/datalog/assignment_sets.hpp"
#include "tyr/datalog/fact_sets.hpp"
#include "tyr/datalog/policies/aggregation.hpp"
#include "tyr/formalism/arithmetic_operator_utils.hpp"
#include "tyr/formalism/boolean_operator_utils.hpp"
#include "tyr/formalism/datalog/declarations.hpp"
#include "tyr/formalism/datalog/ground_atom_index.hpp"
#include "tyr/formalism/datalog/repository.hpp"
#include "tyr/formalism/datalog/views.hpp"

#include <boost/dynamic_bitset.hpp>
#include <concepts>
#include <iterator>
#include <numeric>
#include <type_traits>

namespace tyr::datalog
{
namespace
{
ClosedInterval<float_t> evaluate_interval(formalism::datalog::GroundFunctionExpressionView element, const AssignmentSets& assignment_sets);
ClosedInterval<float_t> evaluate_interval(formalism::datalog::GroundArithmeticOperatorView element, const AssignmentSets& assignment_sets);

template<formalism::FactKind T>
ClosedInterval<float_t> evaluate_interval(formalism::datalog::GroundFunctionTermView<T> element, const AssignmentSets& assignment_sets)
{
    return assignment_sets.template get<T>().function[element.get_row()];
}

ClosedInterval<float_t> evaluate_interval(float_t element, const AssignmentSets&) { return ClosedInterval<float_t>(element, element); }

template<formalism::ArithmeticOpKind O>
ClosedInterval<float_t> evaluate_interval(formalism::datalog::GroundUnaryOperatorView<O> element, const AssignmentSets& assignment_sets)
{
    return formalism::apply(O {}, evaluate_interval(element.get_arg(), assignment_sets));
}

template<formalism::ArithmeticOpKind O>
ClosedInterval<float_t> evaluate_interval(formalism::datalog::GroundBinaryOperatorView<O> element, const AssignmentSets& assignment_sets)
{
    return formalism::apply(O {}, evaluate_interval(element.get_lhs(), assignment_sets), evaluate_interval(element.get_rhs(), assignment_sets));
}

template<formalism::ArithmeticOpKind O>
ClosedInterval<float_t> evaluate_interval(formalism::datalog::GroundMultiOperatorView<O> element, const AssignmentSets& assignment_sets)
{
    const auto child_fexprs = element.get_args();

    return std::accumulate(std::next(child_fexprs.begin()),
                           child_fexprs.end(),
                           evaluate_interval(child_fexprs.front(), assignment_sets),
                           [&](const auto& value, const auto& child_expr)
                           { return formalism::apply(O {}, value, evaluate_interval(child_expr, assignment_sets)); });
}

ClosedInterval<float_t> evaluate_interval(formalism::datalog::GroundFunctionExpressionView element, const AssignmentSets& assignment_sets)
{
    return visit([&](auto&& arg) { return evaluate_interval(arg, assignment_sets); }, element.get_variant());
}

ClosedInterval<float_t> evaluate_interval(formalism::datalog::GroundArithmeticOperatorView element, const AssignmentSets& assignment_sets)
{
    return visit([&](auto&& arg) { return evaluate_interval(arg, assignment_sets); }, element.get_variant());
}

bool evaluate_existential(formalism::datalog::GroundBooleanOperatorView element, const AssignmentSets& assignment_sets)
{
    return visit(
        [&](auto&& arg) -> bool
        {
            using Alternative = std::decay_t<decltype(arg)>;

            return formalism::apply_existential(typename Alternative::OpType {},
                                                evaluate_interval(arg.get_lhs(), assignment_sets),
                                                evaluate_interval(arg.get_rhs(), assignment_sets));
        },
        element.get_variant());
}
}

template<typename AggregationFunction>
TerminationPolicy<AggregationFunction>::TerminationPolicy(formalism::datalog::PredicateListView<formalism::FluentTag> fluent_predicates,
                                                          const formalism::datalog::Repository& repository) :
    unsat_goals(fluent_predicates.size()),
    num_unsat_goals(0),
    goal_fact_sets(fluent_predicates, repository),
    goal(std::nullopt),
    bindings()
{
}

template<typename AggregationFunction>
void TerminationPolicy<AggregationFunction>::set_goals(formalism::datalog::GroundConjunctiveConditionView goals)
{
    clear();
    goal = goals;

    num_unsat_goals = 0;
    for (const auto literal : goals.get_literals<formalism::FluentTag>())
    {
        if (!literal.get_polarity())
            continue;

        goal_fact_sets.insert(literal.get_atom());
    }

    for (const auto& set : goal_fact_sets.get_sets())
    {
        for (const auto& binding : set.get_bindings())
        {
            const auto g = uint_t(binding.get_index().relation);
            const auto i = uint_t(binding.get_index().row);

            tyr::set(i, true, unsat_goals[g]);
            ++num_unsat_goals;
            bindings.push_back(binding);
        }
    }
}

template<typename AggregationFunction>
void TerminationPolicy<AggregationFunction>::achieve(formalism::datalog::PredicateBindingView<formalism::FluentTag> binding) noexcept
{
    const auto g = uint_t(binding.get_index().relation);
    const auto i = uint_t(binding.get_index().row);

    if (tyr::test(i, unsat_goals[g]))
    {
        --num_unsat_goals;
        tyr::set(i, false, unsat_goals[g]);
    }
}

template<typename AggregationFunction>
bool TerminationPolicy<AggregationFunction>::check(const AssignmentSets& assignment_sets) const noexcept
{
    if (num_unsat_goals != 0)
        return false;

    if (!goal)
        return true;

    for (const auto constraint : goal->get_numeric_constraints())
        if (!evaluate_existential(constraint, assignment_sets))
            return false;

    return true;
}

template<typename AggregationFunction>
Cost TerminationPolicy<AggregationFunction>::get_total_cost(const OrAnnotationsList& or_annot) const noexcept
{
    auto cost = AggregationFunction::identity();

    for (const auto binding : bindings)
    {
        const auto g = uint_t(binding.get_index().relation);
        const auto i = uint_t(binding.get_index().row);
        assert(g < or_annot.size());
        assert(i < or_annot[g].size());

        cost = agg(cost, or_annot[g][i]);
    }

    return cost;
}

template<typename AggregationFunction>
void TerminationPolicy<AggregationFunction>::reset() noexcept
{
    num_unsat_goals = 0;
    for (auto& bitset : unsat_goals)
        bitset.reset();

    for (const auto& binding : bindings)
    {
        const auto g = uint_t(binding.get_index().relation);
        const auto i = uint_t(binding.get_index().row);

        tyr::set(i, true, unsat_goals[g]);
        ++num_unsat_goals;
    }
}

template<typename AggregationFunction>
void TerminationPolicy<AggregationFunction>::clear() noexcept
{
    num_unsat_goals = 0;
    for (auto& bitset : unsat_goals)
        bitset.reset();

    goal_fact_sets.reset();
    goal = std::nullopt;
    bindings.clear();
}

template class TerminationPolicy<SumAggregation>;
template class TerminationPolicy<MaxAggregation>;

}
