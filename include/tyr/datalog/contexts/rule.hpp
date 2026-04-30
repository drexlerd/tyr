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

#ifndef TYR_DATALOG_CONTEXTS_RULE_HPP_
#define TYR_DATALOG_CONTEXTS_RULE_HPP_

#include "tyr/datalog/assignment_sets.hpp"
#include "tyr/datalog/declarations.hpp"
#include "tyr/datalog/fact_sets.hpp"
#include "tyr/datalog/policies/annotation_concept.hpp"
#include "tyr/datalog/policies/termination_concept.hpp"
#include "tyr/datalog/workspaces/rule.hpp"
#include "tyr/formalism/datalog/rule_index.hpp"

#include <tbb/global_control.h>
#include <tbb/info.h>

namespace tyr::datalog
{
template<OrAnnotationPolicyConcept OrAP, AndAnnotationPolicyConcept AndAP, TerminationPolicyConcept TP>
struct StratumExecutionContext;

template<OrAnnotationPolicyConcept OrAP, AndAnnotationPolicyConcept AndAP, TerminationPolicyConcept TP>
struct RuleExecutionContext;

template<OrAnnotationPolicyConcept OrAP, AndAnnotationPolicyConcept AndAP, TerminationPolicyConcept TP>
class RuleWorkerExecutionContext
{
public:
    class In
    {
    public:
        explicit In(const RuleExecutionContext<OrAP, AndAP, TP>& rctx, const RuleWorkspace<AndAP>::Worker& ws_worker) :
            m_rctx(rctx),
            m_and_ap(ws_worker.solve.and_ap),
            m_ws_rule(rctx.out().ws_rule()),
            m_cws_rule(rctx.in().cws_rule()),
            m_fact_sets(rctx.stratum_in().program().facts().fact_sets, rctx.stratum_out().program().facts().fact_sets)
        {
        }

        const auto& ws_rule() noexcept { return m_ws_rule; }
        const auto& ws_rule() const noexcept { return m_ws_rule; }
        const auto& cws_rule() noexcept { return m_cws_rule; }
        const auto& cws_rule() const noexcept { return m_cws_rule; }

        const auto& and_ap() noexcept { return m_and_ap; }
        const auto& and_ap() const noexcept { return m_and_ap; }
        const auto& or_annot() noexcept { return m_rctx.stratum_out().program().or_annot(); }
        const auto& or_annot() const noexcept { return m_rctx.stratum_out().program().or_annot(); }
        const auto& cost_buckets() noexcept { return m_rctx.stratum_out().program().cost_buckets(); }
        const auto& cost_buckets() const noexcept { return m_rctx.stratum_out().program().cost_buckets(); }
        const auto& program_repository() noexcept { return m_rctx.out().common().program_repository; }
        const auto& program_repository() const noexcept { return m_rctx.out().common().program_repository; }
        const auto& fact_sets() noexcept { return m_fact_sets; }
        const auto& fact_sets() const noexcept { return m_fact_sets; }

    private:
        const RuleExecutionContext<OrAP, AndAP, TP>& m_rctx;

        const AndAP& m_and_ap;
        const RuleWorkspace<AndAP>& m_ws_rule;
        const ConstRuleWorkspace& m_cws_rule;

        const FactSets m_fact_sets;
    };

    class Out
    {
    public:
        Out(RuleExecutionContext<OrAP, AndAP, TP>& rctx, RuleWorkspace<AndAP>::Worker& ws_worker) :
            m_ws_worker(ws_worker),
            m_ground_context_solve(ws_worker.builder, ws_worker.solve.program_overlay_repository, ws_worker.binding),
            m_ground_context_iteration(ws_worker.builder, ws_worker.iteration.workspace_overlay_repository, ws_worker.binding)
        {
        }

        auto& kpkc_workspace() noexcept { return m_ws_worker.iteration.kpkc_workspace; }
        auto& and_annot() noexcept { return m_ws_worker.iteration.and_annot; }
        auto& head() noexcept { return m_ws_worker.iteration.head; }

        auto& applicability_check_pool() noexcept { return m_ws_worker.solve.applicability_check_pool; }
        auto& seen_bindings_dbg() noexcept { return m_ws_worker.solve.seen_bindings_dbg; }
        auto& pending_rules() noexcept { return m_ws_worker.solve.pending_rules; }
        auto& statistics() noexcept { return m_ws_worker.solve.statistics; }

        auto& ground_context_solve() noexcept { return m_ground_context_solve; }
        auto& ground_context_iteration() noexcept { return m_ground_context_iteration; }

    private:
        RuleWorkspace<AndAP>::Worker& m_ws_worker;

        formalism::datalog::GrounderContext m_ground_context_solve;
        formalism::datalog::GrounderContext m_ground_context_iteration;
    };

    RuleWorkerExecutionContext(RuleExecutionContext<OrAP, AndAP, TP>& rctx, RuleWorkspace<AndAP>::Worker& ws_worker) :
        m_rctx(rctx),
        m_ws_worker(ws_worker),
        m_in(rctx, ws_worker),
        m_out(rctx, ws_worker)
    {
    }

    /**
     * Initialization
     */

    void clear_iteration() noexcept { m_ws_worker.iteration.clear(); }
    void clear_solve() noexcept { m_ws_worker.solve.clear(); }
    void clear() noexcept
    {
        clear_iteration();
        clear_solve();
    }

    /**
     * Getters
     */

    auto& in() noexcept { return m_in; }
    const auto& in() const noexcept { return m_in; }

    auto& out() noexcept { return m_out; }
    const auto& out() const noexcept { return m_out; }

private:
    RuleExecutionContext<OrAP, AndAP, TP>& m_rctx;
    RuleWorkspace<AndAP>::Worker& m_ws_worker;

    In m_in;
    Out m_out;
};

template<OrAnnotationPolicyConcept OrAP, AndAnnotationPolicyConcept AndAP, TerminationPolicyConcept TP>
struct RuleExecutionContext
{
    class In
    {
    public:
        In(Index<formalism::datalog::Rule> rule, const ConstRuleWorkspace& cws_rule) : m_rule(rule), m_cws_rule(cws_rule) {}

        auto rule() const noexcept { return m_rule; }
        const auto& cws_rule() const noexcept { return m_cws_rule; }

    private:
        Index<formalism::datalog::Rule> m_rule;
        const ConstRuleWorkspace& m_cws_rule;
    };

    class Out
    {
    public:
        explicit Out(RuleWorkspace<AndAP>& ws_rule) : m_ws_rule(ws_rule) {}

        auto& ws_rule() noexcept { return m_ws_rule; }
        const auto& ws_rule() const noexcept { return m_ws_rule; }
        auto& common() noexcept { return m_ws_rule.common; }
        const auto& common() const noexcept { return m_ws_rule.common; }
        auto& kpkc() noexcept { return m_ws_rule.common.kpkc; }
        const auto& kpkc() const noexcept { return m_ws_rule.common.kpkc; }
        auto& statistics() noexcept { return m_ws_rule.common.statistics; }
        const auto& statistics() const noexcept { return m_ws_rule.common.statistics; }
        auto& workers() noexcept { return m_ws_rule.worker; }
        const auto& workers() const noexcept { return m_ws_rule.worker; }

    private:
        RuleWorkspace<AndAP>& m_ws_rule;
    };

    RuleExecutionContext(Index<formalism::datalog::Rule> rule, StratumExecutionContext<OrAP, AndAP, TP>& ctx) :
        m_ctx(ctx),
        m_in(rule, *ctx.in().program().rules()[uint_t(rule)]),
        m_out(*ctx.out().program().rules()[uint_t(rule)])
    {
    }

    /**
     * Initialization
     */

    void initialize()
    {
        // std::cout << cws_rule.get_rule() << std::endl;

        out().common().initialize_iteration(in().cws_rule().get_static_consistency_graph(),
                                            AssignmentSets { stratum_in().program().facts().assignment_sets, stratum_out().program().facts().assignment_sets });
    }

    void clear_common() noexcept { out().common().clear(); }
    void clear_worker() noexcept
    {
        for (auto& worker : out().workers())
            worker.clear();
    }
    void clear_iteration() noexcept
    {
        for (auto& worker : out().workers())
            worker.iteration.clear();
    }
    void clear_solve() noexcept
    {
        for (auto& worker : out().workers())
            worker.solve.clear();
    }
    void clear() noexcept
    {
        clear_common();
        clear_worker();
    }

    /**
     * Subcontext
     */

    auto get_rule_worker_execution_context() { return RuleWorkerExecutionContext<OrAP, AndAP, TP>(*this, out().workers().local()); }

    const auto& in() const noexcept { return m_in; }
    auto& out() noexcept { return m_out; }
    const auto& out() const noexcept { return m_out; }

    const auto& stratum_in() const noexcept { return m_ctx.in(); }
    auto& stratum_out() noexcept { return m_ctx.out(); }
    const auto& stratum_out() const noexcept { return m_ctx.out(); }

private:
    StratumExecutionContext<OrAP, AndAP, TP>& m_ctx;

    In m_in;
    Out m_out;
};
}

#endif
