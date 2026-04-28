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

#ifndef TYR_COMMON_FORMATTER_HPP_
#define TYR_COMMON_FORMATTER_HPP_

#include "tyr/common/declarations.hpp"
#include "tyr/common/dynamic_bitset.hpp"
#include "tyr/common/index_mixins.hpp"
#include "tyr/common/types.hpp"
#include "tyr/common/uint_mixins.hpp"
#include "tyr/common/vector.hpp"

#include <array>
#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <fmt/ranges.h>
#include <map>
#include <memory>
#include <optional>
#include <ostream>
#include <set>
#include <span>
#include <sstream>
#include <string_view>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

namespace tyr
{

template<typename T>
std::string to_string(const T& element)
{
    return fmt::format("{}", element);
}

}  // namespace tyr

namespace fmt
{

template<typename T, template<typename> typename Ptr, bool IndexPointers, typename TemplateSizeType, class Allocator, typename Char>
struct range_format_kind<::cista::basic_vector<T, Ptr, IndexPointers, TemplateSizeType, Allocator>, Char, void> : std::false_type
{
};

template<typename Ptr, typename Char>
struct range_format_kind<::cista::basic_string<Ptr>, Char, void> : std::false_type
{
};

template<typename C, typename T, template<typename> typename Ptr, bool IndexPointers, typename TemplateSizeType, class Allocator, typename Char>
struct range_format_kind<tyr::View<::cista::basic_vector<T, Ptr, IndexPointers, TemplateSizeType, Allocator>, C>, Char, void> : std::false_type
{
};

template<typename K, typename V, typename Allocator, typename Char>
struct range_format_kind<gtl::flat_hash_map<K, V, tyr::Hash<K>, tyr::EqualTo<K>, Allocator>, Char, void> : std::false_type
{
};

template<typename T>
struct formatter<tyr::Index<T>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template<typename FormatContext>
    auto format(const tyr::Index<T>& value, FormatContext& ctx) const
    {
        return fmt::format_to(ctx.out(), "{}", tyr::uint_t(value));
    }
};

template<typename T>
struct formatter<std::optional<T>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template<typename FormatContext>
    auto format(const std::optional<T>& value, FormatContext& ctx) const
    {
        if (value.has_value())
            return fmt::format_to(ctx.out(), "{}", value.value());
        return fmt::format_to(ctx.out(), "<nullopt>");
    }
};

template<typename T>
struct formatter<std::shared_ptr<T>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template<typename FormatContext>
    auto format(const std::shared_ptr<T>& value, FormatContext& ctx) const
    {
        if (value)
            return fmt::format_to(ctx.out(), "{}", *value);
        return fmt::format_to(ctx.out(), "<nullptr>");
    }
};

template<typename T, typename Deleter>
struct formatter<std::unique_ptr<T, Deleter>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template<typename FormatContext>
    auto format(const std::unique_ptr<T, Deleter>& value, FormatContext& ctx) const
    {
        if (value)
            return fmt::format_to(ctx.out(), "{}", *value);
        return fmt::format_to(ctx.out(), "<nullptr>");
    }
};

template<typename K, typename V, typename Allocator>
struct formatter<gtl::flat_hash_map<K, V, tyr::Hash<K>, tyr::EqualTo<K>, Allocator>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template<typename FormatContext>
    auto format(const gtl::flat_hash_map<K, V, tyr::Hash<K>, tyr::EqualTo<K>, Allocator>& value, FormatContext& ctx) const
    {
        auto out = fmt::format_to(ctx.out(), "{{");
        auto first = true;
        for (const auto& [key, mapped] : value)
        {
            if (!first)
                out = fmt::format_to(out, ", ");
            first = false;
            out = fmt::format_to(out, "{}: {}", key, mapped);
        }
        return fmt::format_to(out, "}}");
    }
};

template<typename T>
struct formatter<::cista::optional<T>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template<typename FormatContext>
    auto format(const ::cista::optional<T>& value, FormatContext& ctx) const
    {
        if (value.has_value())
            return fmt::format_to(ctx.out(), "{}", value.value());
        return fmt::format_to(ctx.out(), "<nullopt>");
    }
};

template<typename... Ts>
    requires(sizeof...(Ts) > 0)
struct formatter<::cista::offset::variant<Ts...>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template<typename FormatContext>
    auto format(const ::cista::offset::variant<Ts...>& value, FormatContext& ctx) const
    {
        return std::visit(
            [&](auto&& arg)
            {
                return fmt::format_to(ctx.out(), "{}", arg);
            },
            value);
    }
};

template<typename T, template<typename> typename Ptr, bool IndexPointers, typename TemplateSizeType, class Allocator>
struct formatter<::cista::basic_vector<T, Ptr, IndexPointers, TemplateSizeType, Allocator>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template<typename FormatContext>
    auto format(const ::cista::basic_vector<T, Ptr, IndexPointers, TemplateSizeType, Allocator>& value, FormatContext& ctx) const
    {
        auto out = fmt::format_to(ctx.out(), "[");
        bool first = true;
        for (const auto& element : value)
        {
            if (!first)
                out = fmt::format_to(out, ", ");
            first = false;
            out = fmt::format_to(out, "{}", element);
        }
        return fmt::format_to(out, "]");
    }
};

template<typename C, typename T, template<typename> typename Ptr, bool IndexPointers, typename TemplateSizeType, class Allocator>
struct formatter<tyr::View<::cista::basic_vector<T, Ptr, IndexPointers, TemplateSizeType, Allocator>, C>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template<typename FormatContext>
    auto format(const tyr::View<::cista::basic_vector<T, Ptr, IndexPointers, TemplateSizeType, Allocator>, C>& value, FormatContext& ctx) const
    {
        auto out = fmt::format_to(ctx.out(), "[");
        bool first = true;
        for (const auto& element : value)
        {
            if (!first)
                out = fmt::format_to(out, ", ");
            first = false;
            out = fmt::format_to(out, "{}", element);
        }
        return fmt::format_to(out, "]");
    }
};

template<typename C, typename T>
struct formatter<tyr::View<::cista::optional<T>, C>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template<typename FormatContext>
    auto format(const tyr::View<::cista::optional<T>, C>& value, FormatContext& ctx) const
    {
        if (value.has_value())
            return fmt::format_to(ctx.out(), "{}", value.value());
        return fmt::format_to(ctx.out(), "<nullopt>");
    }
};

template<typename C, typename... Ts>
    requires(sizeof...(Ts) > 0)
struct formatter<tyr::View<::cista::offset::variant<Ts...>, C>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template<typename FormatContext>
    auto format(const tyr::View<::cista::offset::variant<Ts...>, C>& value, FormatContext& ctx) const
    {
        return visit(
            [&](auto&& arg)
            {
                return fmt::format_to(ctx.out(), "{}", arg);
            },
            value);
    }
};

template<std::unsigned_integral Block>
struct formatter<tyr::BitsetSpan<Block>, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template<typename FormatContext>
    auto format(const tyr::BitsetSpan<Block>& value, FormatContext& ctx) const
    {
        auto out = fmt::format_to(ctx.out(), "{{");
        size_t pos = value.find_first();
        bool first = true;
        while (pos != tyr::BitsetSpan<Block>::npos)
        {
            if (!first)
                out = fmt::format_to(out, ", ");
            first = false;
            out = fmt::format_to(out, "{}", pos);
            pos = value.find_next(pos);
        }
        return fmt::format_to(out, "}}");
    }
};

template<>
struct formatter<std::monostate, char>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template<typename FormatContext>
    auto format(const std::monostate&, FormatContext& ctx) const
    {
        return fmt::format_to(ctx.out(), "monostate");
    }
};

}  // namespace fmt

namespace tyr
{

template<std::ranges::input_range Range>
std::vector<std::string> to_strings(const Range& range)
{
    auto result = std::vector<std::string> {};
    if constexpr (std::ranges::sized_range<Range>)
        result.reserve(std::ranges::size(range));
    for (const auto& element : range)
        result.push_back(to_string(element));
    return result;
}

}

#endif
