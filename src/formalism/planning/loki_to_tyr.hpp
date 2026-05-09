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

#ifndef TYR_SRC_FORMALISM_PLANNING_LOKI_TO_TYR_HPP_
#define TYR_SRC_FORMALISM_PLANNING_LOKI_TO_TYR_HPP_

#include "tyr/common/equal_to.hpp"
#include "tyr/common/hash.hpp"
#include "tyr/formalism/planning/builder.hpp"
#include "tyr/formalism/planning/canonicalization.hpp"
#include "tyr/formalism/planning/declarations.hpp"
#include "tyr/formalism/planning/fdr_context.hpp"
#include "tyr/formalism/planning/grounder.hpp"
#include "tyr/formalism/planning/planning_domain.hpp"
#include "tyr/formalism/planning/planning_task.hpp"
#include "tyr/formalism/planning/repository.hpp"
#include "tyr/formalism/planning/views.hpp"

#include <loki/loki.hpp>
#include <tuple>
#include <unordered_map>
#include <variant>

namespace tyr::formalism::planning
{

using PredicateViewVariant = std::variant<PredicateView<StaticTag>, PredicateView<FluentTag>, PredicateView<DerivedTag>>;

using AtomViewVariant = std::variant<AtomView<StaticTag>, AtomView<FluentTag>, AtomView<DerivedTag>>;

using LiteralViewVariant = std::variant<LiteralView<StaticTag>, LiteralView<FluentTag>, LiteralView<DerivedTag>>;

using GroundAtomViewVariant = std::variant<GroundAtomView<StaticTag>, GroundAtomView<FluentTag>, GroundAtomView<DerivedTag>>;

using GroundAtomOrFactViewVariant = std::variant<GroundAtomView<StaticTag>, GroundAtomView<DerivedTag>, FDRFactView<FluentTag>>;

using GroundLiteralViewVariant = std::variant<GroundLiteralView<StaticTag>, GroundLiteralView<FluentTag>, GroundLiteralView<DerivedTag>>;

using GroundLiteralOrFactViewVariant = std::variant<GroundLiteralView<StaticTag>, GroundLiteralView<DerivedTag>, std::pair<FDRFactView<FluentTag>, bool>>;

using FunctionViewVariant = std::variant<FunctionView<StaticTag>, FunctionView<FluentTag>, FunctionView<AuxiliaryTag>>;

using FunctionTermViewVariant = std::variant<FunctionTermView<StaticTag>, FunctionTermView<FluentTag>, FunctionTermView<AuxiliaryTag>>;

using GroundFunctionTermViewVariant = std::variant<GroundFunctionTermView<StaticTag>, GroundFunctionTermView<FluentTag>, GroundFunctionTermView<AuxiliaryTag>>;

using GroundFunctionTermValueViewVariant =
    std::variant<GroundFunctionTermValueView<StaticTag>, GroundFunctionTermValueView<FluentTag>, GroundFunctionTermValueView<AuxiliaryTag>>;

using NumericEffectViewVariant = std::variant<NumericEffectView<Assign, FluentTag>,
                                              NumericEffectView<Increase, FluentTag>,
                                              NumericEffectView<Decrease, FluentTag>,
                                              NumericEffectView<ScaleUp, FluentTag>,
                                              NumericEffectView<ScaleDown, FluentTag>,
                                              NumericEffectView<Increase, AuxiliaryTag>>;

class LokiToTyrTranslator
{
private:
    /* Computed in prepare step */

    // For type analysis of predicates.
    std::unordered_set<std::string> m_fluent_predicates;   ///< Fluent predicates that appear in an effect
    std::unordered_set<std::string> m_derived_predicates;  ///< Derived predicates

    // For type analysis of functions.
    std::unordered_set<std::string> m_fexpr_functions;            ///< Functions that appear in a lifted function expression, i.e., numeric effect or constraint
    std::unordered_set<std::string> m_effect_function_skeletons;  ///< Functions that appear in an effect

    template<std::ranges::input_range Range>
    void prepare(const Range& range)
    {
        std::for_each(std::begin(range), std::end(range), [&](auto&& arg) { this->prepare(arg); });
    }
    template<typename T>
    void prepare(const std::optional<T>& element)
    {
        if (element.has_value())
        {
            this->prepare(element.value());
        }
    }
    void prepare(loki::FunctionSkeleton element);
    void prepare(loki::Object element);
    void prepare(loki::Parameter element);
    void prepare(loki::Predicate element);
    void prepare(loki::Requirements element);
    void prepare(loki::Type element);
    void prepare(loki::Variable element);
    void prepare(loki::Term element);
    void prepare(loki::Atom element);
    void prepare(loki::Literal element);
    void prepare(loki::FunctionExpressionNumber element);
    void prepare(loki::FunctionExpressionBinaryOperator element);
    void prepare(loki::FunctionExpressionMultiOperator element);
    void prepare(loki::FunctionExpressionMinus element);
    void prepare(loki::FunctionExpressionFunction element);
    void prepare(loki::FunctionExpression element);
    void prepare(loki::Function element);
    void prepare(loki::Condition element);
    void prepare(loki::Effect element);
    void prepare(loki::Action element);
    void prepare(loki::Axiom element);
    void prepare(loki::Domain element);
    void prepare(loki::FunctionValue element);
    void prepare(loki::OptimizationMetric element);
    void prepare(loki::Problem element);

    /**
     * Common translations.
     */

    struct ParameterIndexMapping
    {
        ParameterIndexMapping() = default;

        UnorderedMap<Index<Variable>, ParameterIndex> map;

        void push_parameters(const IndexList<Variable>& parameters);

        void pop_parameters(const IndexList<Variable>& parameters);

        ParameterIndex lookup_parameter_index(Index<Variable> variable);
    };

    ParameterIndexMapping m_param_map;

    template<typename T>
    auto translate_common(const std::vector<const T*>& input, Builder& builder, Repository& context)
    {
        using ReturnType = decltype(this->translate_common(std::declval<const T*>(), builder, context));
        auto output = ::cista::offset::vector<ReturnType> {};
        output.reserve(input.size());
        std::transform(std::begin(input),
                       std::end(input),
                       std::back_inserter(output),
                       [&](auto&& arg) { return this->translate_common(arg, builder, context); });
        return output;
    }

    FunctionViewVariant translate_common(loki::FunctionSkeleton element, Builder& builder, Repository& context);

    Index<Object> translate_common(loki::Object element, Builder& builder, Repository& context);

    Index<Variable> translate_common(loki::Parameter element, Builder& builder, Repository& context);

    PredicateViewVariant translate_common(loki::Predicate element, Builder& builder, Repository& context);

    Index<Variable> translate_common(loki::Variable element, Builder& builder, Repository& context);

    /**
     * Lifted translation.
     */

    template<typename T>
    auto translate_lifted(const std::vector<const T*>& input, Builder& builder, Repository& context)
    {
        using ReturnType = decltype(this->translate_lifted(std::declval<const T*>(), builder, context));
        auto output = ::cista::offset::vector<ReturnType> {};
        output.reserve(input.size());
        std::transform(std::begin(input),
                       std::end(input),
                       std::back_inserter(output),
                       [&](auto&& arg) { return this->translate_lifted(arg, builder, context); });
        return output;
    }

    Data<Term> translate_lifted(loki::Term element, Builder& builder, Repository& context);

    AtomViewVariant translate_lifted(loki::Atom element, Builder& builder, Repository& context);

    LiteralViewVariant translate_lifted(loki::Literal element, Builder& builder, Repository& context);

    Data<FunctionExpression> translate_lifted(loki::FunctionExpressionNumber element, Builder& builder, Repository& context);

    Data<FunctionExpression> translate_lifted(loki::FunctionExpressionBinaryOperator element, Builder& builder, Repository& context);

    Data<FunctionExpression> translate_lifted(loki::FunctionExpressionMultiOperator element, Builder& builder, Repository& context);

    Data<FunctionExpression> translate_lifted(loki::FunctionExpressionMinus element, Builder& builder, Repository& context);

    Data<FunctionExpression> translate_lifted(loki::FunctionExpressionFunction element, Builder& builder, Repository& context);

    Data<FunctionExpression> translate_lifted(loki::FunctionExpression element, Builder& builder, Repository& context);

    FunctionTermViewVariant translate_lifted(loki::Function element, Builder& builder, Repository& context);

    Data<BooleanOperator<Data<FunctionExpression>>> translate_lifted(loki::ConditionNumericConstraint element, Builder& builder, Repository& context);

    Index<ConjunctiveCondition> translate_lifted(loki::Condition element, const IndexList<Variable>& parameters, Builder& builder, Repository& context);

    NumericEffectViewVariant translate_lifted(loki::EffectNumeric element, Builder& builder, Repository& context);

    IndexList<ConditionalEffect> translate_lifted(loki::Effect element, const IndexList<Variable>& parameters, Builder& builder, Repository& context);

    Index<Action> translate_lifted(loki::Action element, Builder& builder, Repository& context);

    Index<Axiom> translate_lifted(loki::Axiom element, Builder& builder, Repository& context);

    /**
     * Grounded translation
     */

    template<typename T>
    auto translate_grounded(const std::vector<const T*>& input, Builder& builder, Repository& context)
    {
        using ReturnType = decltype(this->translate_grounded(std::declval<const T*>(), builder, context));
        auto output = ::cista::offset::vector<ReturnType> {};
        output.reserve(input.size());
        std::transform(std::begin(input),
                       std::end(input),
                       std::back_inserter(output),
                       [&](auto&& arg) { return this->translate_grounded(arg, builder, context); });
        return output;
    }

    Index<Object> translate_grounded(loki::Term element, Builder& builder, Repository& context);

    GroundAtomViewVariant translate_grounded(loki::Atom element, Builder& builder, Repository& context);

    GroundAtomOrFactViewVariant translate_grounded(loki::Atom element, Builder& builder, Repository& context, FDRContext& fdr_context);

    GroundLiteralViewVariant translate_grounded(loki::Literal element, Builder& builder, Repository& context);

    GroundLiteralOrFactViewVariant translate_grounded(loki::Literal element, Builder& builder, Repository& context, FDRContext& fdr_context);

    Data<GroundFunctionExpression> translate_grounded(loki::FunctionExpressionNumber element, Builder& builder, Repository& context);

    Data<GroundFunctionExpression> translate_grounded(loki::FunctionExpressionBinaryOperator element, Builder& builder, Repository& context);

    Data<GroundFunctionExpression> translate_grounded(loki::FunctionExpressionMultiOperator element, Builder& builder, Repository& context);

    Data<GroundFunctionExpression> translate_grounded(loki::FunctionExpressionMinus element, Builder& builder, Repository& context);

    Data<GroundFunctionExpression> translate_grounded(loki::FunctionExpressionFunction element, Builder& builder, Repository& context);

    Data<GroundFunctionExpression> translate_grounded(loki::FunctionExpression element, Builder& builder, Repository& context);

    GroundFunctionTermViewVariant translate_grounded(loki::Function element, Builder& builder, Repository& context);

    GroundFunctionTermValueViewVariant translate_grounded(loki::FunctionValue element, Builder& builder, Repository& context);

    Data<BooleanOperator<Data<GroundFunctionExpression>>> translate_grounded(loki::ConditionNumericConstraint element, Builder& builder, Repository& context);

    Index<GroundConjunctiveCondition> translate_grounded(loki::Condition element, Builder& builder, Repository& context, FDRContext& fdr_context);

    Index<Metric> translate_grounded(loki::OptimizationMetric element, Builder& builder, Repository& context);

public:
    LokiToTyrTranslator() = default;

    PlanningDomain translate(const loki::Domain& domain);

    PlanningTask translate(const loki::Problem& problem, PlanningDomain domain);
};

}

#endif
