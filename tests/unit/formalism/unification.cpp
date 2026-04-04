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

#include "tyr/formalism/unification/apply_substitution.hpp"

#include <gtest/gtest.h>

namespace f = tyr::formalism;
namespace fu = tyr::formalism::unification;

namespace tyr::tests
{
namespace
{
using TermSubstitution = fu::SubstitutionFunction<Data<f::Term>>;

Data<f::Term> parameter(size_t index) { return Data<f::Term>(f::ParameterIndex(index)); }

Data<f::Term> object(size_t index) { return Data<f::Term>(Index<f::Object>(index)); }
}

TEST(TyrTests, TyrFormalismUnificationApplySubstitutionFixpoint)
{
    auto sigma = TermSubstitution::from_range(f::ParameterIndex(0), 3);

    EXPECT_TRUE(sigma.assign(f::ParameterIndex(0), parameter(1)));
    EXPECT_TRUE(sigma.assign(f::ParameterIndex(1), object(7)));

    EXPECT_EQ(fu::apply_substitution(parameter(0), sigma), parameter(1));
    EXPECT_EQ(fu::apply_substitution_once(parameter(0), sigma), parameter(1));
    EXPECT_EQ(fu::apply_substitution_fixpoint(parameter(0), sigma), object(7));
    EXPECT_EQ(fu::apply_substitution_fixpoint(parameter(2), sigma), parameter(2));
}

TEST(TyrTests, TyrFormalismUnificationComposeSubstitutions)
{
    auto inner = TermSubstitution::from_range(f::ParameterIndex(0), 2);
    auto outer = TermSubstitution::from_range(f::ParameterIndex(1), 2);

    EXPECT_TRUE(inner.assign(f::ParameterIndex(0), parameter(1)));
    EXPECT_TRUE(outer.assign(f::ParameterIndex(1), object(4)));
    EXPECT_TRUE(outer.assign(f::ParameterIndex(2), object(9)));

    const auto composed = fu::compose_substitutions(outer, inner);

    ASSERT_TRUE(composed.contains_parameter(f::ParameterIndex(0)));
    ASSERT_TRUE(composed.contains_parameter(f::ParameterIndex(1)));
    ASSERT_TRUE(composed.contains_parameter(f::ParameterIndex(2)));

    ASSERT_TRUE(composed[f::ParameterIndex(0)].has_value());
    ASSERT_TRUE(composed[f::ParameterIndex(1)].has_value());
    ASSERT_TRUE(composed[f::ParameterIndex(2)].has_value());

    EXPECT_EQ(*composed[f::ParameterIndex(0)], object(4));
    EXPECT_EQ(*composed[f::ParameterIndex(1)], object(4));
    EXPECT_EQ(*composed[f::ParameterIndex(2)], object(9));

    EXPECT_EQ(fu::apply_substitution_fixpoint(parameter(0), composed), object(4));
    EXPECT_EQ(fu::apply_substitution_fixpoint(parameter(1), composed), object(4));
    EXPECT_EQ(fu::apply_substitution_fixpoint(parameter(2), composed), object(9));
}

TEST(TyrTests, TyrFormalismUnificationSubstitutionTryGet)
{
    auto sigma = TermSubstitution::from_range(f::ParameterIndex(0), 2);

    ASSERT_NE(sigma.try_get(f::ParameterIndex(0)), nullptr);
    EXPECT_FALSE(sigma.try_get(f::ParameterIndex(0))->has_value());
    EXPECT_EQ(sigma.try_get(f::ParameterIndex(7)), nullptr);

    EXPECT_TRUE(sigma.assign(f::ParameterIndex(1), object(11)));

    auto* slot = sigma.try_get(f::ParameterIndex(1));
    ASSERT_NE(slot, nullptr);
    ASSERT_TRUE(slot->has_value());
    EXPECT_EQ(**slot, object(11));
}

}  // namespace tyr::tests
