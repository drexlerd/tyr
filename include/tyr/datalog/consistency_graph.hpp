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

#ifndef TYR_DATALOG_CONSISTENCY_GRAPH_HPP_
#define TYR_DATALOG_CONSISTENCY_GRAPH_HPP_

#include "tyr/analysis/declarations.hpp"
#include "tyr/common/vector.hpp"
#include "tyr/datalog/assignment_sets.hpp"
#include "tyr/datalog/declarations.hpp"
#include "tyr/datalog/delta_kpkc_graph.hpp"
#include "tyr/formalism/datalog/repository.hpp"
#include "tyr/formalism/datalog/views.hpp"

#include <boost/dynamic_bitset/dynamic_bitset.hpp>
#include <optional>
#include <ranges>
#include <sstream>
#include <vector>

namespace tyr::datalog
{
class StaticConsistencyGraph;

namespace details
{
/**
 * For mapping rule bindings to literal bindings
 */

struct RuleToLiteralInfoMappings
{
    // For building vertex assignments (p/o)
    std::vector<std::vector<uint_t>> parameter_to_infos;

    // For building edge assignments (p/o,q/c)
    std::vector<std::vector<std::vector<uint_t>>> parameter_pairs_to_infos;
    std::vector<std::vector<uint_t>> parameter_to_infos_with_constants;

    // For global vertex assignments (c) for constant c
    std::vector<uint_t> infos_with_constants;
    // For global edge assignments (c,c') for constants c,c'
    std::vector<uint_t> infos_with_constant_pairs;
};

struct RuleToLiteralPositionMappings
{
    std::vector<std::pair<uint_t, Index<formalism::Object>>> constant_positions;
    std::vector<std::vector<uint_t>> parameter_to_positions;
};

template<formalism::FactKind T>
struct RuleToLiteralInfo
{
    Index<formalism::Predicate<T>> predicate;
    bool polarity;
    size_t kpkc_arity;
    size_t num_parameters;
    size_t num_constants;

    RuleToLiteralPositionMappings position_mappings;
};

template<formalism::FactKind T>
struct TaggedRuleToLiteralInfos
{
    std::vector<RuleToLiteralInfo<T>> infos;

    RuleToLiteralInfoMappings info_mappings;
};

struct RuleToLiteralInfos
{
    details::TaggedRuleToLiteralInfos<formalism::StaticTag> static_indexed;
    details::TaggedRuleToLiteralInfos<formalism::FluentTag> fluent_indexed;

    template<formalism::FactKind T>
    const auto& get() const noexcept
    {
        if constexpr (std::is_same_v<T, formalism::StaticTag>)
            return static_indexed;
        else if constexpr (std::is_same_v<T, formalism::FluentTag>)
            return fluent_indexed;
        else
            static_assert(dependent_false<T>::value, "Missing case");
    }
};

template<formalism::FactKind T>
struct RuleToFunctionTermInfo
{
    Index<formalism::Function<T>> function;
    size_t kpkc_arity;
    size_t num_parameters;
    size_t num_constants;

    RuleToLiteralPositionMappings position_mappings;
};

template<formalism::FactKind T>
struct TaggedRuleToFunctionTermInfos
{
    UnorderedMap<Index<formalism::datalog::FunctionTerm<T>>, RuleToFunctionTermInfo<T>> infos;

    RuleToLiteralInfoMappings info_mappings;
};

struct RuleToConstraintInfo
{
    TaggedRuleToFunctionTermInfos<formalism::StaticTag> static_infos;
    TaggedRuleToFunctionTermInfos<formalism::FluentTag> fluent_infos;

    size_t kpkc_arity;

    template<formalism::FactKind T>
    const auto& get() const noexcept
    {
        if constexpr (std::is_same_v<T, formalism::StaticTag>)
            return static_infos;
        else if constexpr (std::is_same_v<T, formalism::FluentTag>)
            return fluent_infos;
        else
            static_assert(dependent_false<T>::value, "Missing case");
    }
};

struct RuleToRuleToConstraintInfos
{
    std::vector<RuleToConstraintInfo> infos;
};

/**
 * Vertex
 */

/// @brief A vertex [parameter_index/object_index] in the consistency graph.
class Vertex
{
private:
    formalism::ParameterIndex m_parameter_index;
    Index<formalism::Object> m_object_index;

public:
    Vertex(formalism::ParameterIndex parameter_index, Index<formalism::Object> object_index) noexcept :
        m_parameter_index(parameter_index),
        m_object_index(object_index)
    {
    }

    auto get_parameter_index() const noexcept { return m_parameter_index; }
    auto get_object_index() const noexcept { return m_object_index; }
};

using Vertices = std::vector<Vertex>;

/**
 * Edge
 */

class Edge
{
private:
    Vertex m_vi;
    Vertex m_vj;

public:
    Edge(Vertex vi, Vertex vj) noexcept : m_vi(std::move(vi)), m_vj(std::move(vj)) {}

    const auto& vi() const noexcept { return m_vi; }
    const auto& vj() const noexcept { return m_vj; }
};

}

class StaticConsistencyGraph
{
private:
    /// @brief Helper to initialize vertices.
    std::tuple<details::Vertices, std::vector<std::vector<uint_t>>, std::vector<std::vector<uint_t>>>
    compute_vertices(const details::TaggedRuleToLiteralInfos<formalism::StaticTag>& indexed_literals,
                     const analysis::VariableDomainList& parameter_domains,
                     size_t num_objects,
                     uint_t begin_parameter_index,
                     uint_t end_parameter_index,
                     const TaggedAssignmentSets<formalism::StaticTag>& static_assignment_sets);

    /// @brief Helper to initialize edges.
    kpkc::DeduplicatedAdjacencyMatrix compute_edges(const details::TaggedRuleToLiteralInfos<formalism::StaticTag>& indexed_literals,
                                                    const TaggedAssignmentSets<formalism::StaticTag>& static_assignment_sets,
                                                    const details::Vertices& vertices,
                                                    const std::vector<std::vector<uint_t>>& vertex_partitions);

public:
    StaticConsistencyGraph(formalism::datalog::RuleView rule,
                           formalism::datalog::ConjunctiveConditionView condition,
                           formalism::datalog::ConjunctiveConditionView unary_overapproximation_condition,
                           formalism::datalog::ConjunctiveConditionView binary_overapproximation_condition,
                           formalism::datalog::ConjunctiveConditionView static_binary_overapproximation_condition,
                           const analysis::VariableDomainList& parameter_domains,
                           size_t num_objects,
                           size_t num_fluent_predicates,
                           uint_t begin_parameter_index,
                           uint_t end_parameter_index,
                           const TaggedAssignmentSets<formalism::StaticTag>& static_assignment_sets);

    void initialize_dynamic_consistency_graphs(const AssignmentSets& assignment_sets,
                                               const kpkc::GraphLayout& layout,
                                               kpkc::Graph& delta_graph,
                                               kpkc::Graph& full_graph,
                                               std::vector<kpkc::Edge>& delta_edges) const;

    auto get_vertices() const noexcept { return std::ranges::subrange(m_vertices.cbegin(), m_vertices.cend()); }

    const details::Vertex& get_vertex(uint_t index) const;

    size_t get_num_vertices() const noexcept;

    formalism::datalog::RuleView get_rule() const noexcept;
    formalism::datalog::ConjunctiveConditionView get_condition() const noexcept;
    const formalism::datalog::VariableDependencyGraph& get_variable_dependeny_graph() const noexcept;
    const std::vector<std::vector<uint_t>>& get_vertex_partitions() const noexcept;
    const std::vector<std::vector<uint_t>>& get_object_to_vertex_per_partition() const noexcept;
    const kpkc::DeduplicatedAdjacencyMatrix& get_adjacency_matrix() const noexcept;

private:
    formalism::datalog::RuleView m_rule;
    formalism::datalog::ConjunctiveConditionView m_condition;
    formalism::datalog::ConjunctiveConditionView m_unary_overapproximation_condition;
    formalism::datalog::ConjunctiveConditionView m_binary_overapproximation_condition;

    formalism::datalog::VariableDependencyGraph m_unary_overapproximation_vdg;
    formalism::datalog::VariableDependencyGraph m_binary_overapproximation_vdg;

    /* The data member of the consistency graph. */
    details::Vertices m_vertices;

    // Adjacency list of edges.
    std::vector<std::vector<uint_t>> m_vertex_partitions;
    std::vector<std::vector<uint_t>> m_object_to_vertex_per_partition;

    kpkc::GraphLayout m_layout;
    kpkc::DeduplicatedAdjacencyMatrix m_matrix;

    details::RuleToLiteralInfos m_unary_overapproximation_indexed_literals;
    details::RuleToLiteralInfos m_binary_overapproximation_indexed_literals;

    details::RuleToRuleToConstraintInfos m_unary_overapproximation_indexed_constraints;
    details::RuleToRuleToConstraintInfos m_binary_overapproximation_indexed_constraints;
};

extern std::pair<formalism::datalog::GroundConjunctiveConditionView, bool>
create_ground_nullary_conjunctive_condition(formalism::datalog::ConjunctiveConditionView condition, formalism::datalog::Repository& context);

extern std::pair<formalism::datalog::RuleView, bool>
create_overapproximation_rule(size_t k, formalism::datalog::RuleView element, formalism::datalog::Repository& context);

extern std::pair<formalism::datalog::RuleView, bool>
create_static_overapproximation_rule(size_t k, formalism::datalog::RuleView element, formalism::datalog::Repository& context);

extern std::pair<formalism::datalog::RuleView, bool>
create_overapproximation_conflicting_rule(size_t k, formalism::datalog::RuleView element, formalism::datalog::Repository& context);

}

#endif
