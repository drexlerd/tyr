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

#ifndef TYR_COMMON_TYPES_HPP_
#define TYR_COMMON_TYPES_HPP_

#include "tyr/common/declarations.hpp"

#include <cista/containers/optional.h>
#include <cista/containers/variant.h>
#include <cista/containers/vector.h>

namespace tyr
{

template<typename T>
struct Data;

template<typename T>
using DataList = ::cista::offset::vector<Data<T>>;

template<typename T>
struct Index;

template<typename T>
using IndexList = ::cista::offset::vector<Index<T>>;

template<typename T, typename C>
struct View
{
};

template<typename T, typename C>
concept ViewConcept = requires(T type, const C& context) {
    // Constructor
    View<T, C>(type, context);
    // Method to retrieve the underlying Data.
    { View<T, C>(type, context).get_data() };
    // Method to retrieve the underlying context.
    { View<T, C>(type, context).get_context() };
    // Method to retrieve the underlying lightweight handle or data.
    { View<T, C>(type, context).get_handle() } -> std::same_as<const T&>;
};

/// @brief Helper to create a view
template<typename T, typename C>
auto make_view(const T& element, const C& context) noexcept
{
    return View<T, C>(element, context);
}

/// @brief A context that can map an element to the canonical context in which it is stored.
template<typename T, typename C>
concept CanonicalizableContext = requires(const C& a, const T& e) {
    { a.get_canonical_context(e) } -> std::same_as<const C&>;
};

/// @brief Helper to create a view
template<typename T, typename C>
    requires CanonicalizableContext<T, C>
auto make_view(const T& element, const C& context) noexcept
{
    return View<T, C>(element, context.get_canonical_context(element));
}

// Storage decision: trivially copyable data types are treated as flat in-memory values.
template<typename T>
inline constexpr bool uses_trivial_storage_v = std::is_trivially_copyable_v<Data<T>> && std::is_default_constructible_v<Data<T>>;
}

#endif