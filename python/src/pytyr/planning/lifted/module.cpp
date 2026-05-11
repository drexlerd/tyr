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

#include "module.hpp"

#include "../planning.hpp"
#include "astar_eager/module.hpp"
#include "gbfs_lazy/module.hpp"

#include <nanobind/stl/shared_ptr.h>
#include <tyr/tyr.hpp>

namespace tyr::planning
{

void bind_lifted_module_definitions(nb::module_& m)
{
    auto astar_eager_module = m.def_submodule("astar_eager");
    astar_eager::bind_lifted_module_definitions(astar_eager_module);

    auto gbfs_lazy_module = m.def_submodule("gbfs_lazy");
    gbfs_lazy::bind_lifted_module_definitions(gbfs_lazy_module);

    nb::enum_<GroundTaskInstantiationStatus>(m, "GroundTaskInstantiationStatus")
        .value("SUCCESS", GroundTaskInstantiationStatus::SUCCESS)
        .value("PROVEN_UNSOLVABLE", GroundTaskInstantiationStatus::PROVEN_UNSOLVABLE);

    nb::class_<GroundTaskInstantiationResult>(m, "GroundTaskInstantiationResult")
        .def(nb::init<>())
        .def(nb::init<TaskPtr<GroundTag>, GroundTaskInstantiationStatus>(), "task"_a, "status"_a)
        .def_rw("task", &GroundTaskInstantiationResult::task)
        .def_rw("status", &GroundTaskInstantiationResult::status);

    nb::class_<GroundTaskInstantiationOptions>(m, "GroundTaskInstantiationOptions")
        .def(nb::init<>())
        .def(nb::init<bool>(), "disable_invariant_synthesis"_a = true)
        .def_rw("disable_invariant_synthesis", &GroundTaskInstantiationOptions::disable_invariant_synthesis);

    nb::class_<Task<LiftedTag>>(m, "Task")  //
        .def(nb::new_([](formalism::planning::PlanningTask&& task) { return Task<LiftedTag>::create(std::move(task)); }),
             "formalism_task"_a,
             R"doc(
Create a planning task from a formalism task.

Parameters
----------
formalism_task : formalism.planning.Task
    The formalism-level task used to construct the planning task.

Notes
-----
The `formalism_task` is **moved** into the planning task. After calling this
constructor, the original formalism task should be considered consumed and
should not be used further.
        )doc")
        .def("get_formalism_task", &Task<LiftedTag>::get_formalism_task)
        .def("get_repository", &Task<LiftedTag>::get_repository)
        .def("get_task", &Task<LiftedTag>::get_task)
        .def("get_fdr_context", nb::overload_cast<>(&Task<LiftedTag>::get_fdr_context, nb::const_))
        .def("instantiate_ground_task", &Task<LiftedTag>::instantiate_ground_task, "execution_context"_a, "options"_a);

    bind_index<Index<State<LiftedTag>>>(m, "StateIndex");
    bind_state<LiftedTag>(m, "State");
    bind_node<LiftedTag>(m, "Node");
    bind_labeled_node<LiftedTag>(m, "LabeledNode");
    bind_plan<LiftedTag>(m, "Plan");
    bind_axiom_evaluator<LiftedTag>(m, "AxiomEvaluator");
    bind_axiom_evaluator_factory<LiftedTag>(m, "AxiomEvaluatorFactory");
    bind_state_repository<LiftedTag>(m, "StateRepository");
    bind_state_repository_factory<LiftedTag>(m, "StateRepositoryFactory");
    bind_successor_generator<LiftedTag>(m, "SuccessorGenerator");
    bind_successor_generator_factory<LiftedTag>(m, "SuccessorGeneratorFactory");
    bind_search_result<LiftedTag>(m, "SearchResult");
    bind_goal_strategy<LiftedTag>(m, "GoalStrategy");
    bind_conjunctive_goal_strategy<LiftedTag>(m, "ConjunctiveGoalStrategy");
    bind_exhaustive_goal_strategy<LiftedTag>(m, "ExhaustiveGoalStrategy");
    bind_pruning_strategy<LiftedTag>(m, "PruningStrategy");
    bind_heuristic<LiftedTag>(m, "Heuristic");
    bind_blind_heuristic<LiftedTag>(m, "BlindHeuristic");
    bind_rpg_max_heuristic<LiftedTag>(m, "MaxRPGHeuristic");
    bind_rpg_add_heuristic<LiftedTag>(m, "AddRPGHeuristic");
    bind_rpg_ff_heuristic<LiftedTag>(m, "FFRPGHeuristic");
    bind_goal_count_heuristic<LiftedTag>(m, "GoalCountHeuristic");
}

}  // namespace tyr::planning
