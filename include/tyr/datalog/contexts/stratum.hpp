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

#ifndef TYR_DATALOG_CONTEXTS_STRATUM_HPP_
#define TYR_DATALOG_CONTEXTS_STRATUM_HPP_

#include "tyr/datalog/care_accessor_concept.hpp"
#include "tyr/datalog/contexts/rule.hpp"
#include "tyr/datalog/declarations.hpp"
#include "tyr/datalog/policies/annotation_concept.hpp"
#include "tyr/datalog/policies/termination_concept.hpp"

namespace tyr::datalog
{
template<OrAnnotationPolicyConcept OrAP, AndAnnotationPolicyConcept AndAP, TerminationPolicyConcept TP, CareAccessorConcept CP>
struct ProgramExecutionContext;

template<OrAnnotationPolicyConcept OrAP, AndAnnotationPolicyConcept AndAP, TerminationPolicyConcept TP, CareAccessorConcept CP>
struct StratumExecutionContext
{
    StratumExecutionContext(RuleSchedulerStratum& scheduler, const ProgramExecutionContext<OrAP, AndAP, TP, CP>& ctx) : scheduler(scheduler), ctx(ctx) {}

    /**
     * Initialization
     */

    /**
     * Subcontext
     */

    auto get_rule_execution_context(Index<formalism::datalog::Rule> rule) { return RuleExecutionContext<OrAP, AndAP, TP, CP> { rule, *this }; }

    RuleSchedulerStratum& scheduler;
    const ProgramExecutionContext<OrAP, AndAP, TP, CP>& ctx;
};
}

#endif
