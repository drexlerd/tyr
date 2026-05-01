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

#include "tyr/datalog/policies/annotation.hpp"

#include "tyr/common/config.hpp"
#include "tyr/common/vector.hpp"
#include "tyr/datalog/policies/aggregation.hpp"
#include "tyr/formalism/binding_index.hpp"
#include "tyr/formalism/datalog/builder.hpp"
#include "tyr/formalism/datalog/canonicalization.hpp"
#include "tyr/formalism/datalog/declarations.hpp"
#include "tyr/formalism/datalog/formatter.hpp"
#include "tyr/formalism/datalog/ground_atom_index.hpp"
#include "tyr/formalism/datalog/ground_conjunctive_condition_index.hpp"
#include "tyr/formalism/datalog/grounder.hpp"
#include "tyr/formalism/datalog/repository.hpp"
#include "tyr/formalism/datalog/rule_index.hpp"

#include <algorithm>
#include <cassert>
#include <concepts>
#include <limits>
#include <optional>
#include <tuple>
#include <vector>

namespace tyr::datalog
{

/**
 * OrAnnotationPolicy
 */

namespace
{
CostUpdate update_min_cost(Cost& cost, Cost candidate)
{
    const auto old_cost = cost;

    if (candidate < old_cost)
        cost = candidate;

    return CostUpdate(old_cost, cost);
}

std::optional<Witness> fetch_best_head_witness_cost(formalism::datalog::PredicateBindingView<formalism::FluentTag> delta_head,
                                                    const AndAnnotationsMap& delta_and_annot)
{
    if (auto it = delta_and_annot.find(delta_head); it != delta_and_annot.end())
        if (const auto* witness = std::get_if<Witness>(&it->second))
            return *witness;

    return std::nullopt;  // No witness available (not derived yet / skipped / not tracked) -> no update from AND side
}

uint_t fetch_annotation_cost(formalism::datalog::PredicateBindingView<formalism::FluentTag> binding, const AndAnnotationsMap& annotations)
{
    if (const auto it = annotations.find(binding); it != annotations.end())
        return get_cost(it->second);
    return std::numeric_limits<uint_t>::max();
}

}

void OrAnnotationPolicy::initialize_annotation(formalism::datalog::PredicateBindingView<formalism::FluentTag> program_head,
                                               AndAnnotationsMap& program_and_annot) const
{
    program_and_annot.insert_or_assign(program_head, BaseCase(Cost(0)));
}

void OrAnnotationPolicy::initialize_annotation(formalism::datalog::FunctionBindingView<formalism::FluentTag> program_head,
                                               NumericAndAnnotationsMap& program_numeric_and_annot) const
{
    program_numeric_and_annot.insert_or_assign(program_head, BaseCase(Cost(0)));
}

CostUpdate OrAnnotationPolicy::update_annotation(formalism::datalog::PredicateBindingView<formalism::FluentTag> program_head,
                                                 formalism::datalog::PredicateBindingView<formalism::FluentTag> delta_head,
                                                 const AndAnnotationsMap& delta_and_annot,
                                                 AndAnnotationsMap& program_and_annot) const
{
    // Fast path 1: already optimal
    auto old_cost = fetch_annotation_cost(program_head, program_and_annot);
    auto or_cost = old_cost;
    if (or_cost == Cost(0))
        return CostUpdate(or_cost, or_cost);

    const auto result = fetch_best_head_witness_cost(delta_head, delta_and_annot);

    // Fast path 2: no witness available => no update
    if (!result)
        return CostUpdate(or_cost, or_cost);

    const auto witness = result.value();

    const auto cost_update = update_min_cost(or_cost, witness.get_cost());

    if (or_cost < old_cost)
        program_and_annot.insert_or_assign(program_head, Annotation(witness));

    return cost_update;
}

/**
 * AndAnnotationPolicy
 */

namespace
{

uint_t fetch_current_best_cost(formalism::datalog::PredicateBindingView<formalism::FluentTag> delta_head, const AndAnnotationsMap& delta_and_annot)
{
    return fetch_annotation_cost(delta_head, delta_and_annot);
}

template<typename AggregationFunction>
std::optional<Witness> try_ground_better_witness(uint_t best_cost,
                                                 uint_t current_cost,
                                                 formalism::datalog::RuleView rule,
                                                 formalism::datalog::ConjunctiveConditionView witness_condition,
                                                 formalism::datalog::GrounderContext& delta_context,
                                                 formalism::datalog::GrounderContext& iteration_context,
                                                 const AndAnnotationsMap& program_and_annot,
                                                 const NumericAndAnnotationsMap& program_numeric_and_annot)
{
    auto body_cost = AggregationFunction::identity();
    const auto rule_cost = rule.get_cost();

    if (best_cost <= body_cost + rule_cost)
        return std::nullopt;  ///< No local or global improvement

    for (const auto literal : witness_condition.get_literals<formalism::FluentTag>())
    {
        assert(literal.get_polarity());

        const auto [program_binding, inserted] = formalism::datalog::ground_binding(literal.get_atom(), iteration_context);
        assert(!inserted);  ///< must exist in program because the precondition is applicable in program fact set.

        const auto program_binding_cost = fetch_annotation_cost(program_binding, program_and_annot);
        assert(program_binding_cost != std::numeric_limits<uint_t>::max());

        body_cost = AggregationFunction()(body_cost, program_binding_cost);

        if (best_cost <= body_cost + rule_cost)
            return std::nullopt;  ///< No local or global improvement
    }

    body_cost = std::max(body_cost, current_cost);

    if (best_cost <= body_cost + rule_cost)
        return std::nullopt;  ///< No local or global improvement

    const auto witness_cost = body_cost + rule_cost;

    const auto delta_binding = formalism::datalog::ground_binding(rule, delta_context).first;

    return Witness(delta_binding, witness_cost);
}
}

template<typename AggregationFunction>
void AndAnnotationPolicy<AggregationFunction>::update_annotation(formalism::datalog::PredicateBindingView<formalism::FluentTag> program_head,
                                                                 formalism::datalog::PredicateBindingView<formalism::FluentTag> delta_head,
                                                                 uint_t current_cost,
                                                                 formalism::datalog::RuleView rule,
                                                                 formalism::datalog::ConjunctiveConditionView witness_condition,
                                                                 const AndAnnotationsMap& program_and_annot,
                                                                 const NumericAndAnnotationsMap& program_numeric_and_annot,
                                                                 AndAnnotationsMap& delta_and_annot,
                                                                 formalism::datalog::GrounderContext& delta_context,
                                                                 formalism::datalog::GrounderContext& iteration_context) const
{
    // Use min among global minimum in cost of last iteration and thread local minimum.
    const auto best_global_cost = fetch_annotation_cost(program_head, program_and_annot);
    const auto best_local_cost = fetch_current_best_cost(delta_head, delta_and_annot);
    const auto best_cost = std::min(best_global_cost, best_local_cost);
    const auto cur_cost_lower_bound = current_cost + rule.get_cost();

    if (best_cost <= cur_cost_lower_bound)
        return;  ///< No local or global improvement

    const auto witness = try_ground_better_witness<AggregationFunction>(best_cost,
                                                                        current_cost,
                                                                        rule,
                                                                        witness_condition,
                                                                        delta_context,
                                                                        iteration_context,
                                                                        program_and_annot,
                                                                        program_numeric_and_annot);
    if (!witness)
        return;  ///< No local or global improvement

    /// Update improved witness and cost annotation
    delta_and_annot.insert_or_assign(delta_head, Annotation(*witness));
}

template<typename AggregationFunction>
void AndAnnotationPolicy<AggregationFunction>::update_annotation(formalism::datalog::FunctionBindingView<formalism::FluentTag> program_head,
                                                                 formalism::datalog::FunctionBindingView<formalism::FluentTag> delta_head,
                                                                 uint_t current_cost,
                                                                 formalism::datalog::RuleView rule,
                                                                 formalism::datalog::ConjunctiveConditionView witness_condition,
                                                                 const AndAnnotationsMap& program_and_annot,
                                                                 const NumericAndAnnotationsMap& program_numeric_and_annot,
                                                                 NumericAndAnnotationsMap& delta_numeric_and_annot,
                                                                 formalism::datalog::GrounderContext& delta_context,
                                                                 formalism::datalog::GrounderContext& iteration_context) const
{
    const auto best_cost = std::numeric_limits<uint_t>::max();
    const auto cur_cost_lower_bound = current_cost + rule.get_cost();

    if (best_cost <= cur_cost_lower_bound)
        return;

    const auto witness = try_ground_better_witness<AggregationFunction>(best_cost,
                                                                        current_cost,
                                                                        rule,
                                                                        witness_condition,
                                                                        delta_context,
                                                                        iteration_context,
                                                                        program_and_annot,
                                                                        program_numeric_and_annot);
    if (!witness)
        return;

    delta_numeric_and_annot.insert_or_assign(delta_head, Annotation(*witness));
}

template class AndAnnotationPolicy<SumAggregation>;
template class AndAnnotationPolicy<MaxAggregation>;

}
