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
#include "tyr/datalog/policies/numeric_support.hpp"
#include "tyr/formalism/datalog/declarations.hpp"
#include "tyr/formalism/datalog/ground_atom_index.hpp"
#include "tyr/formalism/datalog/repository.hpp"
#include "tyr/formalism/datalog/views.hpp"

#include <concepts>
#include <limits>

namespace tyr::datalog
{
template<typename AggregationFunction>
TerminationPolicy<AggregationFunction>::TerminationPolicy(formalism::datalog::PredicateListView<formalism::FluentTag> fluent_predicates,
                                                          const formalism::datalog::Repository& repository) :
    goal(std::nullopt),
    numeric_support_selector_workspace(),
    agg()
{
}

template<typename AggregationFunction>
void TerminationPolicy<AggregationFunction>::set_goals(formalism::datalog::GroundConjunctiveConditionView goals)
{
    clear();
    goal = goals;
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
                                                            const SelectedPredicateAnnotations& and_annot,
                                                            const SelectedFunctionAnnotations& numeric_and_annot,
                                                            const NumericSupportSelector& numeric_support_selector) const noexcept
{
    auto cost = AggregationFunction::identity();

    if (!goal)
        return cost;

    for (const auto literal : goal->get_literals<formalism::FluentTag>())
    {
        assert(literal.get_polarity());

        const auto* annotation = and_annot.find(literal.get_atom().get_row());
        assert(annotation);
        const auto binding_cost = get_cost(*annotation);
        if (binding_cost == std::numeric_limits<Cost>::max())
            return std::numeric_limits<Cost>::max();
        cost = agg(cost, binding_cost);
    }

    for (const auto constraint : goal->get_numeric_constraints())
    {
        const auto constraint_cost = numeric_support_selector.get_constraint_cost(constraint, numeric_support_selector_workspace, agg);
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
    goal = std::nullopt;
    numeric_support_selector_workspace.clear();
}

template class TerminationPolicy<SumAggregation>;
template class TerminationPolicy<MaxAggregation>;

}
