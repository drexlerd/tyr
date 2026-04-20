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
float_t evaluate(float_t element, const LiftedApplicabilityContext<Kind, S>& context)
{
    return element;
}

template<TaskKind Kind, formalism::ArithmeticOpKind O, SemanticTag S>
float_t evaluate(formalism::planning::LiftedUnaryOperatorView<O> element, const LiftedApplicabilityContext<Kind, S>& context)
{
    return formalism::apply(O {}, evaluate(element.get_arg(), context));
}

template<TaskKind Kind, formalism::ArithmeticOpKind O, SemanticTag S>
float_t evaluate(formalism::planning::LiftedBinaryOperatorView<O> element, const LiftedApplicabilityContext<Kind, S>& context)
{
    return formalism::apply(O {}, evaluate(element.get_lhs(), context), evaluate(element.get_rhs(), context));
}

template<TaskKind Kind, formalism::BooleanOpKind O, SemanticTag S>
bool evaluate(formalism::planning::LiftedBinaryOperatorView<O> element, const LiftedApplicabilityContext<Kind, S>& context)
{
    return formalism::apply(O {}, evaluate(element.get_lhs(), context), evaluate(element.get_rhs(), context));
}

template<TaskKind Kind, formalism::ArithmeticOpKind O, SemanticTag S>
float_t evaluate(formalism::planning::LiftedMultiOperatorView<O> element, const LiftedApplicabilityContext<Kind, S>& context)
{
    const auto child_fexprs = element.get_args();

    return std::accumulate(std::next(child_fexprs.begin()),  // Start from the second expression
                           child_fexprs.end(),
                           evaluate(child_fexprs.front(), context),
                           [&](const auto& value, const auto& child_expr)
                           { return formalism::apply(formalism::OpMul {}, value, evaluate(child_expr, context)); });
}

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

template<TaskKind Kind, SemanticTag S>
bool is_applicable_if_fires(formalism::planning::GroundConditionalEffectListView elements,
                            const LiftedApplicabilityContext<Kind, S>& context,
                            formalism::planning::EffectFamilyList& out_fluent_effect_families,
                            itertools::cartesian_set::Workspace<Index<formalism::Object>> cartesian_workspace)
{
    out_fluent_effect_families.clear();

    return std::all_of(elements.begin(),
                       elements.end(),
                       [&](auto&& cond_effect) { return is_applicable_if_fires(cond_effect, context, out_fluent_effect_families, cartesian_workspace); });
}

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
bool is_applicable(formalism::planning::LiftedBooleanOperatorView element, const LiftedApplicabilityContext<Kind, S>& context)
{
    return evaluate(element, context);
}

template<TaskKind Kind, SemanticTag S>
bool is_applicable(formalism::planning::LiftedBooleanOperatorListView elements, const LiftedApplicabilityContext<Kind, S>& context)
{
    return std::all_of(elements.begin(), elements.end(), [&](auto&& arg) { return is_applicable(arg, context); });
}

template<TaskKind Kind, formalism::planning::NumericEffectOpKind Op, SemanticTag S>
bool is_applicable(formalism::planning::NumericEffectView<Op, formalism::FluentTag> element,
                   const LiftedApplicabilityContext<Kind, S>& context,
                   formalism::planning::EffectFamilyList& ref_fluent_effect_families)
{
    const auto fterm_index = element.get_fterm().get_index();
    ref_fluent_effect_families.resize(fterm_index.get_value() + 1, formalism::planning::EffectFamily::NONE);

    // Check non-conflicting effects
    if (!is_compatible_effect_family(Op::family, ref_fluent_effect_families[fterm_index.get_value()]))
        return false;  /// incompatible effects

    ref_fluent_effect_families[fterm_index.get_value()] = Op::family;

    // Check fterm is well-defined in context
    if constexpr (!std::is_same_v<Op, formalism::planning::OpAssign>)
    {
        if (std::isnan(evaluate(element.get_fterm(), context)))
            return false;  /// target function is undefined and operator is not assign
    }

    // Check fexpr is well-defined in context
    return !std::isnan(evaluate(element.get_fexpr(), context));
}

template<TaskKind Kind, SemanticTag S>
bool is_applicable(formalism::planning::NumericEffectOperatorView<formalism::FluentTag> element,
                   const LiftedApplicabilityContext<Kind, S>& context,
                   formalism::planning::EffectFamilyList& ref_fluent_effect_families)
{
    return visit([&](auto&& arg) { return is_applicable(arg, context, ref_fluent_effect_families); }, element.get_variant());
}

template<TaskKind Kind, SemanticTag S>
bool is_applicable(formalism::planning::NumericEffectOperatorListView<formalism::FluentTag> elements,
                   const LiftedApplicabilityContext<Kind, S>& context,
                   formalism::planning::EffectFamilyList& ref_fluent_effect_families)
{
    return std::all_of(elements.begin(), elements.end(), [&](auto&& arg) { return is_applicable(arg, context, ref_fluent_effect_families); });
}

template<TaskKind Kind, SemanticTag S>
bool is_applicable(formalism::planning::NumericEffectView<formalism::planning::OpIncrease, formalism::AuxiliaryTag> element,
                   const LiftedApplicabilityContext<Kind, S>& context)
{
    // Check fexpr is well-defined in context
    return !std::isnan(evaluate(element.get_fexpr(), context));
}

template<TaskKind Kind, SemanticTag S>
bool is_applicable(formalism::planning::NumericEffectOperatorView<formalism::AuxiliaryTag> element, const LiftedApplicabilityContext<Kind, S>& context)
{
    return visit([&](auto&& arg) { return is_applicable(arg, context); }, element.get_variant());
}

// ConjunctiveCondition

template<TaskKind Kind, SemanticTag S>
bool is_applicable(formalism::planning::ConjunctiveConditionView element, const LiftedApplicabilityContext<Kind, S>& context)
{
    return is_applicable(element.template get_literals<formalism::StaticTag>(), context)  //
           && is_applicable(element.template get_literals<formalism::FluentTag>(), context)
           && is_applicable(element.template get_literals<formalism::DerivedTag>(), context)  //
           && is_applicable(element.get_numeric_constraints(), context);
}

// ConjunctiveEffect

template<TaskKind Kind, SemanticTag S>
bool is_applicable(formalism::planning::ConjunctiveEffectView element,
                   const LiftedApplicabilityContext<Kind, S>& context,
                   formalism::planning::EffectFamilyList& ref_fluent_effect_families)
{
    return is_applicable(element.get_numeric_effects(), context, ref_fluent_effect_families)
           && (!element.get_auxiliary_numeric_effect().has_value() || is_applicable(element.get_auxiliary_numeric_effect().value(), context));
}

// Action

template<TaskKind Kind, SemanticTag S>
bool is_applicable(formalism::planning::ActionView element,
                   const LiftedApplicabilityContext<Kind, S>& context,
                   formalism::planning::EffectFamilyList& out_fluent_effect_families,
                   itertools::cartesian_set::Workspace<Index<formalism::Object>> cartesian_workspace)
{
    return is_applicable(element.get_condition(), context)
           && is_applicable_if_fires(element.get_effects(), context, out_fluent_effect_families, cartesian_workspace);
}

// Axiom

template<TaskKind Kind, SemanticTag S>
bool is_applicable(formalism::planning::AxiomView element, const LiftedApplicabilityContext<Kind, S>& context)
{
    return is_applicable(element.get_body(), context);
}

}
