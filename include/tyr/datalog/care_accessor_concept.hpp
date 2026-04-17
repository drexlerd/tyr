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

#ifndef TYR_DATALOG_CARE_ACCESSOR_CONCEPT_HPP_
#define TYR_DATALOG_CARE_ACCESSOR_CONCEPT_HPP_

#include "tyr/common/closed_interval.hpp"
#include "tyr/datalog/assignment.hpp"
#include "tyr/datalog/workspaces/facts.hpp"
#include "tyr/formalism/datalog/declarations.hpp"
#include "tyr/formalism/datalog/grounder_decl.hpp"

#include <concepts>

namespace tyr::datalog
{

template<typename P>
concept CareAccessorConcept = requires(const ConstFactsWorkspace& cws, const FactsWorkspace& ws) {
    typename P::FactSetAccessor;
    typename P::AssignmentSetAccessor;

    { P::make_fact_set_policy(cws, ws) } -> std::same_as<typename P::FactSetAccessor>;
    { P::make_assignment_set_policy(cws, ws) } -> std::same_as<typename P::AssignmentSetAccessor>;
};

}  // namespace tyr::datalog

#endif