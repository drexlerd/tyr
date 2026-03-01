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

#include "../init_declarations.hpp"

namespace tyr::planning
{

void bind_module_definitions(nb::module_& m)
{
    /**
     * Domain
     */

    nb::class_<Domain>(m, "Domain");

    /**
     * GroundTask
     */

    nb::class_<GroundTask>(m, "GroundTask");

    /**
     * LiftedTask
     */

    nb::class_<LiftedTask>(m, "LiftedTask");

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