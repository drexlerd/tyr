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

#ifndef TYR_COMMON_SEGMENTED_VECTOR_HPP_
#define TYR_COMMON_SEGMENTED_VECTOR_HPP_

#include "tyr/common/bit.hpp"

#include <bit>
#include <cassert>
#include <cstddef>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

namespace tyr
{
template<typename T, size_t FirstSegmentSize = 32>
class SegmentedVector
{
    static_assert(bit::is_power_of_two(FirstSegmentSize));
    static_assert(std::is_trivially_copyable_v<T>);
    static_assert(std::is_default_constructible_v<T>);

private:
    static constexpr size_t seg_shift = std::countr_zero(FirstSegmentSize);
    static constexpr size_t seg_mask = FirstSegmentSize - 1;

    static size_t get_segment_index(size_t index) noexcept { return std::bit_width((index >> seg_shift) + size_t { 1 }) - 1; }

    static size_t get_segment_pos(size_t index) noexcept
    {
        const size_t q = index >> seg_shift;
        const size_t r = index & seg_mask;
        const size_t seg_idx = get_segment_index(index);
        return ((q - ((size_t { 1 } << seg_idx) - 1)) << seg_shift) + r;
    }

    void resize_to_fit(size_t n)
    {
        if (m_segments.empty())
        {
            m_segments.emplace_back();
            m_segments.back().resize(FirstSegmentSize);
            m_capacity += FirstSegmentSize;
        }

        while (m_capacity < n)
        {
            const size_t new_segment_size = FirstSegmentSize << m_segments.size();
            m_segments.emplace_back();
            m_segments.back().resize(new_segment_size);
            m_capacity += new_segment_size;
        }
    }

public:
    SegmentedVector() : m_segments(), m_capacity(0), m_size(0) {}

    void clear() noexcept { m_size = 0; }

    void push_back(const T& element)
    {
        resize_to_fit(m_size + 1);

        const auto index = get_segment_index(m_size);
        const auto offset = get_segment_pos(m_size);
        m_segments[index][offset] = element;

        ++m_size;
    }

    void pop_back()
    {
        assert(m_size > 0);
        --m_size;
    }

    const T& operator[](size_t pos) const
    {
        assert(pos < m_size);

        const auto index = get_segment_index(pos);
        const auto offset = get_segment_pos(pos);
        return m_segments[index][offset];
    }

    T& operator[](size_t pos)
    {
        assert(pos < m_size);

        const auto index = get_segment_index(pos);
        const auto offset = get_segment_pos(pos);
        return m_segments[index][offset];
    }

    const T& at(size_t pos) const
    {
        if (pos >= m_size)
            throw std::out_of_range("SegmentedVector::at");

        const auto index = get_segment_index(pos);
        const auto offset = get_segment_pos(pos);
        return m_segments.at(index).at(offset);
    }

    T& at(size_t pos)
    {
        if (pos >= m_size)
            throw std::out_of_range("SegmentedVector::at");

        const auto index = get_segment_index(pos);
        const auto offset = get_segment_pos(pos);
        return m_segments.at(index).at(offset);
    }

    const T& front() const noexcept
    {
        assert(m_size > 0);
        return (*this)[0];
    }

    T& front() noexcept
    {
        assert(m_size > 0);
        return (*this)[0];
    }

    const T& back() const noexcept
    {
        assert(m_size > 0);
        return (*this)[m_size - 1];
    }

    T& back() noexcept
    {
        assert(m_size > 0);
        return (*this)[m_size - 1];
    }

    size_t memory_usage() const noexcept
    {
        size_t bytes = 0;
        for (const auto& seg : m_segments)
            bytes += seg.capacity() * sizeof(T);
        return bytes;
    }

    size_t capacity() const noexcept { return m_capacity; }
    size_t size() const noexcept { return m_size; }
    bool empty() const noexcept { return m_size == 0; }

private:
    // Segments grow geometrically, i.e., FirstSegmentSize, 2*FirstSegmentSize, 4*FirstSegmentSize, ...
    std::vector<std::vector<T>> m_segments;
    size_t m_capacity;
    size_t m_size;
};

}

#endif