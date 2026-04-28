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

#ifndef TYR_DATALOG_FORMATTER_HPP_
#define TYR_DATALOG_FORMATTER_HPP_

#include "tyr/common/formatter.hpp"
#include "tyr/common/iostream.hpp"
#include "tyr/datalog/declarations.hpp"
#include "tyr/formalism/declarations.hpp"  // for Context

#include <fmt/ostream.h>
#include <iosfwd>  // for ostream
#include <sstream>

namespace fmt
{

#define TYR_DECLARE_DATALOG_FORMATTER(Type)                                          \
    template<>                                                                       \
    struct formatter<Type, char>                                                     \
    {                                                                                \
        constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }      \
        auto format(const Type& value, format_context& ctx) const -> format_context::iterator; \
    }

TYR_DECLARE_DATALOG_FORMATTER(tyr::datalog::VertexAssignment);
TYR_DECLARE_DATALOG_FORMATTER(tyr::datalog::EdgeAssignment);
TYR_DECLARE_DATALOG_FORMATTER(tyr::datalog::details::Vertex);
TYR_DECLARE_DATALOG_FORMATTER(tyr::datalog::details::Edge);
TYR_DECLARE_DATALOG_FORMATTER(tyr::datalog::details::RuleToLiteralInfoMappings);
TYR_DECLARE_DATALOG_FORMATTER(tyr::datalog::details::RuleToLiteralPositionMappings);

TYR_DECLARE_DATALOG_FORMATTER(tyr::datalog::details::RuleToLiteralInfo<tyr::formalism::StaticTag>);
TYR_DECLARE_DATALOG_FORMATTER(tyr::datalog::details::RuleToLiteralInfo<tyr::formalism::FluentTag>);
TYR_DECLARE_DATALOG_FORMATTER(tyr::datalog::details::TaggedRuleToLiteralInfos<tyr::formalism::StaticTag>);
TYR_DECLARE_DATALOG_FORMATTER(tyr::datalog::details::TaggedRuleToLiteralInfos<tyr::formalism::FluentTag>);

TYR_DECLARE_DATALOG_FORMATTER(tyr::datalog::details::RuleToLiteralInfos);
TYR_DECLARE_DATALOG_FORMATTER(tyr::datalog::StaticConsistencyGraph);
TYR_DECLARE_DATALOG_FORMATTER(tyr::datalog::kpkc::Vertex);
TYR_DECLARE_DATALOG_FORMATTER(tyr::datalog::kpkc::Edge);
TYR_DECLARE_DATALOG_FORMATTER(tyr::datalog::kpkc::VertexPartitions);
TYR_DECLARE_DATALOG_FORMATTER(tyr::datalog::kpkc::DeduplicatedAdjacencyMatrix);
TYR_DECLARE_DATALOG_FORMATTER(tyr::datalog::kpkc::PartitionedAdjacencyMatrix);
TYR_DECLARE_DATALOG_FORMATTER(tyr::datalog::ProgramStatistics);
TYR_DECLARE_DATALOG_FORMATTER(tyr::datalog::RuleStatistics);
TYR_DECLARE_DATALOG_FORMATTER(tyr::datalog::AggregatedRuleStatistics);
TYR_DECLARE_DATALOG_FORMATTER(tyr::datalog::RuleWorkerStatistics);
TYR_DECLARE_DATALOG_FORMATTER(tyr::datalog::AggregatedRuleWorkerStatistics);

#undef TYR_DECLARE_DATALOG_FORMATTER


}  // namespace fmt

#endif
