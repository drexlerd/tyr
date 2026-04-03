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

#ifndef TYR_DATALOG_RULE_SCHEDULER_HPP_
#define TYR_DATALOG_RULE_SCHEDULER_HPP_

#include "tyr/analysis/listeners.hpp"              // for ListenerStratum
#include "tyr/analysis/stratification.hpp"         // for RuleStratum, Rule...
#include "tyr/common/declarations.hpp"             // for UnorderedSet
#include "tyr/common/equal_to.hpp"                 // for EqualTo
#include "tyr/common/formatter.hpp"                // for operator<<
#include "tyr/common/hash.hpp"                     // for Hash
#include "tyr/common/types.hpp"                    // for IndexList
#include "tyr/common/vector.hpp"                   // for View
#include "tyr/formalism/datalog/declarations.hpp"  // for FluentTag (ptr only)
#include "tyr/formalism/datalog/repository.hpp"    // for Repository
#include "tyr/formalism/datalog/rule_index.hpp"    // for Index

#include <boost/dynamic_bitset/dynamic_bitset.hpp>  // for dynamic_bitset
#include <vector>                                   // for vector

namespace tyr::datalog
{

class RuleSchedulerStratum
{
public:
    RuleSchedulerStratum(const analysis::RuleStratum& rules, const analysis::ListenerStratum& listeners, const formalism::datalog::Repository& context);

    void activate_all();

    void on_start_iteration() noexcept;

    void on_generate(Index<formalism::Predicate<formalism::FluentTag>> predicate);

    void on_finish_iteration();

    const formalism::datalog::Repository& get_context() const noexcept { return m_context; }
    const IndexList<formalism::datalog::Rule>& get_rules() const noexcept { return m_rules; }
    const UnorderedSet<Index<formalism::datalog::Rule>>& get_active_rules() const noexcept { return m_active_rules; }

private:
    const analysis::RuleStratum& m_rules;
    const analysis::ListenerStratum& m_listeners;
    const formalism::datalog::Repository& m_context;

    boost::dynamic_bitset<> m_active_predicates;
    UnorderedSet<Index<formalism::datalog::Rule>> m_active_rules;
};

struct RuleSchedulerStrata
{
    std::vector<RuleSchedulerStratum> data;
};

extern RuleSchedulerStrata
create_schedulers(const analysis::RuleStrata& rules, const analysis::ListenerStrata& listeners, const formalism::datalog::Repository& context);

}

#endif