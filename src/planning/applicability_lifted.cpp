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

#include "tyr/planning/applicability_lifted.hpp"

#include "tyr/planning/lifted_task.hpp"
#include "tyr/planning/lifted_task/state_builder.hpp"

#ifndef TYR_HEADER_INSTANTIATION
#include "tyr/planning/applicability_lifted.ipp"

namespace tyr::planning
{

/**
 * evaluate
 */

// LiftedUnaryOperatorView

template float_t evaluate(formalism::planning::LiftedUnaryOperatorView<formalism::Sub> element, const ApplicabilityContext& context);

// LiftedBinaryOperatorView arithmetic

template float_t evaluate(formalism::planning::LiftedBinaryOperatorView<formalism::Add> element, const ApplicabilityContext& context);
template float_t evaluate(formalism::planning::LiftedBinaryOperatorView<formalism::Sub> element, const ApplicabilityContext& context);
template float_t evaluate(formalism::planning::LiftedBinaryOperatorView<formalism::Mul> element, const ApplicabilityContext& context);
template float_t evaluate(formalism::planning::LiftedBinaryOperatorView<formalism::Div> element, const ApplicabilityContext& context);

// LiftedBinaryOperatorView boolean

template bool evaluate(formalism::planning::LiftedBinaryOperatorView<formalism::Eq> element, const ApplicabilityContext& context);
template bool evaluate(formalism::planning::LiftedBinaryOperatorView<formalism::Ne> element, const ApplicabilityContext& context);
template bool evaluate(formalism::planning::LiftedBinaryOperatorView<formalism::Ge> element, const ApplicabilityContext& context);
template bool evaluate(formalism::planning::LiftedBinaryOperatorView<formalism::Gt> element, const ApplicabilityContext& context);
template bool evaluate(formalism::planning::LiftedBinaryOperatorView<formalism::Le> element, const ApplicabilityContext& context);
template bool evaluate(formalism::planning::LiftedBinaryOperatorView<formalism::Lt> element, const ApplicabilityContext& context);

// LiftedMultiOperatorView

template float_t evaluate(formalism::planning::LiftedMultiOperatorView<formalism::Add> element, const ApplicabilityContext& context);
template float_t evaluate(formalism::planning::LiftedMultiOperatorView<formalism::Mul> element, const ApplicabilityContext& context);

// NumericEffectView

template float_t evaluate(formalism::planning::NumericEffectView<formalism::Assign, formalism::FluentTag> element, const ApplicabilityContext& context);
template float_t evaluate(formalism::planning::NumericEffectView<formalism::Increase, formalism::FluentTag> element, const ApplicabilityContext& context);
template float_t evaluate(formalism::planning::NumericEffectView<formalism::Decrease, formalism::FluentTag> element, const ApplicabilityContext& context);
template float_t evaluate(formalism::planning::NumericEffectView<formalism::ScaleUp, formalism::FluentTag> element, const ApplicabilityContext& context);
template float_t evaluate(formalism::planning::NumericEffectView<formalism::ScaleDown, formalism::FluentTag> element, const ApplicabilityContext& context);

template float_t evaluate(formalism::planning::NumericEffectView<formalism::Increase, formalism::AuxiliaryTag> element, const ApplicabilityContext& context);

// NumericEffectOperatorView

template float_t evaluate(formalism::planning::NumericEffectOperatorView<formalism::FluentTag> element, const ApplicabilityContext& context);
template float_t evaluate(formalism::planning::NumericEffectOperatorView<formalism::AuxiliaryTag> element, const ApplicabilityContext& context);

/**
 * is_applicable
 */

// LiteralListView

template bool is_applicable(formalism::planning::LiteralListView<formalism::StaticTag> elements, const ApplicabilityContext& context);
template bool is_applicable(formalism::planning::LiteralListView<formalism::FluentTag> elements, const ApplicabilityContext& context);
template bool is_applicable(formalism::planning::LiteralListView<formalism::DerivedTag> elements, const ApplicabilityContext& context);

// NumericEffectView over fluent function terms

template bool is_applicable(formalism::planning::NumericEffectView<formalism::Assign, formalism::FluentTag> element,
                            const ApplicabilityContext& context,
                            formalism::planning::EffectFamilyList& ref_fluent_effect_families);
template bool is_applicable(formalism::planning::NumericEffectView<formalism::Increase, formalism::FluentTag> element,
                            const ApplicabilityContext& context,
                            formalism::planning::EffectFamilyList& ref_fluent_effect_families);
template bool is_applicable(formalism::planning::NumericEffectView<formalism::Decrease, formalism::FluentTag> element,
                            const ApplicabilityContext& context,
                            formalism::planning::EffectFamilyList& ref_fluent_effect_families);
template bool is_applicable(formalism::planning::NumericEffectView<formalism::ScaleUp, formalism::FluentTag> element,
                            const ApplicabilityContext& context,
                            formalism::planning::EffectFamilyList& ref_fluent_effect_families);
template bool is_applicable(formalism::planning::NumericEffectView<formalism::ScaleDown, formalism::FluentTag> element,
                            const ApplicabilityContext& context,
                            formalism::planning::EffectFamilyList& ref_fluent_effect_families);

}

#endif