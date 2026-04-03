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

#ifndef TYR_DATALOG_PROGRAM_CONTEXT_HPP_
#define TYR_DATALOG_PROGRAM_CONTEXT_HPP_

#include "tyr/analysis/domains.hpp"
#include "tyr/analysis/listeners.hpp"
#include "tyr/analysis/stratification.hpp"
#include "tyr/formalism/datalog/program_index.hpp"
#include "tyr/formalism/datalog/repository.hpp"

namespace tyr::datalog
{
/// @brief `ProgramContext` stores the program and all derived necessities together.
class ProgramContext
{
public:
    ProgramContext(formalism::datalog::ProgramView program,
                   formalism::datalog::RepositoryPtr program_repository,
                   formalism::datalog::RepositoryFactoryPtr repository_factory,
                   analysis::ProgramVariableDomains domains,
                   analysis::RuleStrata strata,
                   analysis::ListenerStrata listeners) :
        m_program(program),
        m_program_repository(std::move(program_repository)),
        m_repository_factory(std::move(repository_factory)),
        m_workspace_repository(m_repository_factory->create_shared(m_program_repository.get())),
        m_domains(std::move(domains)),
        m_strata(std::move(strata)),
        m_listeners(std::move(listeners))
    {
    }
    auto get_program() const noexcept { return m_program; }
    const auto& get_program_repository() const noexcept { return *m_program_repository; }
    auto& get_repository_factory() noexcept { return *m_repository_factory; }
    auto& get_workspace_repository() noexcept { return *m_workspace_repository; }
    const auto& get_workspace_repository() const noexcept { return *m_workspace_repository; }
    const auto& get_domains() const noexcept { return m_domains; }
    const auto& get_strata() const noexcept { return m_strata; }
    const auto& get_listeners() const noexcept { return m_listeners; }

private:
    formalism::datalog::ProgramView m_program;
    formalism::datalog::RepositoryPtr m_program_repository;
    formalism::datalog::RepositoryFactoryPtr m_repository_factory;
    formalism::datalog::RepositoryPtr m_workspace_repository;
    analysis::ProgramVariableDomains m_domains;
    analysis::RuleStrata m_strata;
    analysis::ListenerStrata m_listeners;
};
}
#endif
