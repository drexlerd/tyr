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

#include "tyr/common/config.hpp"
#include "tyr/datalog/applicability.hpp"
#include "tyr/datalog/fact_sets.hpp"
#include "tyr/datalog/policies/aggregation.hpp"
#include "tyr/formalism/arithmetic_operator_utils.hpp"
#include "tyr/formalism/boolean_operator_utils.hpp"
#include "tyr/formalism/datalog/declarations.hpp"
#include "tyr/formalism/datalog/expression_properties.hpp"
#include "tyr/formalism/datalog/ground_atom_index.hpp"
#include "tyr/formalism/datalog/repository.hpp"
#include "tyr/formalism/datalog/views.hpp"

#include <algorithm>
#include <concepts>
#include <limits>

namespace tyr::datalog
{
namespace
{
using FluentIntervalSelection =
    std::vector<std::pair<formalism::datalog::FunctionBindingView<formalism::FluentTag>, ClosedInterval<float_t>>>;

ClosedInterval<float_t> evaluate_with_selection(float_t element, const FactSets&, const FluentIntervalSelection&)
{
    return ClosedInterval<float_t>(element, element);
}

ClosedInterval<float_t>
evaluate_with_selection(formalism::datalog::GroundFunctionTermView<formalism::StaticTag> element, const FactSets& fact_sets, const FluentIntervalSelection&)
{
    return fact_sets.get<formalism::StaticTag>().function[element];
}

ClosedInterval<float_t> evaluate_with_selection(formalism::datalog::GroundFunctionTermView<formalism::FluentTag> element,
                                                const FactSets&,
                                                const FluentIntervalSelection& selection)
{
    const auto binding_equal = EqualTo<formalism::datalog::FunctionBindingView<formalism::FluentTag>> {};
    for (const auto& [binding, interval] : selection)
        if (binding_equal(binding, element.get_row()))
            return interval;

    return ClosedInterval<float_t>();
}

ClosedInterval<float_t> evaluate_with_selection(formalism::datalog::GroundFunctionExpressionView element,
                                                const FactSets& fact_sets,
                                                const FluentIntervalSelection& selection);
ClosedInterval<float_t> evaluate_with_selection(formalism::datalog::GroundArithmeticOperatorView element,
                                                const FactSets& fact_sets,
                                                const FluentIntervalSelection& selection);

template<formalism::ArithmeticOpKind O>
ClosedInterval<float_t> evaluate_with_selection(formalism::datalog::GroundUnaryOperatorView<O> element,
                                                const FactSets& fact_sets,
                                                const FluentIntervalSelection& selection)
{
    return formalism::apply(O {}, evaluate_with_selection(element.get_arg(), fact_sets, selection));
}

template<formalism::ArithmeticOpKind O>
ClosedInterval<float_t> evaluate_with_selection(formalism::datalog::GroundBinaryOperatorView<O> element,
                                                const FactSets& fact_sets,
                                                const FluentIntervalSelection& selection)
{
    return formalism::apply(O {},
                            evaluate_with_selection(element.get_lhs(), fact_sets, selection),
                            evaluate_with_selection(element.get_rhs(), fact_sets, selection));
}

template<formalism::ArithmeticOpKind O>
ClosedInterval<float_t> evaluate_with_selection(formalism::datalog::GroundMultiOperatorView<O> element,
                                                const FactSets& fact_sets,
                                                const FluentIntervalSelection& selection)
{
    const auto child_fexprs = element.get_args();
    return std::accumulate(std::next(child_fexprs.begin()),
                           child_fexprs.end(),
                           evaluate_with_selection(child_fexprs.front(), fact_sets, selection),
                           [&](const auto& value, const auto& child_expr)
                           { return formalism::apply(O {}, value, evaluate_with_selection(child_expr, fact_sets, selection)); });
}

ClosedInterval<float_t> evaluate_with_selection(formalism::datalog::GroundFunctionExpressionView element,
                                                const FactSets& fact_sets,
                                                const FluentIntervalSelection& selection)
{
    return visit([&](auto&& arg) { return evaluate_with_selection(arg, fact_sets, selection); }, element.get_variant());
}

ClosedInterval<float_t> evaluate_with_selection(formalism::datalog::GroundArithmeticOperatorView element,
                                                const FactSets& fact_sets,
                                                const FluentIntervalSelection& selection)
{
    return visit([&](auto&& arg) { return evaluate_with_selection(arg, fact_sets, selection); }, element.get_variant());
}

template<formalism::BooleanOpKind O>
bool evaluate_with_selection(formalism::datalog::GroundBinaryOperatorView<O> element, const FactSets& fact_sets, const FluentIntervalSelection& selection)
{
    return formalism::apply_existential(O {},
                                        evaluate_with_selection(element.get_lhs(), fact_sets, selection),
                                        evaluate_with_selection(element.get_rhs(), fact_sets, selection));
}

bool evaluate_with_selection(formalism::datalog::GroundBooleanOperatorView element, const FactSets& fact_sets, const FluentIntervalSelection& selection)
{
    return visit([&](auto&& arg) { return evaluate_with_selection(arg, fact_sets, selection); }, element.get_variant());
}

template<typename AggregationFunction>
Cost get_numeric_constraint_cost(formalism::datalog::GroundBooleanOperatorView constraint,
                                 const FactSets& fact_sets,
                                 const NumericIntervalAnnotations& numeric_interval_annot,
                                 AggregationFunction agg)
{
    const auto fterms = formalism::datalog::collect_fterms<formalism::FluentTag>(constraint);

    if (fterms.empty())
        return evaluate_with_selection(constraint, fact_sets, FluentIntervalSelection {}) ? Cost(0) : std::numeric_limits<Cost>::max();

    for (const auto fterm : fterms)
    {
        auto found = false;
        const auto binding_equal = EqualTo<formalism::datalog::FunctionBindingView<formalism::FluentTag>> {};
        for (const auto& entry : numeric_interval_annot)
            if (binding_equal(entry.binding, fterm.get_row()))
            {
                found = true;
                break;
            }

        if (!found)
            return std::numeric_limits<Cost>::max();
    }

    auto selection = FluentIntervalSelection {};
    selection.reserve(fterms.size());
    auto best_cost = std::numeric_limits<Cost>::max();
    const auto binding_equal = EqualTo<formalism::datalog::FunctionBindingView<formalism::FluentTag>> {};

    auto recurse = [&](auto&& self, size_t pos, Cost cost) -> void
    {
        if (cost >= best_cost)
            return;

        if (pos == fterms.size())
        {
            if (evaluate_with_selection(constraint, fact_sets, selection))
                best_cost = cost;
            return;
        }

        const auto binding = fterms[pos].get_row();
        for (const auto& entry : numeric_interval_annot)
        {
            if (!binding_equal(entry.binding, binding))
                continue;

            selection.emplace_back(binding, entry.interval);
            self(self, pos + 1, agg(cost, get_cost(entry.annotation)));
            selection.pop_back();
        }
    };

    recurse(recurse, 0, AggregationFunction::identity());

    return best_cost;
}
}

template<typename AggregationFunction>
TerminationPolicy<AggregationFunction>::TerminationPolicy(formalism::datalog::PredicateListView<formalism::FluentTag> fluent_predicates,
                                                          const formalism::datalog::Repository& repository) :
    goal_fact_sets(fluent_predicates, repository),
    goal(std::nullopt),
    predicate_bindings(),
    function_bindings()
{
}

template<typename AggregationFunction>
void TerminationPolicy<AggregationFunction>::set_goals(formalism::datalog::GroundConjunctiveConditionView goals)
{
    clear();
    goal = goals;

    for (const auto literal : goals.get_literals<formalism::FluentTag>())
    {
        if (!literal.get_polarity())
            continue;

        goal_fact_sets.insert(literal.get_atom());
    }

    for (const auto& set : goal_fact_sets.get_sets())
    {
        for (const auto& binding : set.get_bindings())
            predicate_bindings.push_back(binding);
    }

    for (const auto constraint : goals.get_numeric_constraints())
        for (const auto fterm : formalism::datalog::collect_fterms<formalism::FluentTag>(constraint))
            function_bindings.push_back(fterm.get_row());
}

template<typename AggregationFunction>
bool TerminationPolicy<AggregationFunction>::check(const FactSets& fact_sets) const noexcept
{
    if (!goal)
        return false;

    return is_applicable(*goal, fact_sets);
}

template<typename AggregationFunction>
Cost TerminationPolicy<AggregationFunction>::get_total_cost(const FactSets& fact_sets,
                                                            const AndAnnotationsMap& and_annot,
                                                            const NumericAndAnnotationsMap& numeric_and_annot,
                                                            const NumericIntervalAnnotations& numeric_interval_annot) const noexcept
{
    auto cost = AggregationFunction::identity();

    for (const auto binding : predicate_bindings)
    {
        const auto it = and_annot.find(binding);
        assert(it != and_annot.end());
        const auto binding_cost = get_cost(it->second);
        if (binding_cost == std::numeric_limits<Cost>::max())
            return std::numeric_limits<Cost>::max();
        cost = agg(cost, binding_cost);
    }

    if (goal)
        for (const auto constraint : goal->get_numeric_constraints())
        {
            const auto constraint_cost = get_numeric_constraint_cost(constraint, fact_sets, numeric_interval_annot, agg);
            if (constraint_cost == std::numeric_limits<Cost>::max())
                return std::numeric_limits<Cost>::max();
            cost = agg(cost, constraint_cost);
        }

    return cost;
}

template<typename AggregationFunction>
void TerminationPolicy<AggregationFunction>::reset() noexcept
{
}

template<typename AggregationFunction>
void TerminationPolicy<AggregationFunction>::clear() noexcept
{
    goal_fact_sets.reset();
    goal = std::nullopt;
    predicate_bindings.clear();
    function_bindings.clear();
}

template class TerminationPolicy<SumAggregation>;
template class TerminationPolicy<MaxAggregation>;

}
