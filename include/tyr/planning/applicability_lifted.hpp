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

#include "tyr/common/dynamic_bitset.hpp"
#include "tyr/common/equal_to.hpp"
#include "tyr/common/hash.hpp"
#include "tyr/common/itertools.hpp"
#include "tyr/common/semantics.hpp"
#include "tyr/common/vector.hpp"
#include "tyr/formalism/arithmetic_operator_utils.hpp"
#include "tyr/formalism/boolean_operator_utils.hpp"
#include "tyr/formalism/planning/declarations.hpp"
#include "tyr/formalism/planning/ground_numeric_effect_operator_utils.hpp"
#include "tyr/formalism/planning/repository.hpp"
#include "tyr/formalism/planning/views.hpp"
#include "tyr/planning/applicability_lifted_decl.hpp"
#include "tyr/planning/node.hpp"
#include "tyr/planning/state_accessor.hpp"

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

template<TaskKind Kind, SemanticTag S>
float_t evaluate(float_t element, const LiftedApplicabilityContext<Kind, S>& context);

template<TaskKind Kind, formalism::ArithmeticOpKind O, SemanticTag S>
float_t evaluate(formalism::planning::LiftedUnaryOperatorView<O> element, const LiftedApplicabilityContext<Kind, S>& context);

template<TaskKind Kind, formalism::ArithmeticOpKind O, SemanticTag S>
float_t evaluate(formalism::planning::LiftedBinaryOperatorView<O> element, const LiftedApplicabilityContext<Kind, S>& context);

template<TaskKind Kind, formalism::BooleanOpKind O, SemanticTag S>
bool evaluate(formalism::planning::LiftedBinaryOperatorView<O> element, const LiftedApplicabilityContext<Kind, S>& context);

template<TaskKind Kind, formalism::ArithmeticOpKind O, SemanticTag S>
float_t evaluate(formalism::planning::LiftedMultiOperatorView<O> element, const LiftedApplicabilityContext<Kind, S>& context);

template<TaskKind Kind, SemanticTag S>
float_t evaluate(formalism::planning::FunctionTermView<formalism::StaticTag> element, const LiftedApplicabilityContext<Kind, S>& context);

template<TaskKind Kind, SemanticTag S>
float_t evaluate(formalism::planning::FunctionTermView<formalism::FluentTag> element, const LiftedApplicabilityContext<Kind, S>& context);

template<TaskKind Kind, SemanticTag S>
float_t evaluate(formalism::planning::FunctionTermView<formalism::AuxiliaryTag> element, const LiftedApplicabilityContext<Kind, S>& context);

template<TaskKind Kind, SemanticTag S>
float_t evaluate(formalism::planning::FunctionExpressionView element, const LiftedApplicabilityContext<Kind, S>& context);

template<TaskKind Kind, SemanticTag S>
float_t evaluate(formalism::planning::LiftedArithmeticOperatorView element, const LiftedApplicabilityContext<Kind, S>& context);

template<TaskKind Kind, SemanticTag S>
bool evaluate(formalism::planning::LiftedBooleanOperatorView element, const LiftedApplicabilityContext<Kind, S>& context);

template<TaskKind Kind, formalism::planning::NumericEffectOpKind Op, formalism::FactKind T, SemanticTag S>
float_t evaluate(formalism::planning::NumericEffectView<Op, T> element, const LiftedApplicabilityContext<Kind, S>& context);

template<TaskKind Kind, formalism::FactKind T, SemanticTag S>
float_t evaluate(formalism::planning::NumericEffectOperatorView<T> element, const LiftedApplicabilityContext<Kind, S>& context);

/**
 * is_applicable_if_fires
 */

template<TaskKind Kind, SemanticTag S>
bool is_applicable_if_fires(formalism::planning::ConditionalEffectView element,
                            const LiftedApplicabilityContext<Kind, S>& context,
                            formalism::planning::EffectFamilyList& ref_fluent_effect_families,
                            itertools::cartesian_set::Workspace<Index<formalism::Object>> cartesian_workspace);

/**
 * is_applicable
 */

template<TaskKind Kind, SemanticTag S>
bool is_applicable(formalism::planning::LiteralView<formalism::StaticTag> element, const LiftedApplicabilityContext<Kind, S>& context);

template<TaskKind Kind, SemanticTag S>
bool is_applicable(formalism::planning::LiteralView<formalism::DerivedTag> element, const LiftedApplicabilityContext<Kind, S>& context);

template<TaskKind Kind, formalism::FactKind T, SemanticTag S>
bool is_applicable(formalism::planning::LiteralListView<T> elements, const LiftedApplicabilityContext<Kind, S>& context);

template<formalism::PolarityKind P, TaskKind Kind, SemanticTag S>
bool is_applicable(formalism::planning::FDRFactView<formalism::FluentTag> element, const LiftedApplicabilityContext<Kind, S>& context);

template<formalism::PolarityKind P, TaskKind Kind, SemanticTag S>
bool is_applicable(formalism::planning::FDRFactListView<formalism::FluentTag> elements, const LiftedApplicabilityContext<Kind, S>& context);

template<TaskKind Kind, SemanticTag S>
bool is_applicable(formalism::planning::LiftedBooleanOperatorView element, const LiftedApplicabilityContext<Kind, S>& context);

template<TaskKind Kind, SemanticTag S>
bool is_applicable(formalism::planning::LiftedBooleanOperatorListView elements, const LiftedApplicabilityContext<Kind, S>& context);

template<TaskKind Kind, formalism::planning::NumericEffectOpKind Op, SemanticTag S>
bool is_applicable(formalism::planning::NumericEffectView<Op, formalism::FluentTag> element,
                   const LiftedApplicabilityContext<Kind, S>& context,
                   formalism::planning::EffectFamilyList& ref_fluent_effect_families);

template<TaskKind Kind, SemanticTag S>
bool is_applicable(formalism::planning::NumericEffectOperatorView<formalism::FluentTag> element,
                   const LiftedApplicabilityContext<Kind, S>& context,
                   formalism::planning::EffectFamilyList& ref_fluent_effect_families);

template<TaskKind Kind, SemanticTag S>
bool is_applicable(formalism::planning::NumericEffectOperatorListView<formalism::FluentTag> elements,
                   const LiftedApplicabilityContext<Kind, S>& context,
                   formalism::planning::EffectFamilyList& ref_fluent_effect_families);

template<TaskKind Kind, SemanticTag S>
bool is_applicable(formalism::planning::NumericEffectView<formalism::planning::OpIncrease, formalism::AuxiliaryTag> element,
                   const LiftedApplicabilityContext<Kind, S>& context);

template<TaskKind Kind, SemanticTag S>
bool is_applicable(formalism::planning::NumericEffectOperatorView<formalism::AuxiliaryTag> element, const LiftedApplicabilityContext<Kind, S>& context);

// ConjunctiveCondition

template<TaskKind Kind, SemanticTag S>
bool is_applicable(formalism::planning::ConjunctiveConditionView element, const LiftedApplicabilityContext<Kind, S>& context);

// ConjunctiveEffect

template<TaskKind Kind, SemanticTag S>
bool is_applicable(formalism::planning::ConjunctiveEffectView element,
                   const LiftedApplicabilityContext<Kind, S>& context,
                   formalism::planning::EffectFamilyList& ref_fluent_effect_families);

// Action

template<TaskKind Kind, SemanticTag S>
bool is_applicable(formalism::planning::ActionView element,
                   const LiftedApplicabilityContext<Kind, S>& context,
                   formalism::planning::EffectFamilyList& out_fluent_effect_families,
                   itertools::cartesian_set::Workspace<Index<formalism::Object>> cartesian_workspace);

// Axiom

template<TaskKind Kind, SemanticTag S>
bool is_applicable(formalism::planning::AxiomView element, const LiftedApplicabilityContext<Kind, S>& context);

}

#ifdef TYR_HEADER_INSTANTIATION
#include "tyr/planning/applicability_lifted.ipp"
#endif

#endif
