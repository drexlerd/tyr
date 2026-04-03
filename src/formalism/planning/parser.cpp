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

#include "tyr/formalism/planning/parser.hpp"

#include "loki_to_tyr.hpp"

namespace tyr::formalism::planning
{

Parser::Parser(const fs::path& domain_filepath, const loki::ParserOptions& options) : Parser(loki::read_file(domain_filepath), domain_filepath, options) {}

Parser::Parser(const std::string& domain_description, const fs::path& domain_filepath, const loki::ParserOptions& options) :
    m_loki_parser(loki::Parser(domain_description, domain_filepath, options)),
    m_loki_domain_translation_result(loki::translate(m_loki_parser.get_domain())),
    m_domain(LokiToTyrTranslator().translate(m_loki_domain_translation_result.get_translated_domain()))
{
}

PlanningTask Parser::parse_task(const fs::path& task_filepath, const loki::ParserOptions& options)
{
    return parse_task(loki::read_file(task_filepath), task_filepath, options);
}

PlanningTask Parser::parse_task(const std::string& task_description, const fs::path& task_filepath, const loki::ParserOptions& options)
{
    return LokiToTyrTranslator().translate(
        loki::translate(m_loki_parser.parse_problem(task_description, task_filepath, options), m_loki_domain_translation_result),
        m_domain);
}

PlanningDomain Parser::get_domain() const { return m_domain; }

}
