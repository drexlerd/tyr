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

#ifndef TYR_PLANNING_APPLICABILITY_LIFTED_HPP_
#define TYR_PLANNING_APPLICABILITY_LIFTED_HPP_

#include "tyr/analysis/declarations.hpp"
#include "tyr/common/dynamic_bitset.hpp"
#include "tyr/common/equal_to.hpp"
#include "tyr/common/hash.hpp"
#include "tyr/common/itertools.hpp"
#include "tyr/common/vector.hpp"
#include "tyr/formalism/arithmetic_operator_utils.hpp"
#include "tyr/formalism/boolean_operator_utils.hpp"
#include "tyr/formalism/planning/declarations.hpp"
#include "tyr/formalism/planning/ground_numeric_effect_operator_utils.hpp"
#include "tyr/formalism/planning/repository.hpp"
#include "tyr/formalism/planning/views.hpp"
#include "tyr/planning/applicability_lifted_decl.hpp"
#include "tyr/planning/node.hpp"

#include <algorithm>
#include <boost/dynamic_bitset.hpp>
#include <concepts>
#include <iterator>
#include <limits>
#include <numeric>
#include <stdexcept>
#include <vector>

namespace tyr::planning
{

/**
 * evaluate
 */

float_t evaluate(float_t element, const ApplicabilityContext& context);

template<formalism::ArithmeticOpKind O>
float_t evaluate(formalism::planning::LiftedUnaryOperatorView<O> element, const ApplicabilityContext& context);

template<formalism::ArithmeticOpKind O>
float_t evaluate(formalism::planning::LiftedBinaryOperatorView<O> element, const ApplicabilityContext& context);

template<formalism::BooleanOpKind O>
bool evaluate(formalism::planning::LiftedBinaryOperatorView<O> element, const ApplicabilityContext& context);

template<formalism::ArithmeticOpKind O>
float_t evaluate(formalism::planning::LiftedMultiOperatorView<O> element, const ApplicabilityContext& context);

float_t evaluate(formalism::planning::FunctionTermView<formalism::StaticTag> element, const ApplicabilityContext& context);

float_t evaluate(formalism::planning::FunctionTermView<formalism::FluentTag> element, const ApplicabilityContext& context);

float_t evaluate(formalism::planning::FunctionTermView<formalism::AuxiliaryTag> element, const ApplicabilityContext& context);

float_t evaluate(formalism::planning::FunctionExpressionView element, const ApplicabilityContext& context);

float_t evaluate(formalism::planning::LiftedArithmeticOperatorView element, const ApplicabilityContext& context);

bool evaluate(formalism::planning::LiftedBooleanOperatorView element, const ApplicabilityContext& context);

template<formalism::planning::NumericEffectOpKind Op, formalism::FactKind T>
float_t evaluate(formalism::planning::NumericEffectView<Op, T> element, const ApplicabilityContext& context);

template<formalism::FactKind T>
float_t evaluate(formalism::planning::NumericEffectOperatorView<T> element, const ApplicabilityContext& context);

/**
 * is_applicable_if_fires
 */

bool is_applicable_if_fires(formalism::planning::ConditionalEffectView element,
                            const ApplicabilityContext& context,
                            formalism::planning::EffectFamilyList& ref_fluent_effect_families,
                            itertools::cartesian_set::Workspace<Index<formalism::Object>>& cartesian_workspace,
                            const analysis::ConditionalEffectDomain& effect_domains);

bool is_applicable_if_fires(formalism::planning::ConditionalEffectListView elements,
                            const ApplicabilityContext& context,
                            formalism::planning::EffectFamilyList& out_fluent_effect_families,
                            itertools::cartesian_set::Workspace<Index<formalism::Object>>& cartesian_workspace,
                            const analysis::ActionDomain& action_domains);

/**
 * is_applicable
 */

template<formalism::FactKind T>
bool is_applicable(formalism::planning::LiteralView<T> element, const ApplicabilityContext& context);

template<formalism::FactKind T>
bool is_applicable(formalism::planning::LiteralListView<T> elements, const ApplicabilityContext& context);

bool is_applicable(formalism::planning::LiftedBooleanOperatorView element, const ApplicabilityContext& context);

bool is_applicable(formalism::planning::LiftedBooleanOperatorListView elements, const ApplicabilityContext& context);

template<formalism::planning::NumericEffectOpKind Op>
bool is_applicable(formalism::planning::NumericEffectView<Op, formalism::FluentTag> element,
                   const ApplicabilityContext& context,
                   formalism::planning::EffectFamilyList& ref_fluent_effect_families);

bool is_applicable(formalism::planning::NumericEffectOperatorView<formalism::FluentTag> element,
                   const ApplicabilityContext& context,
                   formalism::planning::EffectFamilyList& ref_fluent_effect_families);

bool is_applicable(formalism::planning::NumericEffectOperatorListView<formalism::FluentTag> elements,
                   const ApplicabilityContext& context,
                   formalism::planning::EffectFamilyList& ref_fluent_effect_families);

bool is_applicable(formalism::planning::NumericEffectView<formalism::planning::OpIncrease, formalism::AuxiliaryTag> element,
                   const ApplicabilityContext& context);

bool is_applicable(formalism::planning::NumericEffectOperatorView<formalism::AuxiliaryTag> element, const ApplicabilityContext& context);

// ConjunctiveCondition

bool is_applicable(formalism::planning::ConjunctiveConditionView element, const ApplicabilityContext& context);

// ConjunctiveEffect

bool is_applicable(formalism::planning::ConjunctiveEffectView element,
                   const ApplicabilityContext& context,
                   formalism::planning::EffectFamilyList& ref_fluent_effect_families);

// Action

bool is_applicable(formalism::planning::ActionView element,
                   const ApplicabilityContext& context,
                   formalism::planning::EffectFamilyList& out_fluent_effect_families,
                   itertools::cartesian_set::Workspace<Index<formalism::Object>>& cartesian_workspace,
                   const analysis::ActionDomain& action_domains);

// Axiom

bool is_applicable(formalism::planning::AxiomView element, const ApplicabilityContext& context);

}

#ifdef TYR_HEADER_INSTANTIATION
#include "tyr/planning/applicability_lifted.ipp"
#endif

#endif