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

#ifndef TYR_FORMALISM_UNIFICATION_FORMATTER_HPP_
#define TYR_FORMALISM_UNIFICATION_FORMATTER_HPP_

#include "tyr/formalism/formatter.hpp"
#include "tyr/formalism/unification/substitution.hpp"

#include <sstream>

namespace fmt
{

template<typename T>
struct formatter<tyr::formalism::unification::SubstitutionFunction<T>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template<typename FormatContext>
    auto format(const tyr::formalism::unification::SubstitutionFunction<T>& value, FormatContext& ctx) const
    {
        auto os = std::stringstream {};
        os << "SubstitutionFunction(\n";

        {
            tyr::IndentScope scope(os);

            os << tyr::print_indent;
            fmt::print(os, "{}{}\n", "parameters = ", value.parameters());

            os << tyr::print_indent << "bindings = [\n";
            {
                tyr::IndentScope scope(os);
                for (const auto& parameter : value.parameters())
                {
                    os << tyr::print_indent;
                    fmt::print(os, "{} -> {}\n", parameter, value.is_bound(parameter) ? tyr::to_string(*value[parameter]) : "unbound");
                }
            }
        }
        os << tyr::print_indent << ")";

        return fmt::format_to(ctx.out(), "{}", os.str());
    }
};

}

#endif
