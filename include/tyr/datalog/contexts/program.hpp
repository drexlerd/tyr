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
#include "tyr/datalog/contexts/stratum.hpp"
#include "tyr/datalog/declarations.hpp"
#include "tyr/datalog/fact_sets.hpp"
#include "tyr/datalog/policies/annotation_concept.hpp"
#include "tyr/datalog/policies/termination_concept.hpp"
#include "tyr/datalog/workspaces/program.hpp"
#include "tyr/datalog/workspaces/rule.hpp"

#include <ranges>
#include <cassert>

namespace tyr::datalog
{

template<OrAnnotationPolicyConcept OrAP, AndAnnotationPolicyConcept AndAP, TerminationPolicyConcept TP>
struct ProgramExecutionContext
{
    class In
    {
    public:
        explicit In(const ConstProgramWorkspace& cws) : m_cws(cws) {}

        const auto& facts() const noexcept { return m_cws.facts; }
        const auto& rules() const noexcept { return m_cws.rules; }

    private:
        const ConstProgramWorkspace& m_cws;
    };

    class Out
    {
    public:
        explicit Out(ProgramWorkspace<OrAP, AndAP, TP>& ws) : m_ws(ws) {}

        auto& facts() noexcept { return m_ws.facts; }
        const auto& facts() const noexcept { return m_ws.facts; }
        auto& or_ap() noexcept { return m_ws.or_ap; }
        const auto& or_ap() const noexcept { return m_ws.or_ap; }
        auto& and_annot() noexcept { return m_ws.and_annot; }
        const auto& and_annot() const noexcept { return m_ws.and_annot; }
        auto& numeric_and_annot() noexcept { return m_ws.numeric_and_annot; }
        const auto& numeric_and_annot() const noexcept { return m_ws.numeric_and_annot; }
        const auto& numeric_support_selector() const noexcept
        {
            assert(m_ws.numeric_support_selector.has_value());
            return *m_ws.numeric_support_selector;
        }
        void rebuild_numeric_support_selector(const TaggedFactSets<formalism::StaticTag>& static_fact_sets)
        {
            m_ws.numeric_support_selector.emplace(FactSets { static_fact_sets, m_ws.facts.fact_sets }, m_ws.numeric_and_annot);
        }
        auto& tp() noexcept { return m_ws.tp; }
        const auto& tp() const noexcept { return m_ws.tp; }
        auto& rules() noexcept { return m_ws.rules; }
        const auto& rules() const noexcept { return m_ws.rules; }
        auto& datalog_builder() noexcept { return m_ws.datalog_builder; }
        const auto& datalog_builder() const noexcept { return m_ws.datalog_builder; }
        auto& workspace_repository() noexcept { return m_ws.workspace_repository; }
        const auto& workspace_repository() const noexcept { return m_ws.workspace_repository; }
        auto& schedulers() noexcept { return m_ws.schedulers; }
        const auto& schedulers() const noexcept { return m_ws.schedulers; }
        auto& cost_buckets() noexcept { return m_ws.cost_buckets; }
        const auto& cost_buckets() const noexcept { return m_ws.cost_buckets; }
        auto& statistics() noexcept { return m_ws.statistics; }
        const auto& statistics() const noexcept { return m_ws.statistics; }

    private:
        ProgramWorkspace<OrAP, AndAP, TP>& m_ws;
    };

    ProgramExecutionContext(ProgramWorkspace<OrAP, AndAP, TP>& ws, const ConstProgramWorkspace& cws) : m_in(cws), m_out(ws) {}

    /**
     * Initialization
     */

    void clear() noexcept
    {
        auto& out = this->out();

        // Clear the rules
        for (auto& rule : out.rules())
            if (rule)
                rule->clear();

        // Clear the annotation policy.
        out.and_annot().clear();
        out.numeric_and_annot().clear();

        // Initialize the termination policy.
        out.tp().reset();

        // Initialize first fact layer.
        for (const auto& set : out.facts().fact_sets.predicate.get_sets())
        {
            for (const auto binding : set.get_bindings())
            {
                out.or_ap().initialize_annotation(binding, out.and_annot());
                out.facts().assignment_sets.predicate.insert(binding);
            }
        }

        // Initialize first numeric assignment layer.
        for (const auto& set : out.facts().fact_sets.function.get_sets())
        {
            const auto bindings = set.get_bindings();
            const auto& values = set.get_values();

            for (uint_t i = 0; i < bindings.size(); ++i)
            {
                out.or_ap().initialize_annotation(bindings[i], values[i], out.numeric_and_annot());
                out.facts().assignment_sets.function.insert(bindings[i], values[i]);
            }
        }

        // Reset cost buckets.
        out.cost_buckets().clear();

        out.rebuild_numeric_support_selector(in().facts().fact_sets);
    }

    /**
     * Subcontext
     */

    auto get_stratum_execution_contexts()
    {
        return out().schedulers().data
               | std::views::transform([this](RuleSchedulerStratum& scheduler) { return StratumExecutionContext<OrAP, AndAP, TP> { scheduler, *this }; });
    }

    const auto& in() const noexcept { return m_in; }
    auto& out() noexcept { return m_out; }
    const auto& out() const noexcept { return m_out; }

private:
    In m_in;
    Out m_out;
};
}

#endif
