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

namespace f = tyr::formalism;
namespace fd = tyr::formalism::datalog;

namespace tyr::analysis
{
// Build dependency graph: nodes = fluent predicates
static stratification::DepGraph build_dependency_graph(fd::ProgramView program, size_t num_predicates)
{
    stratification::DepGraph graph(num_predicates);

    for (const auto rule : program.get_rules())
    {
        const auto h_predicate = rule.get_head().get_predicate().get_index();

        for (const auto literal : rule.get_body().get_literals<f::FluentTag>())
        {
            const auto b_predicate = literal.get_atom().get_predicate().get_index();

            stratification::EdgeProps ep;
            ep.kind = literal.get_polarity() ? stratification::EdgeKind::NonStrict : stratification::EdgeKind::Strict;
            boost::add_edge(b_predicate.value, h_predicate.value, ep, graph);
        }
    }

    return graph;
}

RuleStrata compute_rule_stratification(fd::ProgramView program)
{
    const auto num_predicates = program.get_predicates<f::FluentTag>().size();

    // 1) dependency graph
    const auto dep = build_dependency_graph(program, num_predicates);

    // 2) SCC + check
    const auto [comp, num_comps] = stratification::compute_scc_and_check(dep);

    // 3) Condensed DAG
    const auto dag = stratification::build_condensation_dag(dep, comp, num_comps);

    // 4) Component strata
    const auto comp_stratum = stratification::compute_component_strata(dag);

    // 5) Assign each fluent predicate to a stratum
    auto predicate_stratum = std::vector<uint_t>(num_predicates, 0);
    for (uint_t i = 0; i < num_predicates; ++i)
        predicate_stratum[i] = comp_stratum[comp[i]];

    // 6) Bucket axioms by head stratum
    auto max_s = uint_t(0);
    for (auto s : comp_stratum)
        max_s = std::max(max_s, s);

    auto buckets = std::vector<IndexList<fd::Rule>>(max_s + 1);
    for (const auto rule : program.get_rules())
        buckets[predicate_stratum[uint_t(rule.get_head().get_predicate().get_index())]].push_back(rule.get_index());

    auto out = RuleStrata {};
    out.data.reserve(buckets.size());
    for (auto& b : buckets)
        out.data.emplace_back(RuleStratum(std::move(b)));

    return out;
}
}
