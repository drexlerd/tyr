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

#include "tyr/datalog/rule_scheduler.hpp"

#include "tyr/common/config.hpp"  // for uint_t
#include "tyr/formalism/datalog/formatter.hpp"
#include "tyr/formalism/datalog/views.hpp"  // for View

#include <assert.h>       // for assert
#include <gtl/phmap.hpp>  // for operator!=, flat_hash_set
#include <type_traits>
#include <utility>  // for pair

namespace f = tyr::formalism;
namespace fd = tyr::formalism::datalog;

namespace tyr::datalog
{
namespace
{
template<typename Relation>
void activate_relation(boost::dynamic_bitset<>& bitset, Index<Relation> relation)
{
    assert(uint_t(relation) < bitset.size());
    bitset.set(uint_t(relation));
}

template<typename Relation>
void collect_active_rules(const boost::dynamic_bitset<>& bitset,
                          const UnorderedMap<Index<Relation>, UnorderedSet<Index<fd::Rule>>>& listeners,
                          UnorderedSet<Index<fd::Rule>>& active_rules)
{
    for (auto i = bitset.find_first(); i != boost::dynamic_bitset<>::npos; i = bitset.find_next(i))
        if (const auto it = listeners.find(Index<Relation>(i)); it != listeners.end())
            for (const auto rule : it->second)
                active_rules.insert(rule);
}
}

RuleSchedulerStratum::RuleSchedulerStratum(const analysis::RuleStratum& rules,
                                           const analysis::ListenerStratum& listeners,
                                           const fd::Repository& context,
                                           size_t num_fluent_predicates,
                                           size_t num_fluent_functions) :
    m_rules(rules),
    m_listeners(listeners),
    m_context(context),
    m_active_predicates(num_fluent_predicates),
    m_active_functions(num_fluent_functions),
    m_active_rules()
{
}

void RuleSchedulerStratum::activate_all()
{
    m_active_rules.clear();
    for (const auto rule : m_rules)
        m_active_rules.insert(rule);
}

void RuleSchedulerStratum::on_start_iteration() noexcept
{
    m_active_predicates.reset();
    m_active_functions.reset();
}

void RuleSchedulerStratum::on_generate(Index<f::Predicate<f::FluentTag>> predicate) { activate_relation(m_active_predicates, predicate); }

void RuleSchedulerStratum::on_generate(Index<f::Function<f::FluentTag>> function) { activate_relation(m_active_functions, function); }

void RuleSchedulerStratum::on_finish_iteration()
{
    m_active_rules.clear();
    collect_active_rules(m_active_predicates, m_listeners.predicates, m_active_rules);
    collect_active_rules(m_active_functions, m_listeners.functions, m_active_rules);
}

RuleSchedulerStrata create_schedulers(const analysis::RuleStrata& rules,
                                      const analysis::ListenerStrata& listeners,
                                      const fd::Repository& context,
                                      size_t num_fluent_predicates,
                                      size_t num_fluent_functions)
{
    assert(rules.data.size() == listeners.data.size());

    auto result = RuleSchedulerStrata {};
    for (uint_t i = 0; i < rules.data.size(); ++i)
        result.data.emplace_back(rules.data[i], listeners.data[i], context, num_fluent_predicates, num_fluent_functions);

    return result;
}

}
