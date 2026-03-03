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

#ifndef TYR_PYTHON_PLANNING_BINDINGS_HPP
#define TYR_PYTHON_PLANNING_BINDINGS_HPP

#include "../init_declarations.hpp"

NB_MAKE_OPAQUE(std::vector<tyr::planning::LabeledNode<tyr::planning::GroundTask>>);
NB_MAKE_OPAQUE(std::vector<tyr::planning::LabeledNode<tyr::planning::LiftedTask>>);

namespace tyr::planning
{
template<typename Task>
void bind_state(nb::module_& m, const std::string& name)
{
    nb::class_<State<Task>>(m, name.c_str())  //
        .def("__str__", [](const State<Task>& self) { return to_string(self); })
        .def("get_index", &State<Task>::get_index, nb::rv_policy::copy)
        .def("get_task", &State<Task>::get_task, nb::rv_policy::reference_internal)
        // AccessibleStateConcept
        .def("test",
             nb::overload_cast<Index<formalism::planning::GroundAtom<formalism::StaticTag>>>(&State<Task>::test, nb::const_),
             nb::rv_policy::copy,
             "index"_a)
        .def("test",
             nb::overload_cast<Index<formalism::planning::GroundAtom<formalism::DerivedTag>>>(&State<Task>::test, nb::const_),
             nb::rv_policy::copy,
             "index"_a)
        .def("get",
             nb::overload_cast<Index<formalism::planning::GroundFunctionTerm<formalism::StaticTag>>>(&State<Task>::get, nb::const_),
             nb::rv_policy::copy,
             "index"_a)
        .def("get",
             nb::overload_cast<Index<formalism::planning::FDRVariable<formalism::FluentTag>>>(&State<Task>::get, nb::const_),
             nb::rv_policy::copy,
             "index"_a)
        .def("get",
             nb::overload_cast<Index<formalism::planning::GroundFunctionTerm<formalism::FluentTag>>>(&State<Task>::get, nb::const_),
             nb::rv_policy::copy,
             "index"_a)
        // IterableStateConcept
        .def(
            "static_atoms",
            [](const State<Task>& s)
            {
                auto r = s.get_static_atoms();
                return nb::make_iterator(nb::type<AtomRange<formalism::StaticTag>>(), "static atom iterator", std::begin(r), std::end(r));
            },
            nb::keep_alive<0, 1>())
        .def(
            "fluent_facts",
            [](const State<Task>& s)
            {
                auto r = s.get_fluent_facts();
                return nb::make_iterator(nb::type<FDRFactRange<Task, formalism::FluentTag>>(), "fluent fact iterator", std::begin(r), std::end(r));
            },
            nb::keep_alive<0, 1>())
        .def(
            "derived_atoms",
            [](const State<Task>& s)
            {
                auto r = s.get_derived_atoms();
                return nb::make_iterator(nb::type<AtomRange<formalism::DerivedTag>>(), "derived atom iterator", std::begin(r), std::end(r));
            },
            nb::keep_alive<0, 1>())
        .def(
            "static_fterm_values",
            [](const State<Task>& s)
            {
                auto r = s.get_static_fterm_values();
                return nb::make_iterator(nb::type<FunctionTermValueRange<formalism::StaticTag>>(),
                                         "static function term value iterator",
                                         std::begin(r),
                                         std::end(r));
            },
            nb::keep_alive<0, 1>())
        .def(
            "fluent_fterm_values",
            [](const State<Task>& s)
            {
                auto r = s.get_fluent_fterm_values();
                return nb::make_iterator(nb::type<FunctionTermValueRange<formalism::FluentTag>>(),
                                         "fluent function term value iterator",
                                         std::begin(r),
                                         std::end(r));
            },
            nb::keep_alive<0, 1>());
}

template<typename Task>
void bind_node(nb::module_& m, const std::string& name)
{
    nb::class_<Node<Task>>(m, name.c_str())
        .def("__str__", [](const Node<Task>& self) { return to_string(self); })
        .def("get_state", &Node<Task>::get_state, nb::rv_policy::reference_internal)
        .def("get_metric", &Node<Task>::get_metric, nb::rv_policy::copy)
        .def("get_task", &Node<Task>::get_task, nb::rv_policy::reference_internal);
}

template<typename Task>
void bind_labeled_node(nb::module_& m, const std::string& name)
{
    nb::class_<LabeledNode<Task>>(m, name.c_str())  //
        .def_ro("label", &LabeledNode<Task>::label, nb::rv_policy::copy, nb::keep_alive<0, 1>())
        .def_ro("node", &LabeledNode<Task>::node, nb::rv_policy::copy, nb::keep_alive<0, 1>());
}

template<typename Task>
void bind_state_repository(nb::module_& m, const std::string& name)
{
    nb::class_<StateRepository<Task>>(m, name.c_str())  //
        .def(nb::init<std::shared_ptr<Task>>(), "task"_a)
        .def("get_initial_state", &StateRepository<Task>::get_initial_state, nb::rv_policy::move, nb::keep_alive<0, 1>())
        .def("get_registered_state", &StateRepository<Task>::get_registered_state, nb::rv_policy::move, nb::keep_alive<0, 1>(), "state_index"_a)
        .def("get_axiom_evaluator", &StateRepository<Task>::get_axiom_evaluator, nb::rv_policy::reference_internal);
}

template<typename Task>
void bind_successor_generator(nb::module_& m, const std::string& name)
{
    nb::class_<SuccessorGenerator<Task>>(m, name.c_str())
        .def(nb::new_([](std::shared_ptr<Task> task) { return SuccessorGenerator<Task>::create(std::move(task)); }))  // class is not copieable
        .def("get_initial_node", &SuccessorGenerator<Task>::get_initial_node, nb::keep_alive<0, 1>())
        .def("get_labeled_successor_nodes",
             nb::overload_cast<const Node<Task>&>(&SuccessorGenerator<Task>::get_labeled_successor_nodes),
             nb::rv_policy::move,
             nb::keep_alive<0, 1>(),
             "node"_a)
        .def("get_state", &SuccessorGenerator<Task>::get_state, nb::rv_policy::move, nb::keep_alive<0, 1>())
        .def("get_state_repository", &SuccessorGenerator<Task>::get_state_repository, nb::rv_policy::reference_internal);
}

}

#endif