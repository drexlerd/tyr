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

#include "tyr/tyr.hpp"

#include <gtest/gtest.h>

namespace b = tyr::buffer;
namespace f = tyr::formalism;

namespace tyr::tests
{

TEST(TyrTests, TyrBufferIndexedHashSet)
{
    auto arena = b::SegmentedBuffer();
    auto buffer = b::Buffer();
    auto repository = b::IndexedHashSet<f::Predicate<f::FluentTag>>(buffer, arena);
    auto builder = Data<f::Predicate<f::FluentTag>>();

    // Create a unique predicate
    builder.index.value = 0;
    builder.name = "predicate_0";
    builder.arity = 2;

    canonicalize(builder);
    auto [predicate_index_0, success_0] = repository.insert(builder);
    const auto& predicate_0 = repository[predicate_index_0];

    EXPECT_EQ(predicate_0.index.value, 0);
    EXPECT_EQ(predicate_0.name, builder.name);
    EXPECT_EQ(predicate_0.arity, builder.arity);

    // Create a unique predicate
    builder.index.value = 1;
    builder.name = "predicate_1";
    builder.arity = 3;

    canonicalize(builder);
    auto [predicate_index_1, success_1] = repository.insert(builder);
    const auto& predicate_1 = repository[predicate_index_1];

    EXPECT_EQ(predicate_1.index.value, 1);
    EXPECT_EQ(predicate_1.name, builder.name);
    EXPECT_EQ(predicate_1.arity, builder.arity);

    // Create an existing predicate
    builder.index.value = 1;
    builder.name = "predicate_1";
    builder.arity = 3;

    canonicalize(builder);
    auto [predicate_index_2, success_2] = repository.insert(builder);
    const auto& predicate_2 = repository[predicate_index_2];

    EXPECT_EQ(predicate_2.index.value, 1);
    EXPECT_EQ(predicate_2.name, builder.name);
    EXPECT_EQ(predicate_2.arity, builder.arity);
}

}