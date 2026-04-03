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
#include <tyr/common/vector.hpp>

namespace tyr::tests
{

TEST(TyrTests, TyrCommonVector)
{
    const auto dim0 = 2;
    const auto dim1 = 3;
    const auto dim2 = 4;
    auto vec = std::vector<uint64_t>(2 * 3 * 4, uint64_t(0));

    auto mdspan = MDSpan<uint64_t, 3>(vec.data(), std::array<size_t, 3> { dim0, dim1, dim2 });

    EXPECT_EQ(mdspan.size(), 24);
    EXPECT_EQ(mdspan.shapes(), (std::array<size_t, 3> { dim0, dim1, dim2 }));
    EXPECT_EQ(mdspan.stride(), (std::array<size_t, 3> { 12, 4, 1 }));
    EXPECT_EQ(mdspan.begin(), vec.data());
    EXPECT_EQ(mdspan.begin() + mdspan.size(), vec.data() + vec.size());

    auto submdspan_full = mdspan();
    EXPECT_EQ(submdspan_full.size(), 24);

    auto submdspan_0 = mdspan(0);
    EXPECT_EQ(submdspan_0.size(), 12);

    auto submdspan_1 = mdspan(1);
    EXPECT_EQ(submdspan_1.size(), 12);

    auto submdspan_0_0 = mdspan(0, 0);
    EXPECT_EQ(submdspan_0_0.size(), 4);

    auto submdspan_0_1 = mdspan(0, 1);
    EXPECT_EQ(submdspan_0_1.size(), 4);

    auto submdspan_0_2 = mdspan(0, 2);
    EXPECT_EQ(submdspan_0_2.size(), 4);

    submdspan_0_2(0) = 5;
    EXPECT_EQ(submdspan_0_2(0), 5);
    EXPECT_EQ(submdspan_0(2, 0), 5);
    EXPECT_EQ(mdspan(0, 2, 0), 5);

    EXPECT_EQ(submdspan_0_2.data(), &mdspan(0, 2, 0));
    EXPECT_EQ(submdspan_1.data(), &mdspan(1, 0, 0));
}

}
