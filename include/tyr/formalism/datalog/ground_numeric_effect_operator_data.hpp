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

#ifndef TYR_FORMALISM_DATALOG_GROUND_NUMERIC_EFFECT_OPERATOR_DATA_HPP_
#define TYR_FORMALISM_DATALOG_GROUND_NUMERIC_EFFECT_OPERATOR_DATA_HPP_

#include "tyr/common/types.hpp"
#include "tyr/common/types_utils.hpp"
#include "tyr/common/variant.hpp"
#include "tyr/formalism/datalog/declarations.hpp"
#include "tyr/formalism/datalog/ground_numeric_effect_index.hpp"

namespace tyr
{
template<>
struct Data<formalism::datalog::GroundNumericEffectOperator<formalism::FluentTag>>
{
    using Variant = ::cista::offset::variant<Index<formalism::datalog::GroundNumericEffect<formalism::Assign, formalism::FluentTag>>,
                                             Index<formalism::datalog::GroundNumericEffect<formalism::Increase, formalism::FluentTag>>,
                                             Index<formalism::datalog::GroundNumericEffect<formalism::Decrease, formalism::FluentTag>>,
                                             Index<formalism::datalog::GroundNumericEffect<formalism::ScaleUp, formalism::FluentTag>>,
                                             Index<formalism::datalog::GroundNumericEffect<formalism::ScaleDown, formalism::FluentTag>>>;

    Variant value;

    Data() = default;
    Data(Variant value_) : value(value_) {}

    void clear() noexcept { tyr::clear(value); }

    auto cista_members() const noexcept { return std::tie(value); }
    auto identifying_members() const noexcept { return std::tie(value); }
};

static_assert(!uses_trivial_storage_v<formalism::datalog::GroundNumericEffectOperator<formalism::FluentTag>>);

}

#endif
