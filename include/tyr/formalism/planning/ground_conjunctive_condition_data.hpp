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

#ifndef TYR_FORMALISM_PLANNING_GROUND_CONJUNCTIVE_CONDITION_DATA_HPP_
#define TYR_FORMALISM_PLANNING_GROUND_CONJUNCTIVE_CONDITION_DATA_HPP_

#include "tyr/common/types.hpp"
#include "tyr/common/types_utils.hpp"
#include "tyr/formalism/binding_index.hpp"
#include "tyr/formalism/planning/boolean_operator_data.hpp"
#include "tyr/formalism/planning/declarations.hpp"
#include "tyr/formalism/planning/fdr_fact_data.hpp"
#include "tyr/formalism/planning/ground_conjunctive_condition_index.hpp"
#include "tyr/formalism/planning/ground_literal_index.hpp"

namespace tyr
{
template<>
struct Data<formalism::planning::GroundConjunctiveCondition>
{
    Index<formalism::planning::GroundConjunctiveCondition> index;
    IndexList<formalism::planning::GroundLiteral<formalism::StaticTag>> static_literals;
    IndexList<formalism::planning::GroundLiteral<formalism::DerivedTag>> derived_literals;
    DataList<formalism::planning::FDRFact<formalism::FluentTag>> positive_facts;
    DataList<formalism::planning::FDRFact<formalism::FluentTag>> negative_facts;
    DataList<formalism::planning::BooleanOperator<Data<formalism::planning::GroundFunctionExpression>>> numeric_constraints;

    Data() = default;
    Data(IndexList<formalism::planning::GroundLiteral<formalism::StaticTag>> static_literals_,
         IndexList<formalism::planning::GroundLiteral<formalism::DerivedTag>> derived_literals_,
         DataList<formalism::planning::FDRFact<formalism::FluentTag>> positive_facts_,
         DataList<formalism::planning::FDRFact<formalism::FluentTag>> negative_facts_,
         DataList<formalism::planning::BooleanOperator<Data<formalism::planning::GroundFunctionExpression>>> numeric_constraints_) :
        index(),
        static_literals(std::move(static_literals_)),
        derived_literals(std::move(derived_literals_)),
        positive_facts(std::move(positive_facts_)),
        negative_facts(std::move(negative_facts_)),
        numeric_constraints(std::move(numeric_constraints_))
    {
    }
    // Python constructor
    template<typename C>
    Data(const std::vector<View<Index<formalism::planning::GroundLiteral<formalism::StaticTag>>, C>>& static_literals_,
         const std::vector<View<Index<formalism::planning::GroundLiteral<formalism::DerivedTag>>, C>>& derived_literals_,
         const std::vector<View<Data<formalism::planning::FDRFact<formalism::FluentTag>>, C>>& positive_facts_,
         const std::vector<View<Data<formalism::planning::FDRFact<formalism::FluentTag>>, C>>& negative_facts_,
         const std::vector<View<Data<formalism::planning::BooleanOperator<Data<formalism::planning::GroundFunctionExpression>>>, C>>& numeric_constraints_) :
        index(),
        static_literals(),
        derived_literals(),
        positive_facts(),
        negative_facts(),
        numeric_constraints()
    {
        set(static_literals_, static_literals);
        set(derived_literals_, derived_literals);
        set(positive_facts_, positive_facts);
        set(negative_facts_, negative_facts);
        set(numeric_constraints_, numeric_constraints);
    }
    Data(const Data& other) = delete;
    Data& operator=(const Data& other) = delete;
    Data(Data&& other) = default;
    Data& operator=(Data&& other) = default;

    void clear() noexcept
    {
        tyr::clear(index);
        tyr::clear(static_literals);
        tyr::clear(derived_literals);
        tyr::clear(positive_facts);
        tyr::clear(negative_facts);
        tyr::clear(numeric_constraints);
    }

    template<formalism::FactKind T>
    const auto& get_literals() const
    {
        if constexpr (std::same_as<T, formalism::StaticTag>)
            return static_literals;
        else if constexpr (std::same_as<T, formalism::DerivedTag>)
            return derived_literals;
        else
            static_assert(dependent_false<T>::value, "Missing case");
    }

    template<formalism::PolarityKind T>
    const auto& get_facts() const
    {
        if constexpr (std::same_as<T, formalism::PositiveTag>)
            return positive_facts;
        else if constexpr (std::same_as<T, formalism::NegativeTag>)
            return negative_facts;
        else
            static_assert(dependent_false<T>::value, "Missing case");
    }

    auto cista_members() const noexcept { return std::tie(index, positive_facts, negative_facts, static_literals, derived_literals, numeric_constraints); }
    auto identifying_members() const noexcept { return std::tie(positive_facts, negative_facts, static_literals, derived_literals, numeric_constraints); }
};

static_assert(!uses_trivial_storage_v<formalism::planning::GroundConjunctiveCondition>);
}

#endif
