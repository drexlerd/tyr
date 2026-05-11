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

#ifndef TYR_PLANNING_FACTORY_HPP_
#define TYR_PLANNING_FACTORY_HPP_

#include "tyr/planning/declarations.hpp"
#include "tyr/planning/ground_task/axiom_evaluator.hpp"
#include "tyr/planning/ground_task/state_repository.hpp"
#include "tyr/planning/ground_task/successor_generator.hpp"
#include "tyr/planning/lifted_task/axiom_evaluator.hpp"
#include "tyr/planning/lifted_task/state_repository.hpp"
#include "tyr/planning/lifted_task/successor_generator.hpp"

#include <memory>
#include <utility>

namespace tyr::planning
{

template<TaskKind Kind>
class StateRepositoryFactory
{
public:
    StateRepositoryFactory() : m_next_index(0) {}

    StateRepositoryPtr<Kind> create(TaskPtr<Kind> task, AxiomEvaluatorPtr<Kind> axiom_evaluator)
    {
        return std::make_shared<StateRepository<Kind>>(m_next_index++, std::move(task), std::move(axiom_evaluator));
    }

private:
    uint_t m_next_index;
};

template<TaskKind Kind>
class AxiomEvaluatorFactory
{
public:
    AxiomEvaluatorFactory() : m_next_index(0) {}

    AxiomEvaluatorPtr<Kind> create(TaskPtr<Kind> task, ExecutionContextPtr execution_context)
    {
        return std::make_shared<AxiomEvaluator<Kind>>(m_next_index++, std::move(task), std::move(execution_context));
    }

private:
    uint_t m_next_index;
};

template<TaskKind Kind>
class SuccessorGeneratorFactory
{
public:
    SuccessorGeneratorFactory() : m_next_index(0) {}

    SuccessorGeneratorPtr<Kind> create(TaskPtr<Kind> task, StateRepositoryPtr<Kind> state_repository)
    {
        return std::make_shared<SuccessorGenerator<Kind>>(m_next_index++, std::move(task), std::move(state_repository));
    }

private:
    uint_t m_next_index;
};

}

#endif
