/*
 * Copyright (C) 2025 Dominik Drexler
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

#ifndef TYR_FORMALISM_PLANNING_INVARIANTS_FORMATTER_HPP_
#define TYR_FORMALISM_PLANNING_INVARIANTS_FORMATTER_HPP_

#include "tyr/common/formatter.hpp"
#include "tyr/common/iostream.hpp"
#include "tyr/formalism/formatter.hpp"
#include "tyr/formalism/planning/invariants/invariant.hpp"

#include <fmt/core.h>
#include <fmt/ostream.h>
#include <fmt/ranges.h>
#include <ostream>

namespace tyr
{
/**
 * Forward declarations
 */

namespace formalism::planning::invariant
{
std::ostream& operator<<(std::ostream& os, const Invariant& el);

std::ostream& operator<<(std::ostream& os, const TempAtom& el);

template<typename Tag>
std::ostream& operator<<(std::ostream& os, const Substitution<Tag>& el);
}

/**
 * Declarations
 */

std::ostream& print(std::ostream& os, const formalism::planning::invariant::Invariant& el);

std::ostream& print(std::ostream& os, const formalism::planning::invariant::TempAtom& el);

template<typename Tag>
std::ostream& print(std::ostream& os, const formalism::planning::invariant::Substitution<Tag>& el);

namespace formalism::planning::invariant
{
std::ostream& operator<<(std::ostream& os, const Invariant& el);

std::ostream& operator<<(std::ostream& os, const TempAtom& el);

template<typename Tag>
std::ostream& operator<<(std::ostream& os, const Substitution<Tag>& el);
}
}
#endif
