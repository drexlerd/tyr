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

#ifndef TYR_COMMON_IOSTREAM_HPP_
#define TYR_COMMON_IOSTREAM_HPP_

#include <iosfwd>
#include <iostream>

namespace tyr
{

inline int indent_index()
{
    static int idx = std::ios_base::xalloc();
    return idx;
}

constexpr int INDENT_WIDTH = 4;

inline std::ostream& print_indent(std::ostream& os)
{
    auto level = os.iword(indent_index());
    if (level < 0)
        level = 0;
    for (long i = 0; i < level * INDENT_WIDTH; ++i)
        os.put(' ');
    return os;
}

// manipulators to change indentation
inline std::ostream& indent_up(std::ostream& os)
{
    ++os.iword(indent_index());
    return os;
}

inline std::ostream& indent_down(std::ostream& os)
{
    --os.iword(indent_index());
    return os;
}

// RAII scope for indentation (recommended)
struct IndentScope
{
    std::ostream& os;
    explicit IndentScope(std::ostream& os) : os(os) { indent_up(os); }
    ~IndentScope() { indent_down(os); }
};
}

#endif