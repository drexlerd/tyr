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

#include "tyr/formalism/planning/invariants/formatter.hpp"

#include <fmt/core.h>
#include <fmt/ostream.h>
#include <fmt/ranges.h>
#include <ostream>

namespace tyr
{

/**
 * Declarations
 */

std::ostream& print(std::ostream& os, const formalism::planning::invariant::Invariant& el)
{
    os << "Invariant(\n";

    {
        IndentScope indent { os };

        os << print_indent << "num_rigid_variables = " << el.num_rigid_variables << ",\n";
        os << print_indent << "num_counted_variables = " << el.num_counted_variables << ",\n";
        os << print_indent << "atoms = " << el.atoms << ",\n";
        os << print_indent << "predicates = " << el.predicates << ",\n";
    }

    os << ")";

    return os;
}

std::ostream& print(std::ostream& os, const formalism::planning::invariant::TempAtom& el)
{
    fmt::print(os, "{}({})", to_string(el.predicate.get_name()), fmt::join(to_strings(el.terms), " "));
    return os;
}

template<typename Tag>
std::ostream& print(std::ostream& os, const formalism::planning::invariant::Substitution<Tag>& el)
{
    fmt::print(os, "{{{}}}", fmt::join(to_strings(el.data), ", "));
    return os;
}

template std::ostream& print(std::ostream& os, const formalism::planning::invariant::Substitution<formalism::planning::invariant::ActionTag>& el);
template std::ostream& print(std::ostream& os, const formalism::planning::invariant::Substitution<formalism::planning::invariant::EffectTag>& el);
template std::ostream& print(std::ostream& os, const formalism::planning::invariant::Substitution<formalism::planning::invariant::InvariantTag>& el);

namespace formalism::planning::invariant
{
std::ostream& operator<<(std::ostream& os, const Invariant& el) { return tyr::print(os, el); }

std::ostream& operator<<(std::ostream& os, const TempAtom& el) { return tyr::print(os, el); }

template<typename Tag>
std::ostream& operator<<(std::ostream& os, const Substitution<Tag>& el)
{
    return tyr::print(os, el);
}

template std::ostream& operator<<(std::ostream& os, const Substitution<ActionTag>& el);
template std::ostream& operator<<(std::ostream& os, const Substitution<EffectTag>& el);
template std::ostream& operator<<(std::ostream& os, const Substitution<InvariantTag>& el);
}
}
