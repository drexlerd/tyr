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

#ifndef TYR_FORMALISM_FUNCTION_DATA_HPP_
#define TYR_FORMALISM_FUNCTION_DATA_HPP_

#include "tyr/common/types.hpp"
#include "tyr/common/types_utils.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/function_index.hpp"

namespace tyr
{

template<formalism::FactKind T>
struct Data<formalism::Function<T>>
{
    Index<formalism::Function<T>> index;
    ::cista::offset::string name;
    uint_t arity;

    Data() = default;
    Data(::cista::offset::string name_, uint_t arity_) : index(), name(std::move(name_)), arity(arity_) {}
    // Python constructor
    Data(const std::string& name_, uint_t arity_) : index(), name(name_), arity(arity_) {}
    Data(const Data& other) = delete;
    Data& operator=(const Data& other) = delete;
    Data(Data&& other) = default;
    Data& operator=(Data&& other) = default;

    void clear() noexcept
    {
        tyr::clear(index);
        tyr::clear(name);
        tyr::clear(arity);
    }

    auto cista_members() const noexcept { return std::tie(index, name, arity); }
    auto identifying_members() const noexcept { return std::tie(name, arity); }
};

static_assert(!uses_trivial_storage_v<formalism::Function<formalism::StaticTag>>);
}

#endif
