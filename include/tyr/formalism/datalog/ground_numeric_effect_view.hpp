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

#ifndef TYR_FORMALISM_DATALOG_GROUND_NUMERIC_EFFECT_VIEW_HPP_
#define TYR_FORMALISM_DATALOG_GROUND_NUMERIC_EFFECT_VIEW_HPP_

#include "tyr/common/types.hpp"
#include "tyr/formalism/datalog/declarations.hpp"
#include "tyr/formalism/datalog/ground_function_expression_view.hpp"
#include "tyr/formalism/datalog/ground_function_term_view.hpp"
#include "tyr/formalism/datalog/ground_numeric_effect_index.hpp"

namespace tyr
{
template<formalism::datalog::NumericEffectOpKind Op, formalism::FactKind T, formalism::datalog::Context C>
class View<Index<formalism::datalog::GroundNumericEffect<Op, T>>, C>
{
private:
    const C* m_context;
    Index<formalism::datalog::GroundNumericEffect<Op, T>> m_handle;

public:
    View(Index<formalism::datalog::GroundNumericEffect<Op, T>> handle, const C& context) noexcept : m_context(&context), m_handle(handle) {}

    const auto& get_data() const noexcept { return get_repository(*m_context)[m_handle]; }
    const auto& get_context() const noexcept { return *m_context; }
    const auto& get_handle() const noexcept { return m_handle; }

    auto get_index() const noexcept { return m_handle; }
    auto get_fterm() const noexcept { return make_view(get_data().fterm, *m_context); }
    auto get_fexpr() const noexcept { return make_view(get_data().fexpr, *m_context); }

    auto identifying_members() const noexcept { return std::tie(m_handle, m_context->get_index()); }
};

}

#endif
