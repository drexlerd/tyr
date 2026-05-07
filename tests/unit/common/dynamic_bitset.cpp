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

#include <gtest/gtest.h>
#include <tyr/common/config.hpp>
#include <tyr/common/dynamic_bitset.hpp>
#include <vector>

namespace tyr::tests
{

TEST(TyrTests, TyrCommonDynamicBitset)
{
    auto lhs_blocks = std::vector<uint64_t>(BitsetSpan<uint64_t>::num_blocks(70), 0);
    auto rhs_blocks = std::vector<uint64_t>(BitsetSpan<uint64_t>::num_blocks(70), 0);

    auto lhs = BitsetSpan<uint64_t>(lhs_blocks.data(), 70);
    auto rhs = BitsetSpan<uint64_t>(rhs_blocks.data(), 70);

    EXPECT_EQ(lhs.size(), 70);
    EXPECT_FALSE(lhs.empty());
    EXPECT_TRUE(lhs.none());
    EXPECT_FALSE(lhs.any());
    EXPECT_FALSE(lhs.all());

    lhs[1] = true;
    lhs.set(69);
    lhs.set(68, false);
    rhs.set(1);
    rhs.set(2);
    rhs.set(69);

    EXPECT_TRUE(lhs[1]);
    EXPECT_FALSE(lhs[2]);
    EXPECT_TRUE(lhs.any());
    EXPECT_FALSE(lhs.none());
    EXPECT_FALSE(lhs.all());

    auto lhs_raw_blocks = lhs.blocks();
    lhs_raw_blocks.back() |= ~BitsetSpan<uint64_t>::last_mask(lhs.size());
    EXPECT_FALSE(lhs.trailing_bits_zero());
    lhs.clear_trailing_bits();
    EXPECT_TRUE(lhs.trailing_bits_zero());

    EXPECT_TRUE(lhs.intersects(rhs));
    EXPECT_TRUE(lhs.is_subset_of(rhs));
    EXPECT_TRUE(lhs.is_proper_subset_of(rhs));
    EXPECT_TRUE(rhs.is_superset_of(lhs));
    EXPECT_TRUE(rhs.is_proper_superset_of(lhs));
    EXPECT_FALSE(rhs.is_subset_of(lhs));

    lhs[2].flip();
    EXPECT_TRUE(lhs == rhs);
    EXPECT_TRUE(lhs.is_subset_of(rhs));
    EXPECT_FALSE(lhs.is_proper_subset_of(rhs));

    lhs.flip();
    EXPECT_FALSE(lhs.intersects(rhs));
    EXPECT_TRUE(lhs.trailing_bits_zero());

    lhs ^= rhs;
    EXPECT_TRUE(lhs.all());
}

}
