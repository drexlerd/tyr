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
#include "tyr/common/hash.hpp"
#include "tyr/common/vector.hpp"
#include "tyr/datalog/policies/aggregation.hpp"
#include "tyr/formalism/datalog/declarations.hpp"
#include "tyr/formalism/datalog/repository.hpp"

#include <algorithm>
#include <cassert>
#include <concepts>
#include <limits>
#include <optional>
#include <span>
#include <tuple>
#include <utility>
#include <variant>
#include <vector>

namespace tyr::datalog
{
/**
 * Annotations
 */

/// @brief `WitnessAnnotation` is the rule and binding in the rule delta repository whose ground rule is the witness for its ground atom in the head.
/// The witness lives in the rule delta repository.
struct WitnessAnnotation
{
public:
    WitnessAnnotation(formalism::datalog::RuleBindingView rule_row, Cost cost) : m_rule_row(rule_row), m_cost(cost) {}

    auto get_rule_row() const noexcept { return m_rule_row; }
    auto get_cost() const noexcept { return m_cost; }

    auto identifying_members() const noexcept { return std::tie(m_rule_row); }

private:
    formalism::datalog::RuleBindingView m_rule_row;
    Cost m_cost;
};

struct BaseAnnotation
{
public:
    explicit BaseAnnotation(Cost cost = Cost(0)) : m_cost(cost) {}

    auto get_cost() const noexcept { return m_cost; }

private:
    Cost m_cost;
};

using Annotation = std::variant<BaseAnnotation, WitnessAnnotation>;

inline Cost get_cost(const Annotation& annotation) noexcept
{
    return std::visit([](const auto& value) { return value.get_cost(); }, annotation);
}

template<typename Tag>
class AnnotationMap
{
public:
    using Binding = View<Index<formalism::RelationBinding<Tag>>, formalism::datalog::Repository>;
    using Relation = View<Index<Tag>, formalism::datalog::Repository>;
    using Row = Index<formalism::Row>;
    using Inner = UnorderedMap<Row, Annotation>;
    using Outer = UnorderedMap<Relation, Inner>;

    void clear() noexcept
    {
        for (auto& [_, annotations] : m_annotations)
            annotations.clear();
    }

    void insert_or_assign(Binding binding, Annotation annotation)
    {
        m_annotations[binding.get_relation()].insert_or_assign(binding.get_index().row, std::move(annotation));
    }

    const Annotation* find(Binding binding) const noexcept
    {
        const auto relation_it = m_annotations.find(binding.get_relation());
        if (relation_it == m_annotations.end())
            return nullptr;

        const auto annotation_it = relation_it->second.find(binding.get_index().row);
        return annotation_it == relation_it->second.end() ? nullptr : &annotation_it->second;
    }

    Annotation* find(Binding binding) noexcept
    {
        const auto relation_it = m_annotations.find(binding.get_relation());
        if (relation_it == m_annotations.end())
            return nullptr;

        const auto annotation_it = relation_it->second.find(binding.get_index().row);
        return annotation_it == relation_it->second.end() ? nullptr : &annotation_it->second;
    }

private:
    Outer m_annotations;
};

using SelectedPredicateAnnotations = AnnotationMap<formalism::Predicate<formalism::FluentTag>>;
using SelectedFunctionAnnotations = AnnotationMap<formalism::Function<formalism::FluentTag>>;

struct NumericIntervalAnnotation
{
    Index<formalism::Row> row;
    ClosedInterval<float_t> interval;
    Annotation annotation;
};

class NumericIntervalAnnotations
{
public:
    using Binding = formalism::datalog::FunctionBindingView<formalism::FluentTag>;
    using Relation = formalism::datalog::FunctionView<formalism::FluentTag>;
    using Entry = NumericIntervalAnnotation;
    using Entries = std::vector<Entry>;
    using Partitions = UnorderedMap<Relation, Entries>;

    void clear() noexcept
    {
        m_size = 0;
        for (auto& [_, entries] : m_partitions)
            entries.clear();
    }

    size_t size() const noexcept { return m_size; }

    const Partitions& partitions() const noexcept { return m_partitions; }

    void insert(Binding binding, ClosedInterval<float_t> interval, Annotation annotation)
    {
        if (empty(interval))
            return;

        m_partitions[binding.get_relation()].push_back(Entry { binding.get_index().row, interval, annotation });
        ++m_size;
    }

private:
    Partitions m_partitions;
    size_t m_size = 0;
};

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
