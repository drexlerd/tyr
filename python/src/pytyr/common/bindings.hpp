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

#ifndef TYR_PYTHON_COMMON_BINDINGS_HPP
#define TYR_PYTHON_COMMON_BINDINGS_HPP

#include "../init_declarations.hpp"

namespace tyr
{

template<typename T>
void bind_index(nb::module_& m, const std::string& name)
{
    nb::class_<T>(m, name.c_str())  //
        .def("__int__", [](const T& i) { return (uint_t) i; })
        .def("__index__", [](const T& i) { return (uint_t) i; })
        .def("__hash__", [](const T& i) { return (uint_t) i; })

        .def("__eq__", [](const T& a, const T& b) { return a == b; })
        .def("__lt__", [](const T& a, const T& b) { return a < b; })
        .def("__le__", [](const T& a, const T& b) { return a <= b; })
        .def("__gt__", [](const T& a, const T& b) { return a > b; })
        .def("__ge__", [](const T& a, const T& b) { return a >= b; })

        .def("__repr__", [name](const T& i) { return name + "(" + std::to_string((uint_t) i) + ")"; });
}

}

#endif