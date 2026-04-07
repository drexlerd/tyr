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

#include "tyr/planning/lifted_task.hpp"

#include "tyr/common/comparators.hpp"     // for operat...
#include "tyr/common/dynamic_bitset.hpp"  // for set
#include "tyr/common/vector.hpp"          // for View, set
#include "tyr/formalism/planning/formatter.hpp"
#include "tyr/formalism/planning/views.hpp"  // for View
#include "tyr/planning/lifted_task/task_grounder.hpp"
#include "tyr/planning/task_utils.hpp"

#include <tuple>
#include <utility>

namespace f = tyr::formalism;
namespace fp = tyr::formalism::planning;

namespace tyr::planning
{
Task<LiftedTag>::Task(formalism::planning::PlanningTask task) :
    m_task(std::move(task)),
    m_static_atoms_bitset(),
    m_static_numeric_variables(),
    m_axiom_program(get_task()),
    m_action_program(get_task()),
    m_rpg_program(get_task())
{
    for (const auto atom : get_task().template get_atoms<f::StaticTag>())
        set(uint_t(atom.get_index()), true, m_static_atoms_bitset);

    for (const auto fterm_value : get_task().template get_fterm_values<f::StaticTag>())
        set(uint_t(fterm_value.get_fterm().get_index()), fterm_value.get_value(), m_static_numeric_variables, std::numeric_limits<float_t>::quiet_NaN());
}

std::shared_ptr<LiftedTask> LiftedTask::create(formalism::planning::PlanningTask task) { return std::make_shared<LiftedTask>(std::move(task)); }

GroundTaskInstantiationResult LiftedTask::instantiate_ground_task(ExecutionContext& execution_context, const GroundTaskInstantiationOptions& options)
{
    return tyr::planning::instantiate_ground_task(*this, execution_context, options);
}

}
