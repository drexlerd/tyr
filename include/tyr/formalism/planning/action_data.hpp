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

#ifndef TYR_FORMALISM_PLANNING_ACTION_DATA_HPP_
#define TYR_FORMALISM_PLANNING_ACTION_DATA_HPP_

#include "tyr/common/types.hpp"
#include "tyr/common/types_utils.hpp"
#include "tyr/formalism/planning/action_index.hpp"
#include "tyr/formalism/planning/conditional_effect_index.hpp"
#include "tyr/formalism/planning/conjunctive_condition_index.hpp"
#include "tyr/formalism/planning/declarations.hpp"
#include "tyr/formalism/variable_index.hpp"

namespace tyr
{

template<>
struct Data<formalism::planning::Action>
{
    Index<formalism::planning::Action> index;
    ::cista::offset::string name;
    uint_t original_arity;
    IndexList<formalism::Variable> variables;
    Index<formalism::planning::ConjunctiveCondition> condition;
    IndexList<formalism::planning::ConditionalEffect> effects;

    Data() = default;
    Data(::cista::offset::string name_,
         uint_t original_arity_,
         IndexList<formalism::Variable> variables_,
         Index<formalism::planning::ConjunctiveCondition> condition_,
         IndexList<formalism::planning::ConditionalEffect> effects_) :
        index(),
        name(std::move(name_)),
        original_arity(original_arity_),
        variables(std::move(variables_)),
        condition(condition_),
        effects(std::move(effects_))
    {
    }
    // Python constructor
    template<typename C>
    Data(const std::string& name_,
         uint_t original_arity_,
         const std::vector<View<Index<formalism::Variable>, C>>& variables_,
         View<Index<formalism::planning::ConjunctiveCondition>, C> condition_,
         const std::vector<View<Index<formalism::planning::ConditionalEffect>, C>>& effects_) :
        index(),
        name(name_),
        original_arity(original_arity_),
        variables(),
        condition(),
        effects()
    {
        set(variables_, variables);
        set(condition_, condition);
        set(effects_, effects);
    }
    Data(const Data& other) = delete;
    Data& operator=(const Data& other) = delete;
    Data(Data&& other) = default;
    Data& operator=(Data&& other) = default;

    void clear() noexcept
    {
        tyr::clear(index);
        tyr::clear(name);
        tyr::clear(variables);
        tyr::clear(condition);
        tyr::clear(effects);
    }

    auto cista_members() const noexcept { return std::tie(index, name, variables, original_arity, condition, effects); }
    auto identifying_members() const noexcept { return std::tie(name, variables, original_arity, condition, effects); }
};

static_assert(!uses_trivial_storage_v<formalism::planning::Action>);

namespace formalism::planning
{
using ActionData = Data<Action>;
}
}

#endif
