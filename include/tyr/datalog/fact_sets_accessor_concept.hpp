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

#ifndef TYR_DATALOG_FACT_SETS_ACCESSOR_CONCEPT_HPP_
#define TYR_DATALOG_FACT_SETS_ACCESSOR_CONCEPT_HPP_

#include "tyr/common/closed_interval.hpp"
#include "tyr/datalog/assignment.hpp"
#include "tyr/datalog/workspaces/facts.hpp"
#include "tyr/formalism/datalog/declarations.hpp"
#include "tyr/formalism/datalog/grounder_decl.hpp"

#include <concepts>

namespace tyr::datalog
{

/**
 * Tagged fact-set policies
 */

template<typename P, typename T>
concept TaggedFactSetCareAccessorForKind = formalism::FactKind<T>
                                           && requires(const P& policy,
                                                       formalism::datalog::LiteralView<T> lit,
                                                       formalism::datalog::FunctionTermView<T> fterm,
                                                       formalism::datalog::GroundLiteralView<T> glit,
                                                       formalism::datalog::GroundFunctionTermView<T> gfterm,
                                                       const formalism::datalog::GrounderContext& context) {
                                                  { policy.predicate.check(lit, context) } -> std::same_as<bool>;
                                                  { policy.function.check(fterm, context) } -> std::same_as<float_t>;
                                                  { policy.predicate.check(glit) } -> std::same_as<bool>;
                                                  { policy.function.check(gfterm) } -> std::same_as<float_t>;
                                              };

/**
 * Combined fact-set policies
 */

template<typename P, typename T>
concept FactSetCareAccessorForKind =
    formalism::FactKind<T> && requires(const P& policy) { requires TaggedFactSetCareAccessorForKind<decltype(policy.template get<T>()), T>; };

template<typename P>
concept FactSetCareAccessorConcept = FactSetCareAccessorForKind<P, formalism::StaticTag> && FactSetCareAccessorForKind<P, formalism::FluentTag>;

}  // namespace tyr::datalog

#endif