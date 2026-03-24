/*
 * Copyright (C) 2025 Dominik Drexler
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

#include "tyr/planning/lifted_task/heuristics/add.hpp"

namespace tyr::planning
{
AddHeuristic<LiftedTask>::AddHeuristic(std::shared_ptr<LiftedTask> task, ExecutionContextPtr execution_context) :
    RPGBase<AddHeuristic<LiftedTask>,
            datalog::OrAnnotationPolicy,
            datalog::AndAnnotationPolicy<datalog::SumAggregation>,
            datalog::TerminationPolicy<datalog::SumAggregation>>(
        task,
        std::move(execution_context),
        datalog::OrAnnotationPolicy(),
        datalog::AndAnnotationPolicy<datalog::SumAggregation>(),
        datalog::TerminationPolicy<datalog::SumAggregation>(
            task->get_rpg_program().get_program_context().get_program().get_predicates<formalism::FluentTag>().size()))
{
}

std::shared_ptr<AddHeuristic<LiftedTask>> AddHeuristic<LiftedTask>::create(std::shared_ptr<LiftedTask> task, ExecutionContextPtr execution_context)
{
    return std::make_shared<AddHeuristic<LiftedTask>>(std::move(task), std::move(execution_context));
}

float_t AddHeuristic<LiftedTask>::extract_cost_and_set_preferred_actions_impl(const StateView<LiftedTask>& state)
{
    return m_workspace.tp.get_total_cost(this->m_workspace.or_annot);
}

}
