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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef TYR_COMMON_BITS_HPP_
#define TYR_COMMON_BITS_HPP_

#include <array>
#include <bit>
#include <cassert>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <limits>

namespace tyr
{

/**
 * Partially taken from https://github.com/simongog/sdsl-lite/blob/master/include/sdsl/bits.hpp
 */

namespace bits
{

template<std::unsigned_integral T>
constexpr bool is_power_of_two(T x)
{
    return std::has_single_bit(x);
}

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
}
}

#endif
