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

#ifndef TYR_FORMALISM_DATALOG_NUMERIC_EFFECT_DATA_HPP_
#define TYR_FORMALISM_DATALOG_NUMERIC_EFFECT_DATA_HPP_

#include "tyr/common/types.hpp"
#include "tyr/common/types_utils.hpp"
#include "tyr/formalism/datalog/declarations.hpp"
#include "tyr/formalism/datalog/function_expression_data.hpp"
#include "tyr/formalism/datalog/function_term_index.hpp"
#include "tyr/formalism/datalog/numeric_effect_index.hpp"

namespace tyr
{
template<formalism::datalog::NumericEffectOpKind Op, formalism::FactKind T>
struct Data<formalism::datalog::NumericEffect<Op, T>>
{
    static_assert(std::same_as<T, formalism::FluentTag>, "Datalog numeric effects are currently only supported for fluent functions.");

    Index<formalism::datalog::NumericEffect<Op, T>> index;
    Index<formalism::datalog::FunctionTerm<T>> fterm;
    Data<formalism::datalog::FunctionExpression> fexpr;

    Data() = default;
    Data(Index<formalism::datalog::FunctionTerm<T>> fterm_, Data<formalism::datalog::FunctionExpression> fexpr_) : index(), fterm(fterm_), fexpr(fexpr_) {}

    void clear() noexcept
    {
        tyr::clear(index);
        tyr::clear(fterm);
        tyr::clear(fexpr);
    }

    auto cista_members() const noexcept { return std::tie(index, fterm, fexpr); }
    auto identifying_members() const noexcept { return std::tie(Op::kind, fterm, fexpr); }
};

static_assert(!uses_trivial_storage_v<formalism::datalog::NumericEffect<formalism::datalog::OpAssign, formalism::FluentTag>>);

}

#endif
