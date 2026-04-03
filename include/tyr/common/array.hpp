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

#ifndef TYR_COMMON_ARRAY_HPP_
#define TYR_COMMON_ARRAY_HPP_

#include "tyr/common/bit_packed_array_pool.hpp"
#include "tyr/common/block_array_pool.hpp"
#include "tyr/common/declarations.hpp"
#include "tyr/common/types.hpp"
#include "tyr/common/types_utils.hpp"

#include <cstddef>
#include <iterator>

namespace tyr
{

template<std::unsigned_integral Block, typename Coder, typename C>
class View<BasicBitPackedArrayView<Block, Coder>, C>
{
public:
    using Container = BasicBitPackedArrayView<Block, Coder>;
    using T = typename Coder::value_type;

    View(const Container& handle, const C& context) noexcept : m_context(&context), m_handle(handle) {}

    size_t size() const noexcept { return get_data().size(); }
    bool empty() const noexcept { return get_data().empty(); }

    decltype(auto) operator[](size_t i) const noexcept
    {
        if constexpr (ViewConcept<T, C>)
            return make_view(get_data()[i], get_context());
        else
            return get_data()[i];
    }

    decltype(auto) front() const noexcept
    {
        if constexpr (ViewConcept<T, C>)
            return make_view(get_data().front(), get_context());
        else
            return get_data().front();
    }

    struct const_iterator
    {
        using It = typename Container::const_iterator;

        const C* ctx;
        It it;

        using difference_type = std::ptrdiff_t;
        using value_type = std::conditional_t<ViewConcept<T, C>, ::tyr::View<T, C>, T>;
        using iterator_category = std::random_access_iterator_tag;
        using iterator_concept = std::random_access_iterator_tag;

        const_iterator() noexcept : ctx(nullptr), it() {}
        const_iterator(It it, const C& ctx) noexcept : ctx(&ctx), it(it) {}

        decltype(auto) operator*() const noexcept
        {
            if constexpr (ViewConcept<T, C>)
                return make_view(*it, *ctx);
            else
                return *it;
        }

        // ++
        const_iterator& operator++() noexcept
        {
            ++it;
            return *this;
        }
        const_iterator operator++(int) noexcept
        {
            auto tmp = *this;
            ++(*this);
            return tmp;
        }

        // --
        const_iterator& operator--() noexcept
        {
            --it;
            return *this;
        }
        const_iterator operator--(int) noexcept
        {
            auto tmp = *this;
            --(*this);
            return tmp;
        }

        // += / -=
        const_iterator& operator+=(difference_type n) noexcept
        {
            it += n;
            return *this;
        }
        const_iterator& operator-=(difference_type n) noexcept
        {
            it -= n;
            return *this;
        }

        // + / -
        friend const_iterator operator+(const_iterator it, difference_type n) noexcept
        {
            it += n;
            return it;
        }

        friend const_iterator operator+(difference_type n, const_iterator it) noexcept
        {
            it += n;
            return it;
        }

        friend const_iterator operator-(const_iterator it, difference_type n) noexcept
        {
            it -= n;
            return it;
        }

        // iterator - iterator
        friend difference_type operator-(const_iterator lhs, const_iterator rhs) noexcept { return lhs.it - rhs.it; }

        // []
        auto operator[](difference_type n) const noexcept
        {
            if constexpr (ViewConcept<T, C>)
                return make_view(*(it + n), *ctx);
            else
                return *(it + n);
        }

        // comparisons
        friend bool operator==(const const_iterator& lhs, const const_iterator& rhs) noexcept { return lhs.it == rhs.it; }

        friend bool operator!=(const const_iterator& lhs, const const_iterator& rhs) noexcept { return !(lhs == rhs); }

        friend bool operator<(const const_iterator& lhs, const const_iterator& rhs) noexcept { return lhs.it < rhs.it; }

        friend bool operator>(const const_iterator& lhs, const const_iterator& rhs) noexcept { return rhs < lhs; }

        friend bool operator<=(const const_iterator& lhs, const const_iterator& rhs) noexcept { return !(rhs < lhs); }

        friend bool operator>=(const const_iterator& lhs, const const_iterator& rhs) noexcept { return !(lhs < rhs); }
    };

    const_iterator begin() const noexcept { return const_iterator { get_data().begin(), get_context() }; }

    const_iterator end() const noexcept { return const_iterator { get_data().end(), get_context() }; }

    const auto& get_data() const noexcept { return m_handle; }
    const auto& get_context() const noexcept { return *m_context; }
    const auto& get_handle() const noexcept { return m_handle; }

private:
    const C* m_context;
    Container m_handle;
};

template<std::unsigned_integral Block, typename Coder, typename C>
class View<BasicBlockArrayView<Block, Coder>, C>
{
public:
    using Container = BasicBlockArrayView<Block, Coder>;
    using T = typename Coder::value_type;

    View(const Container& handle, const C& context) noexcept : m_context(&context), m_handle(handle) {}

    size_t size() const noexcept { return get_data().size(); }
    bool empty() const noexcept { return get_data().empty(); }

    decltype(auto) operator[](size_t i) const noexcept
    {
        if constexpr (ViewConcept<T, C>)
            return make_view(get_data()[i], get_context());
        else
            return get_data()[i];
    }

    decltype(auto) front() const noexcept
    {
        if constexpr (ViewConcept<T, C>)
            return make_view(get_data().front(), get_context());
        else
            return get_data().front();
    }

    struct const_iterator
    {
        using It = typename Container::const_iterator;

        const C* ctx;
        It it;

        using difference_type = std::ptrdiff_t;
        using value_type = std::conditional_t<ViewConcept<T, C>, ::tyr::View<T, C>, T>;
        using iterator_category = std::random_access_iterator_tag;
        using iterator_concept = std::random_access_iterator_tag;

        const_iterator() noexcept : ctx(nullptr), it() {}
        const_iterator(It it, const C& ctx) noexcept : ctx(&ctx), it(it) {}

        decltype(auto) operator*() const noexcept
        {
            if constexpr (ViewConcept<T, C>)
                return make_view(*it, *ctx);
            else
                return *it;
        }

        // ++
        const_iterator& operator++() noexcept
        {
            ++it;
            return *this;
        }
        const_iterator operator++(int) noexcept
        {
            auto tmp = *this;
            ++(*this);
            return tmp;
        }

        // --
        const_iterator& operator--() noexcept
        {
            --it;
            return *this;
        }
        const_iterator operator--(int) noexcept
        {
            auto tmp = *this;
            --(*this);
            return tmp;
        }

        // += / -=
        const_iterator& operator+=(difference_type n) noexcept
        {
            it += n;
            return *this;
        }
        const_iterator& operator-=(difference_type n) noexcept
        {
            it -= n;
            return *this;
        }

        // + / -
        friend const_iterator operator+(const_iterator it, difference_type n) noexcept
        {
            it += n;
            return it;
        }

        friend const_iterator operator+(difference_type n, const_iterator it) noexcept
        {
            it += n;
            return it;
        }

        friend const_iterator operator-(const_iterator it, difference_type n) noexcept
        {
            it -= n;
            return it;
        }

        // iterator - iterator
        friend difference_type operator-(const_iterator lhs, const_iterator rhs) noexcept { return lhs.it - rhs.it; }

        // []
        auto operator[](difference_type n) const noexcept
        {
            if constexpr (ViewConcept<T, C>)
                return make_view(*(it + n), *ctx);
            else
                return *(it + n);
        }

        // comparisons
        friend bool operator==(const const_iterator& lhs, const const_iterator& rhs) noexcept { return lhs.it == rhs.it; }

        friend bool operator!=(const const_iterator& lhs, const const_iterator& rhs) noexcept { return !(lhs == rhs); }

        friend bool operator<(const const_iterator& lhs, const const_iterator& rhs) noexcept { return lhs.it < rhs.it; }

        friend bool operator>(const const_iterator& lhs, const const_iterator& rhs) noexcept { return rhs < lhs; }

        friend bool operator<=(const const_iterator& lhs, const const_iterator& rhs) noexcept { return !(rhs < lhs); }

        friend bool operator>=(const const_iterator& lhs, const const_iterator& rhs) noexcept { return !(lhs < rhs); }
    };

    const_iterator begin() const noexcept { return const_iterator { get_data().begin(), get_context() }; }

    const_iterator end() const noexcept { return const_iterator { get_data().end(), get_context() }; }

    const auto& get_data() const noexcept { return m_handle; }
    const auto& get_context() const noexcept { return *m_context; }
    const auto& get_handle() const noexcept { return m_handle; }

private:
    const C* m_context;
    Container m_handle;
};

}

#endif