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

#ifndef TYR_COMMON_CONFIG_HPP_
#define TYR_COMMON_CONFIG_HPP_

#include <cassert>
#include <cista/mode.h>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <ranges>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>

namespace tyr
{
using int_t = std::int32_t;
using uint_t = std::uint32_t;
using float_t = double;

template<typename T>
struct FloatTolerance
{
    static_assert(!std::is_same_v<T, T>, "FloatTolerance<T> is not defined for this type.");
};

template<>
struct FloatTolerance<float_t>
{
    static constexpr float_t abs_epsilon = static_cast<float_t>(1e-12);
    static constexpr float_t rel_epsilon = static_cast<float_t>(1e-12);
};

#ifdef NDEBUG
static constexpr ::cista::mode CISTA_MODE = ::cista::mode::UNCHECKED;
#else
static constexpr ::cista::mode CISTA_MODE = ::cista::mode::NONE;
#endif
}

#endif
