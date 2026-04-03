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

#ifndef TYR_COMMON_BIT_HPP_
#define TYR_COMMON_BIT_HPP_

#include <array>
#include <bit>
#include <cassert>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>

namespace tyr::bit
{

/**
 * Partially taken from https://github.com/simongog/sdsl-lite/blob/master/include/sdsl/bits.hpp
 */

template<std::unsigned_integral T>
constexpr bool is_power_of_two(T x)
{
    return std::has_single_bit(x);
}

template<std::unsigned_integral T>
constexpr T ceil_div(T x, T y)
{
    return (x + y - 1) / y;
}

template<std::unsigned_integral T>
constexpr T bits_needed(T domain_size)
{
    if (domain_size <= 1)
        return 1;
    return std::bit_width(static_cast<T>(domain_size - 1));
}

template<std::unsigned_integral Block>
inline constexpr std::size_t bits_per_block_v = std::numeric_limits<Block>::digits;

//! lo_set[i] is a w-bit word with the i least significant bits set and the high bits not set.
/*! lo_set[0] = 0ULL, lo_set[1]=1ULL, lo_set[2]=3ULL...
 */
template<std::unsigned_integral Block>
consteval auto make_lo_set()
{
    constexpr std::size_t digits = std::numeric_limits<Block>::digits;
    std::array<Block, digits + 1> table {};

    table[0] = Block { 0 };

    for (std::size_t i = 1; i < digits; ++i)
        table[i] = (Block { 1 } << i) - Block { 1 };

    table[digits] = ~Block { 0 };
    return table;
}

template<std::unsigned_integral Block>
constexpr auto lo_set = make_lo_set<Block>();

/// @brief Write a packed unsigned integer of width len starting at bit offset in word[0].
/// @tparam Block is the unsigned integral block type.
/// @param word is a pointer to the first block containing at least one bit.
/// @param x is the value to write.
/// @param offset is the offset relative to the 0-th bit in the first block.
/// @param len is the width of the integer to write.
template<std::unsigned_integral Block>
constexpr void write_int(Block* word, Block x, uint8_t offset, const uint8_t len)
{
    constexpr std::size_t digits = std::numeric_limits<Block>::digits;

    static_assert(is_power_of_two(digits), "Block width must be a power of two.");
    assert(len <= digits && "Width exceeds block width.");
    assert(offset < digits && "Offset must lie within one block.");

    constexpr Block all_set = static_cast<Block>(-1);

    x &= lo_set<Block>[len];
    if (offset + len >= digits)  // Use >= to avoid UB from shifting all_set in the else case by exactly digits.
    {
        *word &= ((lo_set<Block>[offset]));
        *word |= (x << offset);
        if ((offset = (offset + len) & (digits - 1)))  // Number of bits written into word[1]; zero on exact boundary
        {
            *(word + 1) &= (~lo_set<Block>[offset]);
            *(word + 1) |= (x >> (len - offset));
        }
    }
    else
    {
        *word &= ((all_set << (offset + len)) | lo_set<Block>[offset]);
        *word |= (x << offset);
    }
}

/// @brief Read a packed unsigned integer of width len starting at bit offset in word[0].
/// @tparam Block is the unsigned integral block type.
/// @param word is a pointer to the first block containing at least one bit.
/// @param offset is the offset relative to the 0-th bit in the first block.
/// @param len is the width of the integer to read.
/// @return the read value.
template<std::unsigned_integral Block>
constexpr Block read_int(const Block* word, uint8_t offset, const uint8_t len)
{
    constexpr std::size_t digits = std::numeric_limits<Block>::digits;

    static_assert(is_power_of_two(digits), "Block width must be a power of two.");
    assert(len <= digits && "Width exceeds block width.");
    assert(offset < digits && "Offset must lie within one block.");

    const Block w1 = (*word) >> offset;

    if (offset + len > digits)
        return w1 | ((*(word + 1) & lo_set<Block>[(offset + len) & (digits - 1)]) << (digits - offset));
    else
        return w1 & lo_set<Block>[len];
}

template<typename Encoder, typename Block>
concept BlockCoder = std::unsigned_integral<Block> && requires(Block block, typename Encoder::value_type value) {
    { Encoder::decode(block) } -> std::convertible_to<typename Encoder::value_type>;
    { Encoder::encode(value) } -> std::convertible_to<Block>;
};

template<std::unsigned_integral Block>
struct ForwardingBlockCoder
{
    using value_type = Block;

    static constexpr value_type decode(Block block) noexcept { return block; }
    static constexpr Block encode(value_type value) noexcept { return value; }
};

static_assert(BlockCoder<ForwardingBlockCoder<uint32_t>, uint32_t>);

template<std::unsigned_integral Block, typename Coder = bit::ForwardingBlockCoder<Block>>
    requires BlockCoder<Coder, Block>
class int_reference
{
private:
    constexpr void ensure_fits(Block raw) const
    {
        const Block mask = bit::lo_set<Block>[m_len];
        if ((raw & ~mask) != 0)
            throw std::out_of_range("int_reference: encoded value exceeds bit width");
    }

public:
    using value_type = typename Coder::value_type;

    constexpr int_reference(Block* word, uint8_t offset, uint8_t len) noexcept : m_word(word), m_offset(offset), m_len(len) {}

    constexpr int_reference& operator=(const value_type& value)
    {
        const auto raw = Coder::encode(value);
        ensure_fits(raw);
        bit::write_int(m_word, raw, m_offset, m_len);
        return *this;
    }

    constexpr int_reference& operator=(const int_reference& other) { return (*this = static_cast<value_type>(other)); }

    constexpr operator value_type() const { return Coder::decode(bit::read_int(m_word, m_offset, m_len)); }

private:
    Block* m_word;
    uint8_t m_offset;
    uint8_t m_len;
};

template<std::unsigned_integral Block, typename Coder = bit::ForwardingBlockCoder<Block>>
    requires BlockCoder<Coder, Block>
class block_reference
{
public:
    using value_type = typename Coder::value_type;

    constexpr explicit block_reference(Block* ptr) noexcept : m_ptr(ptr) {}

    constexpr block_reference& operator=(const value_type& value)
    {
        *m_ptr = Coder::encode(value);
        return *this;
    }

    constexpr block_reference& operator=(const block_reference& other) { return (*this = static_cast<value_type>(other)); }

    constexpr operator value_type() const { return Coder::decode(*m_ptr); }

private:
    Block* m_ptr;
};

template<std::unsigned_integral Block>
struct bit_reference
{
    Block* data;
    size_t bit;

    bit_reference(Block* data, size_t bit) noexcept : data(data), bit(bit) {}

    static constexpr size_t bits_per_block = std::numeric_limits<Block>::digits;

    static constexpr size_t block_index(size_t bit) { return bit / bits_per_block; }
    static constexpr size_t bit_index(size_t bit) { return bit % bits_per_block; }

    bit_reference& operator=(bool value)
    {
        Block& block = data[block_index(bit)];
        const Block mask = Block(1) << bit_index(bit);

        if (value)
            block |= mask;
        else
            block &= ~mask;

        return *this;
    }

    bit_reference& operator=(const bit_reference& other) noexcept { return *this = static_cast<bool>(other); }

    explicit operator bool() const noexcept { return ((data[block_index(bit)] >> bit_index(bit)) & Block(1)) != 0; }
};

}

#endif
