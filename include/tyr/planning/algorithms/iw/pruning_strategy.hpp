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

#ifndef TYR_PLANNING_ALGORITHMS_IW_PRUNING_STRATEGY_HPP_
#define TYR_PLANNING_ALGORITHMS_IW_PRUNING_STRATEGY_HPP_

#include "tyr/planning/algorithms/iw/novelty_table.hpp"
#include "tyr/planning/algorithms/iw/utils.hpp"
#include "tyr/planning/algorithms/strategies/pruning.hpp"

#include <memory>
#include <optional>
#include <stdexcept>

namespace tyr::planning::iw
{

template<TaskKind Kind>
class NoveltyPruningStrategy : public PruningStrategy<Kind>
{
public:
    explicit NoveltyPruningStrategy(uint_t max_arity) : m_max_arity(max_arity)
    {
        if (max_arity > MaxArity)
            throw std::invalid_argument("NoveltyPruningStrategy(...): max_arity exceeds iw::MaxArity.");
    }

    static std::shared_ptr<NoveltyPruningStrategy<Kind>> create(uint_t max_arity) { return std::make_shared<NoveltyPruningStrategy<Kind>>(max_arity); }

    uint_t get_max_arity() const noexcept { return m_max_arity; }

    const DynamicNoveltyTable<MaxArity>& get_novelty_table() const noexcept { return m_novelty_table; }

    DynamicNoveltyTable<MaxArity>& get_novelty_table() noexcept { return m_novelty_table; }

    bool should_prune_state(const StateView<Kind>& state) override
    {
        if (m_max_arity == 0)
        {
            if (!m_root_state)
                m_root_state = state.get_index();

            return state.get_index() != *m_root_state;
        }

        return !m_novelty_table.insert(state, m_max_arity);
    }

    bool should_prune_successor_state(const StateView<Kind>& state, const StateView<Kind>& succ_state, bool is_new_succ) override
    {
        if (m_max_arity == 0)
        {
            static_cast<void>(succ_state);
            if (!is_new_succ)
                return false;

            return !m_root_state || state.get_index() != *m_root_state;
        }

        return is_new_succ && !m_novelty_table.insert(state, succ_state, m_max_arity);
    }

private:
    uint_t m_max_arity;
    std::optional<Index<State<Kind>>> m_root_state;
    DynamicNoveltyTable<MaxArity> m_novelty_table;
};

}

#endif
