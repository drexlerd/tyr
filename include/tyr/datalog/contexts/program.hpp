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

#ifndef TYR_DATALOG_CONTEXTS_PROGRAM_HPP_
#define TYR_DATALOG_CONTEXTS_PROGRAM_HPP_

#include "tyr/common/onetbb.hpp"
#include "tyr/datalog/assignment_sets_accessor.hpp"
#include "tyr/datalog/contexts/stratum.hpp"
#include "tyr/datalog/declarations.hpp"
#include "tyr/datalog/fact_sets.hpp"
#include "tyr/datalog/fact_sets_accessor.hpp"
#include "tyr/datalog/policies/annotation_concept.hpp"
#include "tyr/datalog/policies/termination_concept.hpp"
#include "tyr/datalog/workspaces/program.hpp"
#include "tyr/datalog/workspaces/rule.hpp"

#include <ranges>

namespace tyr::datalog
{

template<OrAnnotationPolicyConcept OrAP, AndAnnotationPolicyConcept AndAP, TerminationPolicyConcept TP, SemanticTag S>
struct ProgramExecutionContext
{
    ProgramExecutionContext(ProgramWorkspace<OrAP, AndAP, TP>& ws, const ConstProgramWorkspace& cws) :
        ws(ws),
        cws(cws),
        fact_set_cp(FactSetAccessor<S>(cws.facts, ws.facts)),
        assignment_set_cp(AssignmentSetAccessor<S>(cws.facts, ws.facts))
    {
    }

    /**
     * Initialization
     */

    void clear() noexcept
    {
        // Clear the rules
        for (auto& rule : ws.rules)
            rule->clear();

        // Clear the annotation policy.
        for (auto& vec : ws.or_annot)
            vec.clear();
        ws.and_annot.clear();

        // Initialize the termination policy.
        ws.tp.clear();
        ws.tp.set_goals(ws.facts.goal_fact_sets);

        // Initialize first fact layer.
        for (const auto& set : ws.facts.fact_sets.predicate.get_sets())
        {
            for (const auto binding : set.get_bindings())
            {
                ws.or_ap.initialize_annotation(binding, ws.or_annot);
                ws.tp.achieve(binding);
            }
        }

        // Initialize assignment sets
        ws.facts.assignment_sets.insert(ws.facts.fact_sets);
        ws.facts.care_assignment_sets.insert(ws.facts.care_fact_sets);

        // Reset cost buckets.
        ws.cost_buckets.clear();
    }

    /**
     * Subcontext
     */

    auto get_stratum_execution_contexts()
    {
        return ws.schedulers.data
               | std::views::transform([this](RuleSchedulerStratum& scheduler) { return StratumExecutionContext<OrAP, AndAP, TP, S> { scheduler, *this }; });
    }

    ProgramWorkspace<OrAP, AndAP, TP>& ws;
    const ConstProgramWorkspace& cws;

    const FactSetAccessor<S> fact_set_cp;
    const AssignmentSetAccessor<S> assignment_set_cp;
};
}

#endif
