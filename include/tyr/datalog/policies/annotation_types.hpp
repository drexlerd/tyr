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

#ifndef TYR_SOLVER_POLICIES_ANNOTATION_TYPES_HPP_
#define TYR_SOLVER_POLICIES_ANNOTATION_TYPES_HPP_

#include "tyr/common/closed_interval.hpp"
#include "tyr/common/config.hpp"
#include "tyr/common/vector.hpp"
#include "tyr/datalog/policies/aggregation.hpp"
#include "tyr/formalism/datalog/declarations.hpp"
#include "tyr/formalism/datalog/repository.hpp"

#include <algorithm>
#include <cassert>
#include <concepts>
#include <limits>
#include <optional>
#include <tuple>
#include <variant>
#include <vector>

namespace tyr::datalog
{
/**
 * Annotations
 */

/// @brief `Witness` is the rule and binding in the rule delta repository whose ground rule is the witness for its ground atom in the head.
/// The witness lives in the rule delta repository.
struct Witness
{
public:
    Witness(formalism::datalog::RuleBindingView rule_row, Cost cost) : m_rule_row(rule_row), m_cost(cost) {}

    auto get_rule_row() const noexcept { return m_rule_row; }
    auto get_cost() const noexcept { return m_cost; }

    auto identifying_members() const noexcept { return std::tie(m_rule_row); }

private:
    formalism::datalog::RuleBindingView m_rule_row;
    Cost m_cost;
};

struct BaseCase
{
public:
    explicit BaseCase(Cost cost = Cost(0)) : m_cost(cost) {}

    auto get_cost() const noexcept { return m_cost; }

private:
    Cost m_cost;
};

using Annotation = std::variant<BaseCase, Witness>;

inline Cost get_cost(const Annotation& annotation) noexcept
{
    return std::visit([](const auto& value) { return value.get_cost(); }, annotation);
}

using AndAnnotationsMap = UnorderedMap<formalism::datalog::PredicateBindingView<formalism::FluentTag>, Annotation>;
using NumericAndAnnotationsMap = UnorderedMap<formalism::datalog::FunctionBindingView<formalism::FluentTag>, Annotation>;

static_assert(sizeof(AndAnnotationsMap::value_type) == 48);
static_assert(sizeof(NumericAndAnnotationsMap::value_type) == 48);

struct NumericIntervalAnnotation
{
    formalism::datalog::FunctionBindingView<formalism::FluentTag> binding;
    ClosedInterval<float_t> interval;
    Annotation annotation;
};

using NumericIntervalAnnotations = std::vector<NumericIntervalAnnotation>;

inline void insert_numeric_interval_annotation(NumericIntervalAnnotations& annotations,
                                               formalism::datalog::FunctionBindingView<formalism::FluentTag> binding,
                                               ClosedInterval<float_t> interval,
                                               Annotation annotation)
{
    if (empty(interval))
        return;

    const auto cost = get_cost(annotation);
    const auto binding_equal = EqualTo<formalism::datalog::FunctionBindingView<formalism::FluentTag>> {};

    for (const auto& entry : annotations)
        if (binding_equal(entry.binding, binding) && get_cost(entry.annotation) <= cost && subset(interval, entry.interval))
            return;

    annotations.erase(std::remove_if(annotations.begin(),
                                     annotations.end(),
                                     [&](const auto& entry)
                                     {
                                         return binding_equal(entry.binding, binding) && cost <= get_cost(entry.annotation)
                                                && subset(entry.interval, interval);
                                     }),
                      annotations.end());

    annotations.push_back(NumericIntervalAnnotation { binding, interval, annotation });
}

struct CostUpdate
{
    std::optional<Cost> old_cost;
    Cost new_cost;

    CostUpdate() noexcept : old_cost(std::nullopt), new_cost(Cost(0)) { assert(is_monoton()); }
    CostUpdate(std::optional<Cost> old_cost, Cost new_cost) noexcept : old_cost(old_cost), new_cost(new_cost) { assert(is_monoton()); }
    CostUpdate(Cost old_cost, Cost new_cost) noexcept :
        old_cost(old_cost == std::numeric_limits<Cost>::max() ? std::nullopt : std::optional<Cost>(old_cost)),
        new_cost(new_cost)
    {
        assert(is_monoton());
    }

    bool is_monoton() const noexcept { return !old_cost || new_cost <= old_cost.value(); }
};

}

#endif
