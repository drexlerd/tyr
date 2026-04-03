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

#include "tyr/planning/task_utils.hpp"

#include "tyr/analysis/domains.hpp"
#include "tyr/common/config.hpp"
#include "tyr/datalog/assignment_sets.hpp"
#include "tyr/datalog/fact_sets.hpp"
#include "tyr/datalog/workspaces/program.hpp"
#include "tyr/formalism/datalog/merge.hpp"
#include "tyr/formalism/datalog/repository.hpp"
#include "tyr/formalism/datalog/views.hpp"
#include "tyr/formalism/planning/merge_datalog.hpp"
#include "tyr/formalism/planning/repository.hpp"
#include "tyr/formalism/planning/views.hpp"
#include "tyr/planning/lifted_task/unpacked_state.hpp"

namespace f = tyr::formalism;
namespace fp = tyr::formalism::planning;

namespace tyr::planning
{

void insert_fluent_atoms_to_fact_set(const UnpackedState<LiftedTag>& state,
                                     const formalism::planning::Repository& repository,
                                     fp::MergeDatalogContext& merge_context,
                                     datalog::TaggedFactSets<f::FluentTag>& fact_sets)
{
    for (const auto fact : state.get_fluent_facts_view(repository))
        fact_sets.predicate.insert(fp::merge_p2d<f::FluentTag, f::FluentTag>(fact.get_atom().value(), merge_context).first);
}

void insert_derived_atoms_to_fact_set(const UnpackedState<LiftedTag>& state,
                                      const formalism::planning::Repository& repository,
                                      fp::MergeDatalogContext& merge_context,
                                      datalog::TaggedFactSets<f::FluentTag>& fact_sets)
{
    for (const auto atom : state.get_derived_atoms_view(repository))
        fact_sets.predicate.insert(fp::merge_p2d<f::DerivedTag, f::FluentTag>(atom, merge_context).first);
}

void insert_numeric_variables_to_fact_set(const UnpackedState<LiftedTag>& state,
                                          const formalism::planning::Repository& repository,
                                          fp::MergeDatalogContext& merge_context,
                                          datalog::TaggedFactSets<f::FluentTag>& fact_sets)
{
    for (const auto& [fterm, value] : state.get_fluent_fterm_values_view(repository))
        fact_sets.function.insert(fp::merge_p2d(fterm, merge_context).first, value);
}

void insert_extended_state(const UnpackedState<LiftedTag>& unpacked_state,
                           const fp::Repository& atoms_context,
                           fp::MergeDatalogContext& merge_context,
                           datalog::TaggedFactSets<f::FluentTag>& fact_sets,
                           datalog::TaggedAssignmentSets<f::FluentTag>& assignment_sets)
{
    fact_sets.reset();
    assignment_sets.reset();

    insert_fluent_atoms_to_fact_set(unpacked_state, atoms_context, merge_context, fact_sets);
    insert_derived_atoms_to_fact_set(unpacked_state, atoms_context, merge_context, fact_sets);
    insert_numeric_variables_to_fact_set(unpacked_state, atoms_context, merge_context, fact_sets);

    assignment_sets.insert(fact_sets);
}

void insert_unextended_state(const UnpackedState<LiftedTag>& unpacked_state,
                             const fp::Repository& atoms_context,
                             fp::MergeDatalogContext& merge_context,
                             datalog::TaggedFactSets<f::FluentTag>& fact_sets,
                             datalog::TaggedAssignmentSets<f::FluentTag>& assignment_sets)
{
    fact_sets.reset();
    assignment_sets.reset();

    insert_fluent_atoms_to_fact_set(unpacked_state, atoms_context, merge_context, fact_sets);
    insert_numeric_variables_to_fact_set(unpacked_state, atoms_context, merge_context, fact_sets);

    assignment_sets.insert(fact_sets);
}

}
