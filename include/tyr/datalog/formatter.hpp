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

template<>
struct formatter<tyr::datalog::VertexAssignment, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    auto format(const tyr::datalog::VertexAssignment& value, format_context& ctx) const -> format_context::iterator;
};

template<>
struct formatter<tyr::datalog::EdgeAssignment, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    auto format(const tyr::datalog::EdgeAssignment& value, format_context& ctx) const -> format_context::iterator;
};

template<>
struct formatter<tyr::datalog::details::Vertex, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    auto format(const tyr::datalog::details::Vertex& value, format_context& ctx) const -> format_context::iterator;
};

template<>
struct formatter<tyr::datalog::details::Edge, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    auto format(const tyr::datalog::details::Edge& value, format_context& ctx) const -> format_context::iterator;
};

template<>
struct formatter<tyr::datalog::details::RuleToLiteralInfoMappings, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    auto format(const tyr::datalog::details::RuleToLiteralInfoMappings& value, format_context& ctx) const -> format_context::iterator;
};

template<>
struct formatter<tyr::datalog::details::RuleToLiteralPositionMappings, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    auto format(const tyr::datalog::details::RuleToLiteralPositionMappings& value, format_context& ctx) const -> format_context::iterator;
};

template<>
struct formatter<tyr::datalog::details::RuleToLiteralInfo<tyr::formalism::StaticTag>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    auto format(const tyr::datalog::details::RuleToLiteralInfo<tyr::formalism::StaticTag>& value, format_context& ctx) const -> format_context::iterator;
};

template<>
struct formatter<tyr::datalog::details::RuleToLiteralInfo<tyr::formalism::FluentTag>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    auto format(const tyr::datalog::details::RuleToLiteralInfo<tyr::formalism::FluentTag>& value, format_context& ctx) const -> format_context::iterator;
};

template<>
struct formatter<tyr::datalog::details::TaggedRuleToLiteralInfos<tyr::formalism::StaticTag>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    auto format(const tyr::datalog::details::TaggedRuleToLiteralInfos<tyr::formalism::StaticTag>& value, format_context& ctx) const -> format_context::iterator;
};

template<>
struct formatter<tyr::datalog::details::TaggedRuleToLiteralInfos<tyr::formalism::FluentTag>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    auto format(const tyr::datalog::details::TaggedRuleToLiteralInfos<tyr::formalism::FluentTag>& value, format_context& ctx) const -> format_context::iterator;
};

template<>
struct formatter<tyr::datalog::details::RuleToLiteralInfos, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    auto format(const tyr::datalog::details::RuleToLiteralInfos& value, format_context& ctx) const -> format_context::iterator;
};

template<>
struct formatter<tyr::datalog::StaticConsistencyGraph, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    auto format(const tyr::datalog::StaticConsistencyGraph& value, format_context& ctx) const -> format_context::iterator;
};

template<>
struct formatter<tyr::datalog::kpkc::Vertex, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    auto format(const tyr::datalog::kpkc::Vertex& value, format_context& ctx) const -> format_context::iterator;
};

template<>
struct formatter<tyr::datalog::kpkc::Edge, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    auto format(const tyr::datalog::kpkc::Edge& value, format_context& ctx) const -> format_context::iterator;
};

template<>
struct formatter<tyr::datalog::kpkc::VertexPartitions, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    auto format(const tyr::datalog::kpkc::VertexPartitions& value, format_context& ctx) const -> format_context::iterator;
};

template<>
struct formatter<tyr::datalog::kpkc::DeduplicatedAdjacencyMatrix, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    auto format(const tyr::datalog::kpkc::DeduplicatedAdjacencyMatrix& value, format_context& ctx) const -> format_context::iterator;
};

template<>
struct formatter<tyr::datalog::kpkc::PartitionedAdjacencyMatrix, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    auto format(const tyr::datalog::kpkc::PartitionedAdjacencyMatrix& value, format_context& ctx) const -> format_context::iterator;
};

template<>
struct formatter<tyr::datalog::ProgramStatistics, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    auto format(const tyr::datalog::ProgramStatistics& value, format_context& ctx) const -> format_context::iterator;
};

template<>
struct formatter<tyr::datalog::RuleStatistics, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    auto format(const tyr::datalog::RuleStatistics& value, format_context& ctx) const -> format_context::iterator;
};

template<>
struct formatter<tyr::datalog::AggregatedRuleStatistics, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    auto format(const tyr::datalog::AggregatedRuleStatistics& value, format_context& ctx) const -> format_context::iterator;
};

template<>
struct formatter<tyr::datalog::RuleWorkerStatistics, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    auto format(const tyr::datalog::RuleWorkerStatistics& value, format_context& ctx) const -> format_context::iterator;
};

template<>
struct formatter<tyr::datalog::AggregatedRuleWorkerStatistics, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    auto format(const tyr::datalog::AggregatedRuleWorkerStatistics& value, format_context& ctx) const -> format_context::iterator;
};


}  // namespace fmt

#endif
