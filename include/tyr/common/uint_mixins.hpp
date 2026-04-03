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

#ifndef TYR_COMMON_UINT_MIXINS_HPP_
#define TYR_COMMON_UINT_MIXINS_HPP_

#include "tyr/common/config.hpp"

#include <concepts>
#include <limits>

namespace tyr
{

template<typename Derived>
struct FixedUintMixin
{
    using value_type = uint_t;

    value_type m_value;

    static constexpr value_type MAX = std::numeric_limits<value_type>::max();

    constexpr FixedUintMixin() noexcept : m_value(MAX) {}
    explicit constexpr FixedUintMixin(value_type v) noexcept : m_value(v) {}

    static constexpr Derived max() noexcept { return Derived(MAX); }

    // ----------------------------------------------------
    // Comparisons
    // ----------------------------------------------------

    friend constexpr bool operator==(const FixedUintMixin& lhs, const FixedUintMixin& rhs) noexcept { return lhs.m_value == rhs.m_value; }
    friend constexpr bool operator!=(const FixedUintMixin& lhs, const FixedUintMixin& rhs) noexcept { return !(lhs == rhs); }
    friend constexpr bool operator<=(const FixedUintMixin& lhs, const FixedUintMixin& rhs) noexcept { return lhs.m_value <= rhs.m_value; }
    friend constexpr bool operator<(const FixedUintMixin& lhs, const FixedUintMixin& rhs) noexcept { return lhs.m_value < rhs.m_value; }
    friend constexpr bool operator>=(const FixedUintMixin& lhs, const FixedUintMixin& rhs) noexcept { return lhs.m_value >= rhs.m_value; }
    friend constexpr bool operator>(const FixedUintMixin& lhs, const FixedUintMixin& rhs) noexcept { return lhs.m_value > rhs.m_value; }

    // ----------------------------------------------------
    // Arithmetic with integral types
    // ----------------------------------------------------

    template<std::integral T>
    friend constexpr Derived operator+(const FixedUintMixin& lhs, T rhs) noexcept
    {
        return Derived(lhs.m_value + rhs);
    }
    template<std::integral T>
    friend constexpr Derived operator-(const FixedUintMixin& lhs, T rhs) noexcept
    {
        return Derived(lhs.m_value - rhs);  // wraps if rhs > lhs
    }

    // ----------------------------------------------------
    // Pre-increment (++x)
    // ----------------------------------------------------
    constexpr Derived& operator++() noexcept
    {
        ++m_value;
        return static_cast<Derived&>(*this);
    }

    // ----------------------------------------------------
    // Post-increment (x++)
    // ----------------------------------------------------
    constexpr Derived operator++(int) noexcept
    {
        Derived tmp = static_cast<const Derived&>(*this);
        ++(*this);
        return tmp;
    }

    constexpr value_type value() const noexcept { return m_value; }

    explicit constexpr operator value_type() const noexcept { return m_value; }

    auto cista_members() const noexcept { return std::tie(m_value); }
    auto identifying_members() const noexcept { return std::tie(m_value); }
};

}

#endif
