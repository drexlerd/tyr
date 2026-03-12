/*
 * Copyright (C) 2025 Dominik Drexler
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
 *<
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef TYR_COMMON_BIT_PACKED_ARRAY_POOL_HPP_
#define TYR_COMMON_BIT_PACKED_ARRAY_POOL_HPP_

#include "tyr/common/bits.hpp"

#include <bit>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <utility>
#include <vector>

namespace tyr
{
template<typename Encoder, typename Block>
concept BlockCoder = std::unsigned_integral<Block> && requires(Block block, typename Encoder::value_type value) {
    { Encoder::decode(block) } -> std::convertible_to<typename Encoder::value_type>;
    { Encoder::encode(value) } -> std::convertible_to<Block>;
};

template<std::unsigned_integral Block>
struct ForwardingBlockCoder
{
    using value_type = Block;

    static Block decode(Block block) noexcept { return block; }
    static Block encode(value_type value) noexcept { return value; }
};

static_assert(BlockCoder<ForwardingBlockCoder<uint32_t>, uint32_t>);

/// Stores fixed-length arrays as bit-packed unsigned integer codes.
/// The exposed value type is obtained via Encoder::decode.
template<std::unsigned_integral Block, typename Coder>
    requires BlockCoder<Coder, Block>
class BitPackedArrayPool
{
public:
    using value_type = typename Coder::value_type;

    static constexpr std::size_t digits = std::numeric_limits<Block>::digits;
    static constexpr size_t block_shift = std::countr_zero(digits);

    class ArrayView;

    static auto get_segment_index(size_t index) noexcept { return std::bit_width(index + 1) - 1; }
    static auto get_segment_pos(size_t index) noexcept { return index - ((size_t { 1 } << get_segment_index(index)) - 1); }

    auto operator[](size_t index) const noexcept { return ArrayView(index, *this); }

    void push_back(std::span<const value_type> array) {}

    auto length() const noexcept { return m_length; }
    auto width() const noexcept { return m_width; }
    auto capacity() const noexcept { return m_capacity; }
    auto size() const noexcept { return m_size; }

    explicit BitPackedArrayPool(size_t length, uint8_t width) : m_length(length), m_width(width), m_capacity(0), m_size(0) {}

    class ArrayView
    {
    public:
        ArrayView(size_t index, const BitPackedArrayPool& pool) : m_length(pool.length()), m_data(nullptr), m_width(pool.width()), m_offset()
        {
            const auto seg_idx = get_segment_index(index);
            const auto seg_pos = get_segment_pos(index);
            const auto start_bit = seg_pos * m_width * m_length;

            m_data = pool.m_segments[seg_idx].data() + (start_bit >> block_shift);
            m_offset = static_cast<uint8_t>(start_bit & (digits - 1));
        }

        value_type operator[](size_t pos) const noexcept
        {
            assert(pos < m_length);

            const size_t bit_index = static_cast<size_t>(m_offset) + pos * m_width;
            const Block* word = m_data + (bit_index >> block_shift);
            const uint8_t offset = static_cast<uint8_t>(bit_index & (digits - 1));

            return Coder::decode(bits::read_int<Block>(word, offset, m_width));
        }

        size_t size() const noexcept { return m_length; }

    private:
        size_t m_length;
        const Block* m_data;
        uint8_t m_width;
        uint8_t m_offset;
    };

private:
    // Segments grow geometrically, i.e., 1, 2, 4, 8, 16, 32, 64, 128 to make index access easy
    std::vector<std::vector<Block>> m_segments;

    size_t m_length;
    uint8_t m_width;

    size_t m_capacity;
    size_t m_size;
};
}

#endif
