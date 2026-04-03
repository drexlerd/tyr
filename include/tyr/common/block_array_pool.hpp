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

#ifndef TYR_COMMON_BLOCK_ARRAY_POOL_HPP_
#define TYR_COMMON_BLOCK_ARRAY_POOL_HPP_

#include "tyr/common/bit.hpp"

#include <algorithm>
#include <bit>
#include <cassert>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <ranges>
#include <span>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

namespace tyr
{

/**
 * BasicBlockArrayView
 */

template<typename Block, typename Coder>
class BasicBlockArrayView
{
public:
    using block_type = std::remove_const_t<Block>;
    using value_type = typename Coder::value_type;
    using reference_type = typename bit::block_reference<block_type, Coder>;
    using reference = std::conditional_t<std::is_const_v<Block>, value_type, reference_type>;

private:
    void ensure_fits(std::span<const value_type> elements) const
    {
        if (elements.size() != m_length)
            throw std::invalid_argument("BasicBlockArrayView: wrong number of elements.");
    }

public:
    template<typename View>
    class BasicIterator;

    using iterator = BasicIterator<BasicBlockArrayView<Block, Coder>>;
    using const_iterator = BasicIterator<const BasicBlockArrayView<Block, Coder>>;

    BasicBlockArrayView(Block* data, size_t length) : m_data(data), m_length(length) {}

    BasicBlockArrayView& operator=(std::span<const value_type> elements)
        requires(!std::is_const_v<Block>)
    {
        ensure_fits(elements);

        for (size_t i = 0; i < m_length; ++i)
            (*this)[i] = elements[i];

        return *this;
    }

    friend bool operator==(const BasicBlockArrayView& lhs, const BasicBlockArrayView& rhs) { return std::ranges::equal(lhs, rhs); }

    friend bool operator==(const BasicBlockArrayView& lhs, std::span<const value_type> rhs) { return std::ranges::equal(lhs, rhs); }

    friend bool operator==(std::span<const value_type> lhs, const BasicBlockArrayView& rhs) { return rhs == lhs; }

    template<typename View>
    class BasicIterator
    {
    private:
        using view_type = View;
        using view_pointer = View*;

    public:
        using difference_type = std::ptrdiff_t;
        using raw_view_type = std::remove_const_t<View>;
        using value_type = typename raw_view_type::value_type;
        using reference = std::conditional_t<std::is_const_v<View>, value_type, typename raw_view_type::reference_type>;
        using iterator_category = std::bidirectional_iterator_tag;
        using iterator_concept = std::bidirectional_iterator_tag;

        BasicIterator() : m_view(nullptr), m_pos(0) {}
        BasicIterator(view_type& view, size_t pos) : m_view(&view), m_pos(pos) {}

        reference operator*() const { return (*m_view)[m_pos]; }

        BasicIterator& operator++()
        {
            ++m_pos;
            return *this;
        }

        BasicIterator operator++(int)
        {
            auto tmp = *this;
            ++(*this);
            return tmp;
        }

        BasicIterator& operator--()
        {
            --m_pos;
            return *this;
        }

        BasicIterator operator--(int)
        {
            auto tmp = *this;
            --(*this);
            return tmp;
        }

        friend bool operator==(const BasicIterator&, const BasicIterator&) = default;

    private:
        view_pointer m_view;
        size_t m_pos;
    };

    reference_type operator[](size_t pos) noexcept
        requires(!std::is_const_v<Block>)
    {
        assert(pos < m_length);
        return reference_type(m_data + pos);
    }

    value_type operator[](size_t pos) const noexcept
    {
        assert(pos < m_length);
        return Coder::decode(m_data[pos]);
    }

    iterator begin() noexcept
        requires(!std::is_const_v<Block>)
    {
        return iterator(*this, 0);
    }

    iterator end() noexcept
        requires(!std::is_const_v<Block>)
    {
        return iterator(*this, size());
    }

    const_iterator begin() const noexcept { return const_iterator(*this, 0); }
    const_iterator end() const noexcept { return const_iterator(*this, size()); }
    const_iterator cbegin() const noexcept { return const_iterator(*this, 0); }
    const_iterator cend() const noexcept { return const_iterator(*this, size()); }

    size_t size() const noexcept { return m_length; }

private:
    Block* m_data;
    size_t m_length;
};

/// Stores fixed-length arrays where each element occupies one full Block.
/// Values are encoded and decoded via Coder.
template<std::unsigned_integral Block, typename Coder = bit::ForwardingBlockCoder<Block>, size_t FirstSegmentSize = 16>
    requires bit::BlockCoder<Coder, Block>
class BlockArrayPool
{
    static_assert(bit::is_power_of_two(FirstSegmentSize));

public:
    using block_type = std::remove_const_t<Block>;
    using value_type = typename Coder::value_type;
    using ArrayView = BasicBlockArrayView<Block, Coder>;
    using ConstArrayView = BasicBlockArrayView<const Block, Coder>;

private:
    static constexpr size_t seg_shift = std::countr_zero(FirstSegmentSize);
    static constexpr size_t seg_mask = FirstSegmentSize - 1;

    static size_t get_segment_index(size_t index) noexcept { return std::bit_width((index >> seg_shift) + 1) - 1; }

    static size_t get_segment_pos(size_t index) noexcept
    {
        const size_t q = index >> seg_shift;
        const size_t r = index & seg_mask;
        const size_t seg_idx = get_segment_index(index);
        return ((q - ((size_t { 1 } << seg_idx) - 1)) << seg_shift) + r;
    }

    void reserve(size_t size)
    {
        if (size == 0 || size <= m_capacity)
            return;

        const size_t last_segment = get_segment_index(size - 1);
        const size_t first_new_segment = m_segments.size();

        m_segments.resize(last_segment + 1);

        for (size_t seg = first_new_segment; seg <= last_segment; ++seg)
        {
            const size_t arrays_in_segment = FirstSegmentSize << seg;
            assert(bit::is_power_of_two(arrays_in_segment));

            const size_t blocks_in_segment = arrays_in_segment * m_length;
            m_segments[seg].resize(blocks_in_segment, Block { 0 });
            m_capacity += arrays_in_segment;
        }
    }

    void ensure_fits(std::span<const value_type> elements) const
    {
        if (elements.size() != m_length)
            throw std::invalid_argument("BlockArrayPool: wrong number of elements.");
    }

public:
    template<typename Pool>
    class BasicIterator;

    using iterator = BasicIterator<BlockArrayPool>;
    using const_iterator = BasicIterator<const BlockArrayPool>;

    explicit BlockArrayPool(size_t length) : m_length(length), m_capacity(0), m_size(0) {}

    template<typename Pool>
    class BasicIterator
    {
    private:
        using pool_type = Pool;
        using pool_pointer = Pool*;

    public:
        using difference_type = std::ptrdiff_t;
        using value_type = ConstArrayView;
        using reference = std::conditional_t<std::is_const_v<pool_type>, ConstArrayView, ArrayView>;
        using iterator_category = std::bidirectional_iterator_tag;
        using iterator_concept = std::bidirectional_iterator_tag;

        BasicIterator() : m_pool(nullptr), m_pos(0) {}
        BasicIterator(pool_type& pool, size_t pos) : m_pool(&pool), m_pos(pos) {}

        reference operator*() const { return (*m_pool)[m_pos]; }

        BasicIterator& operator++()
        {
            ++m_pos;
            return *this;
        }

        BasicIterator operator++(int)
        {
            auto tmp = *this;
            ++(*this);
            return tmp;
        }

        BasicIterator& operator--()
        {
            --m_pos;
            return *this;
        }

        BasicIterator operator--(int)
        {
            auto tmp = *this;
            --(*this);
            return tmp;
        }

        friend bool operator==(const BasicIterator&, const BasicIterator&) = default;

    private:
        pool_pointer m_pool;
        size_t m_pos;
    };

    ArrayView operator[](size_t index) noexcept
    {
        assert(index < m_size);

        const size_t seg_idx = get_segment_index(index);
        const size_t seg_pos = get_segment_pos(index);
        auto* data = m_segments[seg_idx].data() + seg_pos * m_length;

        return ArrayView(data, m_length);
    }

    ConstArrayView operator[](size_t index) const noexcept
    {
        assert(index < m_size);

        const size_t seg_idx = get_segment_index(index);
        const size_t seg_pos = get_segment_pos(index);
        const auto* data = m_segments[seg_idx].data() + seg_pos * m_length;

        return ConstArrayView(data, m_length);
    }

    iterator begin() noexcept { return iterator(*this, 0); }
    iterator end() noexcept { return iterator(*this, size()); }
    const_iterator begin() const noexcept { return const_iterator(*this, 0); }
    const_iterator end() const noexcept { return const_iterator(*this, size()); }
    const_iterator cbegin() const noexcept { return const_iterator(*this, 0); }
    const_iterator cend() const noexcept { return const_iterator(*this, size()); }

    void push_back(std::span<const value_type> elements)
    {
        ensure_fits(elements);

        const size_t index = m_size;
        reserve(m_size + 1);
        ++m_size;

        auto view = (*this)[index];
        for (size_t i = 0; i < m_length; ++i)
            view[i] = elements[i];
    }

    void clear() noexcept { m_size = 0; }

    size_t length() const noexcept { return m_length; }
    size_t capacity() const noexcept { return m_capacity; }
    size_t size() const noexcept { return m_size; }
    bool empty() const noexcept { return m_size == 0; }
    const auto& segments() const noexcept { return m_segments; }

private:
    std::vector<std::vector<block_type>> m_segments;

    size_t m_length;
    size_t m_capacity;
    size_t m_size;
};

}  // namespace tyr

#endif