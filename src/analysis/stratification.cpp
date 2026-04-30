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

#include "tyr/analysis/stratification.hpp"

#include "stratification_utils.hpp"
#include "tyr/formalism/datalog/repository.hpp"  // for Repository
#include "tyr/formalism/datalog/views.hpp"

#include <type_traits>

namespace f = tyr::formalism;
namespace fd = tyr::formalism::datalog;

namespace tyr::analysis
{
namespace
{
struct RelationVertexMap
{
    size_t num_predicates;
    size_t num_functions;

    uint_t get_vertex(Index<f::Predicate<f::FluentTag>> predicate) const { return uint_t(predicate); }
    uint_t get_vertex(Index<f::Function<f::FluentTag>> function) const { return static_cast<uint_t>(num_predicates + uint_t(function)); }
    size_t size() const { return num_predicates + num_functions; }
};

void add_function_dependencies(fd::FunctionExpressionView expression, uint_t head_vertex, const RelationVertexMap& vertices, stratification::DepGraph& graph);

void add_function_dependencies(float_t, uint_t, const RelationVertexMap&, stratification::DepGraph&)
{
}

template<f::OpKind O>
void add_function_dependencies(fd::LiftedUnaryOperatorView<O> expression, uint_t head_vertex, const RelationVertexMap& vertices, stratification::DepGraph& graph)
{
    add_function_dependencies(expression.get_arg(), head_vertex, vertices, graph);
}

template<f::OpKind O>
void add_function_dependencies(fd::LiftedBinaryOperatorView<O> expression, uint_t head_vertex, const RelationVertexMap& vertices, stratification::DepGraph& graph)
{
    add_function_dependencies(expression.get_lhs(), head_vertex, vertices, graph);
    add_function_dependencies(expression.get_rhs(), head_vertex, vertices, graph);
}

template<f::OpKind O>
void add_function_dependencies(fd::LiftedMultiOperatorView<O> expression, uint_t head_vertex, const RelationVertexMap& vertices, stratification::DepGraph& graph)
{
    for (const auto arg : expression.get_args())
        add_function_dependencies(arg, head_vertex, vertices, graph);
}

template<f::FactKind T>
void add_function_dependencies(fd::FunctionTermView<T>, uint_t, const RelationVertexMap&, stratification::DepGraph&)
{
}

void add_function_dependencies(fd::FunctionTermView<f::FluentTag> term, uint_t head_vertex, const RelationVertexMap& vertices, stratification::DepGraph& graph)
{
    stratification::EdgeProps ep;
    ep.kind = stratification::EdgeKind::NonStrict;
    boost::add_edge(vertices.get_vertex(term.get_function().get_index()), head_vertex, ep, graph);
}

void add_function_dependencies(fd::LiftedArithmeticOperatorView expression, uint_t head_vertex, const RelationVertexMap& vertices, stratification::DepGraph& graph)
{
    visit([&](auto&& arg) { add_function_dependencies(arg, head_vertex, vertices, graph); }, expression.get_variant());
}

void add_function_dependencies(fd::FunctionExpressionView expression, uint_t head_vertex, const RelationVertexMap& vertices, stratification::DepGraph& graph)
{
    visit([&](auto&& arg) { add_function_dependencies(arg, head_vertex, vertices, graph); }, expression.get_variant());
}

void add_function_dependencies(fd::LiftedBooleanOperatorView expression, uint_t head_vertex, const RelationVertexMap& vertices, stratification::DepGraph& graph)
{
    visit(
        [&](auto&& arg)
        {
            add_function_dependencies(arg.get_lhs(), head_vertex, vertices, graph);
            add_function_dependencies(arg.get_rhs(), head_vertex, vertices, graph);
        },
        expression.get_variant());
}

template<fd::NumericEffectOpKind Op>
void add_numeric_effect_head_dependencies(fd::NumericEffectView<Op, f::FluentTag> effect,
                                          uint_t head_vertex,
                                          const RelationVertexMap& vertices,
                                          stratification::DepGraph& graph)
{
    if constexpr (!std::is_same_v<Op, fd::OpAssign>)
        add_function_dependencies(effect.get_fterm(), head_vertex, vertices, graph);

    add_function_dependencies(effect.get_fexpr(), head_vertex, vertices, graph);
}

void add_body_dependencies(fd::RuleView rule, uint_t head_vertex, const RelationVertexMap& vertices, stratification::DepGraph& graph)
{
    for (const auto literal : rule.get_body().get_literals<f::FluentTag>())
    {
        const auto body_vertex = vertices.get_vertex(literal.get_atom().get_predicate().get_index());

        stratification::EdgeProps ep;
        ep.kind = literal.get_polarity() ? stratification::EdgeKind::NonStrict : stratification::EdgeKind::Strict;
        boost::add_edge(body_vertex, head_vertex, ep, graph);
    }

    for (const auto constraint : rule.get_body().get_numeric_constraints())
        add_function_dependencies(constraint, head_vertex, vertices, graph);
}

uint_t get_head_vertex(fd::AtomView<f::FluentTag> head, const RelationVertexMap& vertices) { return vertices.get_vertex(head.get_predicate().get_index()); }

uint_t get_head_vertex(fd::NumericEffectOperatorView<f::FluentTag> head, const RelationVertexMap& vertices)
{
    return visit(
        [&](auto&& effect)
        {
            return vertices.get_vertex(effect.get_fterm().get_function().get_index());
        },
        head.get_variant());
}

}  // namespace

// Build dependency graph: nodes = fluent predicates and fluent functions.
static stratification::DepGraph build_dependency_graph(fd::ProgramView program, const RelationVertexMap& vertices)
{
    stratification::DepGraph graph(vertices.size());

    for (const auto rule : program.get_rules())
    {
        visit(
            [&](auto&& head)
            {
                using Head = std::decay_t<decltype(head)>;

                if constexpr (std::is_same_v<Head, fd::AtomView<f::FluentTag>>)
                {
                    add_body_dependencies(rule, get_head_vertex(head, vertices), vertices, graph);
                }
                else
                {
                    const auto head_vertex = get_head_vertex(head, vertices);

                    add_body_dependencies(rule, head_vertex, vertices, graph);
                    visit([&](auto&& effect) { add_numeric_effect_head_dependencies(effect, head_vertex, vertices, graph); }, head.get_variant());
                }
            },
            rule.get_head());
    }

    return graph;
}

RuleStrata compute_rule_stratification(fd::ProgramView program)
{
    const auto vertices = RelationVertexMap {
        program.get_predicates<f::FluentTag>().size(),
        program.get_functions<f::FluentTag>().size(),
    };

    // 1) dependency graph
    const auto dep = build_dependency_graph(program, vertices);

    // 2) SCC + check
    const auto [comp, num_comps] = stratification::compute_scc_and_check(dep);

    // 3) Condensed DAG
    const auto dag = stratification::build_condensation_dag(dep, comp, num_comps);

    // 4) Component strata
    const auto comp_stratum = stratification::compute_component_strata(dag);

    // 5) Assign each fluent predicate/function relation to a stratum.
    auto relation_stratum = std::vector<uint_t>(vertices.size(), 0);
    for (uint_t i = 0; i < vertices.size(); ++i)
        relation_stratum[i] = comp_stratum[comp[i]];

    // 6) Bucket rules by head stratum.
    auto max_s = uint_t(0);
    for (auto s : comp_stratum)
        max_s = std::max(max_s, s);

    auto buckets = std::vector<IndexList<fd::Rule>>(max_s + 1);
    for (const auto rule : program.get_rules())
    {
        visit(
            [&](auto&& head)
            {
                using Head = std::decay_t<decltype(head)>;

                if constexpr (std::is_same_v<Head, fd::AtomView<f::FluentTag>>)
                {
                    buckets[relation_stratum[get_head_vertex(head, vertices)]].push_back(rule.get_index());
                }
                else
                {
                    buckets[relation_stratum[get_head_vertex(head, vertices)]].push_back(rule.get_index());
                }
            },
            rule.get_head());
    }

    auto out = RuleStrata {};
    out.data.reserve(buckets.size());
    for (auto& b : buckets)
        out.data.emplace_back(RuleStratum(std::move(b)));

    return out;
}
}
