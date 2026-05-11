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

#ifndef TYR_PYTHON_PLANNING_PLANNING_HPP_
#define TYR_PYTHON_PLANNING_PLANNING_HPP_

#include "module.hpp"

#include <nanobind/make_iterator.h>
#include <nanobind/stl/chrono.h>
#include <nanobind/stl/optional.h>
#include <nanobind/stl/pair.h>
#include <nanobind/stl/shared_ptr.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/vector.h>
#include <tyr/common/python/bindings.hpp>
#include <tyr/common/python/type_casters.hpp>
#include <tyr/tyr.hpp>

namespace tyr::planning
{

namespace fp = tyr::formalism::planning;

template<TaskKind Kind>
void bind_state(nb::module_& m, const std::string& name)
{
    using T = StateView<Kind>;

    auto cls = nb::class_<T>(m, name.c_str())  //
                   .def("__str__", [](const T& self) { return to_string(self); })
                   .def("get_index", &T::get_index, nb::rv_policy::copy)
                   .def("get_repository", &T::get_repository, nb::rv_policy::copy)
                   .def("get_state_repository", &T::get_state_repository, nb::rv_policy::copy)
                   // AccessibleStateConcept
                   .def("test",
                        nb::overload_cast<formalism::planning::GroundAtomView<formalism::StaticTag>>(&T::test, nb::const_),
                        nb::rv_policy::copy,
                        "static_atom"_a)
                   .def("test",
                        nb::overload_cast<formalism::planning::GroundAtomView<formalism::DerivedTag>>(&T::test, nb::const_),
                        nb::rv_policy::copy,
                        "derived_atom"_a)
                   .def("get",
                        nb::overload_cast<formalism::planning::GroundFunctionTermView<formalism::StaticTag>>(&T::get, nb::const_),
                        nb::rv_policy::copy,
                        "static_fterm"_a)
                   .def("get",
                        nb::overload_cast<formalism::planning::FDRVariableView<formalism::FluentTag>>(&T::get, nb::const_),
                        nb::rv_policy::copy,
                        "fluent_fact"_a)
                   .def("get",
                        nb::overload_cast<formalism::planning::GroundFunctionTermView<formalism::FluentTag>>(&T::get, nb::const_),
                        nb::rv_policy::copy,
                        "fluent_fterm"_a)
                   // IterableStateConcept
                   .def(
                       "static_atoms",
                       [](const T& s)
                       {
                           auto range = s.get_static_atoms_view();
                           return nb::make_iterator(nb::type<T>(), "static atom iterator", range);
                       },
                       nb::keep_alive<0, 1>())
                   .def(
                       "fluent_facts",
                       [](const T& s)
                       {
                           auto range = s.get_fluent_facts_view();
                           return nb::make_iterator(nb::type<T>(), "fluent facts iterator", range);
                       },
                       nb::keep_alive<0, 1>())
                   .def(
                       "derived_atoms",
                       [](const T& s)
                       {
                           auto range = s.get_derived_atoms_view();
                           return nb::make_iterator(nb::type<T>(), "derived atom iterator", range);
                       },
                       nb::keep_alive<0, 1>())
                   .def(
                       "static_fterm_values",
                       [](const T& s)
                       {
                           auto range = s.get_static_fterm_values_view();
                           return nb::make_iterator(nb::type<T>(), "static function term value iterator", range);
                       },
                       nb::keep_alive<0, 1>())
                   .def(
                       "fluent_fterm_values",
                       [](const T& s)
                       {
                           auto range = s.get_fluent_fterm_values_view();
                           return nb::make_iterator(nb::type<T>(), "fluent function term value iterator", range);
                       },
                       nb::keep_alive<0, 1>());
    add_hash(cls);
}

template<TaskKind Kind>
void bind_node(nb::module_& m, const std::string& name)
{
    using T = Node<Kind>;

    nb::class_<T>(m, name.c_str())
        .def(nb::init<StateView<Kind>, float_t>(), "state"_a, "metric_value"_a)
        .def("__str__", [](const T& self) { return to_string(self); })
        .def("get_state", &T::get_state, nb::rv_policy::reference_internal)
        .def("get_metric", &T::get_metric, nb::rv_policy::copy);
}

template<TaskKind Kind>
void bind_labeled_node(nb::module_& m, const std::string& name)
{
    using T = LabeledNode<Kind>;

    nb::class_<T>(m, name.c_str())  //
        .def_ro("label", &T::label, nb::rv_policy::copy)
        .def_ro("node", &T::node, nb::rv_policy::copy);
}

template<TaskKind Kind>
void bind_plan(nb::module_& m, const std::string& name)
{
    using T = Plan<Kind>;

    nb::class_<T>(m, name.c_str())  //
        .def(nb::init<Node<Kind>>(), "start_node"_a)
        .def(nb::init<Node<Kind>, LabeledNodeList<Kind>>(), "start_node"_a, "labeled_succ_nodes"_a)
        .def("__str__", [](const T& self) { return to_string(self); })
        .def("get_start_node", &T::get_start_node, nb::rv_policy::copy)
        .def("get_labeled_succ_nodes", &T::get_labeled_succ_nodes, nb::rv_policy::copy)
        .def("get_cost", &T::get_cost)
        .def("get_length", &T::get_length);
}

template<TaskKind Kind>
void bind_axiom_evaluator(nb::module_& m, const std::string& name)
{
    using T = AxiomEvaluator<Kind>;

    nb::class_<T>(m, name.c_str())  //
        .def("get_index", &T::get_index);
}

template<TaskKind Kind>
void bind_state_repository(nb::module_& m, const std::string& name)
{
    using T = StateRepository<Kind>;

    nb::class_<T>(m, name.c_str())  //
        .def("get_index", &T::get_index)
        .def("get_initial_state", &T::get_initial_state, nb::rv_policy::move)
        .def("get_registered_state", &T::get_registered_state, nb::rv_policy::move, "state_index"_a)
        .def("create_state",
             nb::overload_cast<const std::vector<formalism::planning::FDRFactView<formalism::FluentTag>>&,
                               const std::vector<formalism::planning::GroundFunctionTermViewValuePair<formalism::FluentTag>>&>(&T::create_state),
             nb::rv_policy::move,
             "fluent_fact"_a,
             "fterm_values"_a)
        .def("get_axiom_evaluator", &T::get_axiom_evaluator, nb::rv_policy::copy);
}

template<TaskKind Kind>
void bind_successor_generator(nb::module_& m, const std::string& name)
{
    using T = SuccessorGenerator<Kind>;

    nb::class_<T>(m, name.c_str())
        .def("get_index", &T::get_index)
        .def("get_initial_node", &T::get_initial_node, nb::rv_policy::move)
        .def("get_labeled_successor_nodes",
             nb::overload_cast<const Node<Kind>&>(&T::get_labeled_successor_nodes),
             nb::rv_policy::move,
             "node"_a,
             nb::call_guard<nb::gil_scoped_release>())
        .def("get_successor_node", nb::overload_cast<const Node<Kind>&, fp::GroundActionView>(&T::get_successor_node), "node"_a, "action"_a)
        .def("get_node", &T::get_node, nb::rv_policy::move, "state_index"_a)
        .def("get_state_repository", &T::get_state_repository, nb::rv_policy::copy);
}

template<TaskKind Kind>
void bind_axiom_evaluator_factory(nb::module_& m, const std::string& name)
{
    using T = AxiomEvaluatorFactory<Kind>;

    nb::class_<T>(m, name.c_str())  //
        .def(nb::init<>())
        .def("create", &T::create, "task"_a, "execution_context"_a);
}

template<TaskKind Kind>
void bind_state_repository_factory(nb::module_& m, const std::string& name)
{
    using T = StateRepositoryFactory<Kind>;

    nb::class_<T>(m, name.c_str())  //
        .def(nb::init<>())
        .def("create", &T::create, "task"_a, "axiom_evaluator"_a);
}

template<TaskKind Kind>
void bind_successor_generator_factory(nb::module_& m, const std::string& name)
{
    using T = SuccessorGeneratorFactory<Kind>;

    nb::class_<T>(m, name.c_str())  //
        .def(nb::init<>())
        .def("create", &T::create, "task"_a, "execution_context"_a, "state_repository"_a);
}

template<TaskKind Kind>
void bind_search_result(nb::module_& m, const std::string& name)
{
    using T = SearchResult<Kind>;

    nb::class_<T>(m, name.c_str()).def(nb::init<>()).def_rw("status", &T::status).def_rw("plan", &T::plan).def_rw("goal_node", &T::goal_node);
}

template<TaskKind Kind>
class PyGoalStrategy : public GoalStrategy<Kind>
{
public:
    using Base = GoalStrategy<Kind>;

    NB_TRAMPOLINE(Base, 2);

    bool is_static_goal_satisfied(const Task<Kind>& task) override { NB_OVERRIDE_PURE(is_static_goal_satisfied, task); }

    bool is_dynamic_goal_satisfied(const StateView<Kind>& state) override { NB_OVERRIDE_PURE(is_dynamic_goal_satisfied, state); }
};

template<TaskKind Kind>
void bind_goal_strategy(nb::module_& m, const std::string& name)
{
    using T = GoalStrategy<Kind>;

    nb::class_<T, PyGoalStrategy<Kind>>(m, name.c_str())  //
        .def("is_static_goal_satisfied", &T::is_static_goal_satisfied, "task"_a)
        .def("is_dynamic_goal_satisfied", &T::is_dynamic_goal_satisfied, "state"_a);
}

template<TaskKind Kind>
void bind_conjunctive_goal_strategy(nb::module_& m, const std::string& name)
{
    using T = ConjunctiveGoalStrategy<Kind>;

    nb::class_<T, GoalStrategy<Kind>>(m, name.c_str())  //
        .def(nb::init<const Task<Kind>&>(), "task"_a)
        .def(nb::init<fp::GroundConjunctiveConditionView>(), "goal"_a)
        .def("set_goal", &T::set_goal, "goal"_a);
}

template<TaskKind Kind>
void bind_exhaustive_goal_strategy(nb::module_& m, const std::string& name)
{
    using T = ExhaustiveGoalStrategy<Kind>;

    nb::class_<T, GoalStrategy<Kind>>(m, name.c_str())  //
        .def(nb::init<>());
}

template<TaskKind Kind>
class PyPruningStrategy : public PruningStrategy<Kind>
{
public:
    using Base = PruningStrategy<Kind>;

    NB_TRAMPOLINE(Base, 2);

    bool should_prune_state(const StateView<Kind>& state) override { NB_OVERRIDE(should_prune_state, state); }

    bool should_prune_successor_state(const StateView<Kind>& state, const StateView<Kind>& succ_state, bool is_new_succ_state) override
    {
        NB_OVERRIDE(should_prune_successor_state, state, succ_state, is_new_succ_state);
    }
};

template<TaskKind Kind>
void bind_pruning_strategy(nb::module_& m, const std::string& name)
{
    using T = PruningStrategy<Kind>;

    nb::class_<T, PyPruningStrategy<Kind>>(m, name.c_str())  //
        .def(nb::init<>())
        .def("should_prune_state", nb::overload_cast<const StateView<Kind>&>(&T::should_prune_state), "state"_a)
        .def("should_prune_successor_state",
             nb::overload_cast<const StateView<Kind>&, const StateView<Kind>&, bool>(&T::should_prune_successor_state),
             "state"_a,
             "succ_state"_a,
             "is_new_succ_state"_a);
}

template<TaskKind Kind>
class PyHeuristic : public Heuristic<Kind>
{
public:
    using Base = Heuristic<Kind>;

    NB_TRAMPOLINE(Base, 3);

    void set_goal(formalism::planning::GroundConjunctiveConditionView goal) override { NB_OVERRIDE_PURE(set_goal, goal); }

    float_t evaluate(const StateView<Kind>& state) override { NB_OVERRIDE_PURE(evaluate, state); }

    const UnorderedSet<formalism::planning::GroundActionView>& get_preferred_action_views() override { NB_OVERRIDE(get_preferred_action_views); }
};

template<TaskKind Kind>
void bind_heuristic(nb::module_& m, const std::string& name)
{
    using T = Heuristic<Kind>;

    nb::class_<T, PyHeuristic<Kind>>(m, name.c_str())  //
        .def("set_goal", &T::set_goal, "goal"_a)
        .def("evaluate", &T::evaluate, "state"_a, nb::call_guard<nb::gil_scoped_release>())
        .def("get_preferred_actions", &T::get_preferred_action_views);
}

template<TaskKind Kind>
void bind_blind_heuristic(nb::module_& m, const std::string& name)
{
    using T = BlindHeuristic<Kind>;

    nb::class_<T, Heuristic<Kind>>(m, name.c_str())  //
        .def(nb::new_([]() { return T::create(); }));
}

template<TaskKind Kind>
void bind_goal_count_heuristic(nb::module_& m, const std::string& name)
{
    using T = GoalCountHeuristic<Kind>;

    nb::class_<T, Heuristic<Kind>>(m, name.c_str())  //
        .def(nb::new_([](std::shared_ptr<const Task<Kind>> task) { return T::create(std::move(task)); }));
}

template<TaskKind Kind>
void bind_rpg_max_heuristic(nb::module_& m, const std::string& name)
{
    using T = MaxRPGHeuristic<Kind>;

    nb::class_<T, Heuristic<Kind>>(m, name.c_str())  //
        .def(nb::new_([](TaskPtr<Kind> task, std::shared_ptr<ExecutionContext> execution_context)
                      { return T::create(std::move(task), std::move(execution_context)); }),
             "task"_a,
             "execution_context"_a);
}

template<TaskKind Kind>
void bind_rpg_add_heuristic(nb::module_& m, const std::string& name)
{
    using T = AddRPGHeuristic<Kind>;

    nb::class_<T, Heuristic<Kind>>(m, name.c_str())  //
        .def(nb::new_([](TaskPtr<Kind> task, std::shared_ptr<ExecutionContext> execution_context)
                      { return T::create(std::move(task), std::move(execution_context)); }),
             "task"_a,
             "execution_context"_a);
}

template<TaskKind Kind>
void bind_rpg_ff_heuristic(nb::module_& m, const std::string& name)
{
    using T = FFRPGHeuristic<Kind>;

    nb::class_<T, Heuristic<Kind>>(m, name.c_str())  //
        .def(nb::new_([](TaskPtr<Kind> task, std::shared_ptr<ExecutionContext> execution_context)
                      { return T::create(std::move(task), std::move(execution_context)); }),
             "task"_a,
             "execution_context"_a);
}

}  // namespace tyr::planning

#endif
