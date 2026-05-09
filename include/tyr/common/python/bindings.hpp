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

#ifndef TYR_COMMON_PYTHON_BINDINGS_HPP_
#define TYR_COMMON_PYTHON_BINDINGS_HPP_

#include "tyr/common/equal_to.hpp"
#include "tyr/common/formatter.hpp"
#include "tyr/common/hash.hpp"
#include "tyr/common/types.hpp"

#include <nanobind/nanobind.h>
#include <string>

namespace tyr
{
namespace nb = nanobind;

template<typename V>
nb::class_<V>& add_print(nb::class_<V>& cls)
{
    return cls  //
        .def("__str__", [](const V& self) { return to_string(self); })
        .def("__repr__", [](const V& self) { return to_string(self); });
}

template<typename V>
nb::class_<V>& add_hash(nb::class_<V>& cls)
{
    return cls  //
        .def("__eq__", [](const V& self, const V& other) { return EqualTo<V> {}(self, other); })
        .def("__hash__", [](const V& self) { return Hash<V> {}(self); });
}

template<typename T>
void bind_index(nb::module_& m, const std::string& name)
{
    nb::class_<T>(m, name.c_str())  //
        .def(nb::init<>())          // default -> MAX sentinel
        .def(nb::init<uint_t>(), nb::arg("index"))
        .def("__int__", [](const T& i) { return static_cast<uint_t>(i); })
        .def("__index__", [](const T& i) { return static_cast<uint_t>(i); })
        .def("__hash__", [](const T& i) { return static_cast<uint_t>(i); })

        .def("__eq__", [](const T& a, const T& b) { return a == b; })
        .def("__lt__", [](const T& a, const T& b) { return a < b; })
        .def("__le__", [](const T& a, const T& b) { return a <= b; })
        .def("__gt__", [](const T& a, const T& b) { return a > b; })
        .def("__ge__", [](const T& a, const T& b) { return a >= b; })

        .def("__repr__", [name](const T& i) { return name + "(" + std::to_string(static_cast<uint_t>(i)) + ")"; });
}

template<typename T>
void bind_fixed_uint(nb::module_& m, const std::string& name)
{
    using value_type = typename T::value_type;

    nb::class_<T>(m, name.c_str())
        .def(nb::init<>())  // default -> MAX sentinel
        .def(nb::init<value_type>(), nb::arg("value"))

        .def("__int__", [](const T& x) { return static_cast<value_type>(x); })
        .def("__index__", [](const T& x) { return static_cast<value_type>(x); })
        .def("__hash__", [](const T& x) { return static_cast<value_type>(x); })

        .def("value", &T::value)
        .def_static("max", &T::max)
        .def_static("MAX", []() { return T::MAX; })

        .def("__eq__", [](const T& a, const T& b) { return a == b; })
        .def("__lt__", [](const T& a, const T& b) { return a < b; })
        .def("__le__", [](const T& a, const T& b) { return a <= b; })
        .def("__gt__", [](const T& a, const T& b) { return a > b; })
        .def("__ge__", [](const T& a, const T& b) { return a >= b; })

        .def(
            "__add__",
            [](const T& a, value_type b) { return a + b; },
            nb::is_operator())
        .def(
            "__sub__",
            [](const T& a, value_type b) { return a - b; },
            nb::is_operator())
        .def(
            "__iadd__",
            [](T& a, value_type b)
            {
                a = a + b;
                return a;
            },
            nb::is_operator())
        .def(
            "__isub__",
            [](T& a, value_type b)
            {
                a = a - b;
                return a;
            },
            nb::is_operator())

        .def("inc",
             [](T& a)
             {
                 ++a;
                 return a;
             })

        .def("__repr__", [name](const T& x) { return name + "(" + std::to_string(static_cast<value_type>(x)) + ")"; });
}

}

#endif
