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

#include "bindings.hpp"

namespace f = tyr::formalism;
namespace fp = tyr::formalism::planning;

namespace tyr::planning
{

void bind_module_definitions(nb::module_& m)
{
    /**
     * Domain
     */

    nb::class_<Domain>(m, "Domain")  //
        .def("get_repository", &Domain::get_repository)
        .def("get_domain", &Domain::get_domain);

    /**
     * GroundTask
     */

    nb::class_<GroundTask>(m, "GroundTask")  //
        .def("get_repository", &GroundTask::get_repository)
        .def("get_task", &GroundTask::get_task);

    bind_state<GroundTask>(m, "GroundState");
    bind_node<GroundTask>(m, "GroundNode");
    bind_labeled_node<GroundTask>(m, "GroundLabeledNode");
    nb::bind_vector<std::vector<LabeledNode<GroundTask>>>(m, "GroundLabeledNodeList");
    bind_state_repository<GroundTask>(m, "GroundStateRepository");
    bind_successor_generator<GroundTask>(m, "GroundSuccessorGenerator");

    /**
     * LiftedTask
     */

    nb::class_<LiftedTask>(m, "LiftedTask")  //
        .def("get_repository", &LiftedTask::get_repository)
        .def("get_task", &LiftedTask::get_task)
        .def("instantiate_ground_task", &LiftedTask::instantiate_ground_task);

    bind_state<LiftedTask>(m, "LiftedState");
    bind_node<LiftedTask>(m, "LiftedNode");
    bind_labeled_node<LiftedTask>(m, "LiftedLabeledNode");
    nb::bind_vector<std::vector<LabeledNode<LiftedTask>>>(m, "LiftedLabeledNodeList");
    bind_state_repository<LiftedTask>(m, "LiftedStateRepository");
    bind_successor_generator<LiftedTask>(m, "LiftedSuccessorGenerator");

    /**
     * Parser
     */

    nb::class_<loki::ParserOptions>(m, "ParserOptions")
        .def(nb::init<>())
        .def_rw("strict", &loki::ParserOptions::strict, "Enable strict mode")
        .def_rw("verbose", &loki::ParserOptions::verbose, "Enable verbose output");

    nb::class_<Parser>(m, "Parser")
        .def(nb::init<const fs::path&, const loki::ParserOptions&>(), "domain_filepath"_a, "parser_options"_a)
        .def(nb::init<const std::string&, const fs::path&, const loki::ParserOptions&>(), "domain_description"_a, "domain_filepath"_a, "parser_options"_a)
        .def("parse_task", nb::overload_cast<const fs::path&, const loki::ParserOptions&>(&Parser::parse_task), "task_filepath"_a, "parser_options"_a)
        .def("parse_task",
             nb::overload_cast<const std::string&, const fs::path&, const loki::ParserOptions&>(&Parser::parse_task),
             "task_description"_a,
             "task_filepath"_a,
             "parser_options"_a)
        .def("get_domain", &Parser::get_domain);
}

}