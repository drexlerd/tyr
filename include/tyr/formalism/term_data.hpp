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

#ifndef TYR_FORMALISM_TERM_DATA_HPP_
#define TYR_FORMALISM_TERM_DATA_HPP_

#include "tyr/common/types.hpp"
#include "tyr/common/types_utils.hpp"
#include "tyr/common/variant.hpp"
#include "tyr/formalism/declarations.hpp"
#include "tyr/formalism/object_index.hpp"
#include "tyr/formalism/parameter_index.hpp"

namespace tyr
{
template<>
struct Data<formalism::Term>
{
    using Variant = ::cista::offset::variant<Index<formalism::Object>, formalism::ParameterIndex>;

    Variant value;

    template<typename C>
    using ViewVariant = std::variant<View<Index<formalism::Object>, C>, formalism::ParameterIndex>;

    Data() = default;
    Data(Variant value) : value(value) {}
    // Python constructor
    template<typename C>
    Data(ViewVariant<C> value_) :
        value(std::visit(
            [](const auto& arg) -> Variant
            {
                using Alternative = std::decay_t<decltype(arg)>;

                if constexpr (std::is_same_v<Alternative, formalism::ParameterIndex>)
                    return Variant(arg);
                else if constexpr (std::is_same_v<Alternative, View<Index<formalism::Object>, C>>)
                    return Variant(arg.get_index());
                else
                    static_assert(dependent_false<Alternative>::value, "Missing case");
            },
            value_))
    {
    }

    void clear() noexcept { tyr::clear(value); }

    auto cista_members() const noexcept { return std::tie(value); }
    auto identifying_members() const noexcept { return std::tie(value); }
};

static_assert(!uses_trivial_storage_v<formalism::Term>);

}

#endif
