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

#ifndef TYR_SOLVER_POLICIES_TERMINATION_HPP_
#define TYR_SOLVER_POLICIES_TERMINATION_HPP_

#include "tyr/common/config.hpp"
#include "tyr/datalog/fact_sets.hpp"
#include "tyr/datalog/policies/aggregation.hpp"
#include "tyr/datalog/policies/annotation_types.hpp"
#include "tyr/datalog/policies/termination_concept.hpp"
#include "tyr/formalism/datalog/declarations.hpp"
#include "tyr/formalism/datalog/ground_atom_index.hpp"
#include "tyr/formalism/datalog/repository.hpp"
#include "tyr/formalism/datalog/views.hpp"

#include <concepts>
#include <optional>

namespace tyr::datalog
{

class NumericSupportSelector;

class NoTerminationPolicy
{
public:
    NoTerminationPolicy() = default;

    void set_goals(formalism::datalog::GroundConjunctiveConditionView goals) {}
    bool check(const FactSets& fact_sets) const noexcept { return false; }
    Cost get_total_cost(const FactSets& fact_sets,
                        const SelectedPredicateAnnotations& and_annot,
                        const SelectedFunctionAnnotations& numeric_and_annot,
                        const NumericSupportSelector& numeric_support_selector) const noexcept
    {
        return Cost(0);
    }
    void reset() noexcept {}
    void clear() noexcept {}
};

template<typename AggregationFunction>
class TerminationPolicy
{
public:
    TerminationPolicy(formalism::datalog::PredicateListView<formalism::FluentTag> fluent_predicates,
                      const formalism::datalog::Repository& repository);

    void set_goals(formalism::datalog::GroundConjunctiveConditionView goals);

    bool check(const FactSets& fact_sets) const noexcept;

    Cost get_total_cost(const FactSets& fact_sets,
                        const SelectedPredicateAnnotations& and_annot,
                        const SelectedFunctionAnnotations& numeric_and_annot,
                        const NumericSupportSelector& numeric_support_selector) const noexcept;

    void reset() noexcept;

    void clear() noexcept;

    const std::vector<formalism::datalog::PredicateBindingView<formalism::FluentTag>>& get_predicate_bindings() const noexcept { return predicate_bindings; }
    const std::vector<formalism::datalog::FunctionBindingView<formalism::FluentTag>>& get_function_bindings() const noexcept { return function_bindings; }

private:
    PredicateFactSets<formalism::FluentTag> goal_fact_sets;
    std::optional<formalism::datalog::GroundConjunctiveConditionView> goal;
    std::vector<formalism::datalog::PredicateBindingView<formalism::FluentTag>> predicate_bindings;
    std::vector<formalism::datalog::FunctionBindingView<formalism::FluentTag>> function_bindings;

    AggregationFunction agg;
};
}

#endif
