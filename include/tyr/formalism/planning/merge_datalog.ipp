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

#include "tyr/common/declarations.hpp"
#include "tyr/common/equal_to.hpp"
#include "tyr/common/hash.hpp"
#include "tyr/common/macros.hpp"
#include "tyr/common/tuple.hpp"
#include "tyr/formalism/datalog/builder.hpp"
#include "tyr/formalism/datalog/canonicalization.hpp"
#include "tyr/formalism/datalog/declarations.hpp"
#include "tyr/formalism/datalog/repository.hpp"
#include "tyr/formalism/planning/declarations.hpp"
#include "tyr/formalism/planning/merge_datalog_decl.hpp"
#include "tyr/formalism/planning/repository.hpp"

namespace tyr::formalism::planning
{

// Common

TYR_INLINE_IMPL std::pair<formalism::datalog::VariableView, bool> merge_p2d(VariableView element, MergeDatalogContext& context)
{
    auto variable_ptr = context.builder.template get_builder<Variable>();
    auto& variable = *variable_ptr;
    variable.clear();

    variable.name = element.get_name();

    canonicalize(variable);
    return context.destination.get_or_create(variable);
}

TYR_INLINE_IMPL std::pair<formalism::datalog::ObjectView, bool> merge_p2d(ObjectView element, MergeDatalogContext& context)
{
    auto object_ptr = context.builder.template get_builder<Object>();
    auto& object = *object_ptr;
    object.clear();

    object.name = element.get_name();

    canonicalize(object);
    return context.destination.get_or_create(object);
}

TYR_INLINE_IMPL Data<Term> merge_p2d(TermView element, MergeDatalogContext& context)
{
    return visit(
        [&](auto&& arg)
        {
            using Alternative = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<Alternative, ParameterIndex>)
                return Data<Term>(arg);
            else if constexpr (std::is_same_v<Alternative, ObjectView>)
                return Data<Term>(merge_p2d(arg, context).first.get_index());
            else
                static_assert(dependent_false<Alternative>::value, "Missing case");
        },
        element.get_variant());
}

// Propositional

template<FactKind T_SRC, FactKind T_DST>
std::pair<formalism::datalog::PredicateView<T_DST>, bool> merge_p2d(PredicateView<T_SRC> element, MergeDatalogContext& context)
{
    auto predicate_ptr = context.builder.template get_builder<Predicate<T_DST>>();
    auto& predicate = *predicate_ptr;
    predicate.clear();

    predicate.name = element.get_name();
    predicate.arity = element.get_arity();

    canonicalize(predicate);
    return context.destination.get_or_create(predicate);
}

template<FactKind T_SRC, FactKind T_DST>
std::pair<formalism::datalog::AtomView<T_DST>, bool>
merge_p2d(AtomView<T_SRC> element,  //
          const UnorderedMap<PredicateView<T_SRC>, formalism::datalog::PredicateView<T_DST>>& predicate_mapping,
          MergeDatalogContext& context)
{
    auto atom_ptr = context.builder.template get_builder<formalism::datalog::Atom<T_DST>>();
    auto& atom = *atom_ptr;
    atom.clear();

    atom.predicate = predicate_mapping.at(element.get_predicate()).get_index();
    for (const auto term : element.get_terms())
        atom.terms.push_back(merge_p2d(term, context));

    canonicalize(atom);
    return context.destination.get_or_create(atom);
}

template<FactKind T_SRC, FactKind T_DST>
std::pair<formalism::datalog::PredicateBindingView<T_DST>, bool>
merge_p2d(PredicateBindingView<T_SRC> element,  //
          const UnorderedMap<PredicateView<T_SRC>, formalism::datalog::PredicateView<T_DST>>& predicate_mapping,
          MergeDatalogContext& context)
{
    auto binding_ptr = context.builder.template get_builder<RelationBinding<Predicate<T_DST>>>();
    auto& binding = *binding_ptr;
    binding.clear();

    binding.relation = predicate_mapping.at(element.get_relation()).get_index();
    for (const auto object : element.get_objects())
        binding.objects.push_back(object.get_index());

    canonicalize(binding);
    return context.destination.get_or_create(binding);
}

template<FactKind T_SRC, FactKind T_DST>
std::pair<formalism::datalog::GroundAtomView<T_DST>, bool>
merge_p2d(GroundAtomView<T_SRC> element,  //
          const UnorderedMap<PredicateView<T_SRC>, formalism::datalog::PredicateView<T_DST>>& predicate_mapping,
          MergeDatalogContext& context)
{
    auto atom_ptr = context.builder.template get_builder<formalism::datalog::GroundAtom<T_DST>>();
    auto& atom = *atom_ptr;
    atom.clear();

    atom.binding = merge_p2d<T_SRC, T_DST>(element.get_row(), predicate_mapping, context).first.get_index();

    canonicalize(atom);
    return context.destination.get_or_create(atom);
}

template<FactKind T_SRC, FactKind T_DST>
std::pair<formalism::datalog::LiteralView<T_DST>, bool>
merge_p2d(LiteralView<T_SRC> element,  //
          const UnorderedMap<PredicateView<T_SRC>, formalism::datalog::PredicateView<T_DST>>& predicate_mapping,
          MergeDatalogContext& context)
{
    auto literal_ptr = context.builder.template get_builder<formalism::datalog::Literal<T_DST>>();
    auto& literal = *literal_ptr;
    literal.clear();

    literal.polarity = element.get_polarity();
    literal.atom = merge_p2d<T_SRC, T_DST>(element.get_atom(), predicate_mapping, context).first.get_index();

    canonicalize(literal);
    return context.destination.get_or_create(literal);
}

template<FactKind T_SRC, FactKind T_DST>
std::pair<formalism::datalog::GroundLiteralView<T_DST>, bool>
merge_p2d(GroundLiteralView<T_SRC> element,  //
          const UnorderedMap<PredicateView<T_SRC>, formalism::datalog::PredicateView<T_DST>>& predicate_mapping,
          MergeDatalogContext& context)
{
    auto literal_ptr = context.builder.template get_builder<formalism::datalog::GroundLiteral<T_DST>>();
    auto& literal = *literal_ptr;
    literal.clear();

    literal.polarity = element.get_polarity();
    literal.atom = merge_p2d<T_SRC, T_DST>(element.get_atom(), predicate_mapping, context).first.get_index();

    canonicalize(literal);
    return context.destination.get_or_create(literal);
}

TYR_INLINE_IMPL std::optional<formalism::datalog::GroundLiteralView<FluentTag>>
merge_p2d(FDRFactView<FluentTag> element,
          bool polarity,
          const UnorderedMap<PredicateView<FluentTag>, formalism::datalog::PredicateView<FluentTag>>& predicate_mapping,
          MergeDatalogContext& context)
{
    if (!element.has_value())
        return std::nullopt;

    auto literal_ptr = context.builder.template get_builder<formalism::datalog::GroundLiteral<FluentTag>>();
    auto& literal = *literal_ptr;
    literal.clear();
    literal.polarity = polarity;
    literal.atom = merge_p2d(element.get_atom().value(), predicate_mapping, context).first.get_index();

    formalism::datalog::canonicalize(literal);
    return context.destination.get_or_create(literal).first;
}

TYR_INLINE_IMPL std::pair<formalism::datalog::GroundConjunctiveConditionView, bool>
merge_p2d(GroundConjunctiveConditionView element,
          const UnorderedMap<PredicateView<FluentTag>, formalism::datalog::PredicateView<FluentTag>>& fluent_predicate_mapping,
          const UnorderedMap<PredicateView<DerivedTag>, formalism::datalog::PredicateView<FluentTag>>& derived_predicate_mapping,
          MergeDatalogContext& context)
{
    auto condition_ptr = context.builder.template get_builder<formalism::datalog::GroundConjunctiveCondition>();
    auto& condition = *condition_ptr;
    condition.clear();

    for (const auto fact : element.template get_facts<PositiveTag>())
        if (const auto literal = merge_p2d(fact, true, fluent_predicate_mapping, context))
            condition.fluent_literals.push_back(literal->get_index());

    for (const auto fact : element.template get_facts<NegativeTag>())
        if (const auto literal = merge_p2d(fact, false, fluent_predicate_mapping, context))
            condition.fluent_literals.push_back(literal->get_index());

    for (const auto literal : element.template get_literals<DerivedTag>())
        condition.fluent_literals.push_back(merge_p2d(literal, derived_predicate_mapping, context).first.get_index());

    for (const auto numeric_constraint : element.get_numeric_constraints())
        condition.numeric_constraints.push_back(merge_p2d(numeric_constraint, context));

    formalism::datalog::canonicalize(condition);
    return context.destination.get_or_create(condition);
}

// Numeric

template<FactKind T>
std::pair<formalism::datalog::FunctionView<T>, bool> merge_p2d(FunctionView<T> element, MergeDatalogContext& context)
{
    auto function_ptr = context.builder.template get_builder<formalism::Function<T>>();
    auto& function = *function_ptr;
    function.clear();

    function.name = element.get_name();
    function.arity = element.get_arity();

    canonicalize(function);
    return context.destination.get_or_create(function);
}

template<FactKind T>
std::pair<formalism::datalog::FunctionTermView<T>, bool> merge_p2d(FunctionTermView<T> element, MergeDatalogContext& context)
{
    auto fterm_ptr = context.builder.template get_builder<formalism::datalog::FunctionTerm<T>>();
    auto& fterm = *fterm_ptr;
    fterm.clear();

    fterm.function = element.get_function().get_index();
    for (const auto term : element.get_terms())
        fterm.terms.push_back(merge_p2d(term, context));

    canonicalize(fterm);
    return context.destination.get_or_create(fterm);
}

template<FactKind T>
std::pair<formalism::datalog::FunctionBindingView<T>, bool> merge_p2d(FunctionBindingView<T> element, MergeDatalogContext& context)
{
    auto binding_ptr = context.builder.template get_builder<RelationBinding<Function<T>>>();
    auto& binding = *binding_ptr;
    binding.clear();

    binding.relation = merge_p2d(element.get_relation(), context).first.get_index();
    for (const auto object : element.get_objects())
        binding.objects.push_back(object.get_index());

    canonicalize(binding);
    return context.destination.get_or_create(binding);
}

template<FactKind T>
std::pair<formalism::datalog::GroundFunctionTermView<T>, bool> merge_p2d(GroundFunctionTermView<T> element, MergeDatalogContext& context)
{
    auto fterm_ptr = context.builder.template get_builder<formalism::datalog::GroundFunctionTerm<T>>();
    auto& fterm = *fterm_ptr;
    fterm.clear();

    fterm.binding = merge_p2d(element.get_row(), context).first.get_index();

    canonicalize(fterm);
    return context.destination.get_or_create(fterm);
}

template<FactKind T>
std::pair<formalism::datalog::GroundFunctionTermValueView<T>, bool> merge_p2d(GroundFunctionTermValueView<T> element, MergeDatalogContext& context)
{
    auto fterm_value_ptr = context.builder.template get_builder<formalism::datalog::GroundFunctionTermValue<T>>();
    auto& fterm_value = *fterm_value_ptr;
    fterm_value.clear();

    fterm_value.fterm = merge_p2d(element.get_fterm(), context).first.get_index();
    fterm_value.value = element.get_value();

    canonicalize(fterm_value);
    return context.destination.get_or_create(fterm_value);
}

template<formalism::datalog::NumericEffectOpKind DOp, NumericEffectOpKind Op>
std::pair<formalism::datalog::NumericEffectView<DOp, FluentTag>, bool>
merge_numeric_effect_as(NumericEffectView<Op, FluentTag> element, MergeDatalogContext& context)
{
    auto numeric_effect_ptr = context.builder.template get_builder<formalism::datalog::NumericEffect<DOp, FluentTag>>();
    auto& numeric_effect = *numeric_effect_ptr;
    numeric_effect.clear();

    numeric_effect.fterm = merge_p2d(element.get_fterm(), context).first.get_index();
    numeric_effect.fexpr = merge_p2d(element.get_fexpr(), context);

    canonicalize(numeric_effect);
    return context.destination.get_or_create(numeric_effect);
}

TYR_INLINE_IMPL std::pair<formalism::datalog::NumericEffectView<formalism::datalog::OpAssign, FluentTag>, bool>
merge_p2d(NumericEffectView<OpAssign, FluentTag> element, MergeDatalogContext& context)
{
    return merge_numeric_effect_as<formalism::datalog::OpAssign>(element, context);
}

TYR_INLINE_IMPL std::pair<formalism::datalog::NumericEffectView<formalism::datalog::OpIncrease, FluentTag>, bool>
merge_p2d(NumericEffectView<OpIncrease, FluentTag> element, MergeDatalogContext& context)
{
    return merge_numeric_effect_as<formalism::datalog::OpIncrease>(element, context);
}

TYR_INLINE_IMPL std::pair<formalism::datalog::NumericEffectView<formalism::datalog::OpDecrease, FluentTag>, bool>
merge_p2d(NumericEffectView<OpDecrease, FluentTag> element, MergeDatalogContext& context)
{
    return merge_numeric_effect_as<formalism::datalog::OpDecrease>(element, context);
}

TYR_INLINE_IMPL std::pair<formalism::datalog::NumericEffectView<formalism::datalog::OpScaleUp, FluentTag>, bool>
merge_p2d(NumericEffectView<OpScaleUp, FluentTag> element, MergeDatalogContext& context)
{
    return merge_numeric_effect_as<formalism::datalog::OpScaleUp>(element, context);
}

TYR_INLINE_IMPL std::pair<formalism::datalog::NumericEffectView<formalism::datalog::OpScaleDown, FluentTag>, bool>
merge_p2d(NumericEffectView<OpScaleDown, FluentTag> element, MergeDatalogContext& context)
{
    return merge_numeric_effect_as<formalism::datalog::OpScaleDown>(element, context);
}

TYR_INLINE_IMPL Data<formalism::datalog::NumericEffectOperator<FluentTag>> merge_p2d(NumericEffectOperatorView<FluentTag> element,
                                                                                      MergeDatalogContext& context)
{
    using OperatorData = Data<formalism::datalog::NumericEffectOperator<FluentTag>>;

    return visit([&](auto&& arg) { return OperatorData(typename OperatorData::Variant(merge_p2d(arg, context).first.get_index())); },
                 element.get_variant());
}

TYR_INLINE_IMPL Data<formalism::datalog::FunctionExpression> merge_p2d(FunctionExpressionView element, MergeDatalogContext& context)
{
    return visit(
        [&](auto&& arg) -> Data<formalism::datalog::FunctionExpression>
        {
            using Alternative = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<Alternative, float_t>)
                return Data<formalism::datalog::FunctionExpression>(arg);
            else if constexpr (std::is_same_v<Alternative, LiftedArithmeticOperatorView>)
                return Data<formalism::datalog::FunctionExpression>(merge_p2d(arg, context));
            else if constexpr (std::is_same_v<Alternative, FunctionTermView<AuxiliaryTag>>)
                throw std::logic_error("AuxiliaryTag FunctionTerm must not be merged.");
            else
                return Data<formalism::datalog::FunctionExpression>(merge_p2d(arg, context).first.get_index());
        },
        element.get_variant());
}

TYR_INLINE_IMPL Data<formalism::datalog::GroundFunctionExpression> merge_p2d(GroundFunctionExpressionView element, MergeDatalogContext& context)
{
    return visit(
        [&](auto&& arg) -> Data<formalism::datalog::GroundFunctionExpression>
        {
            using Alternative = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<Alternative, float_t>)
                return Data<formalism::datalog::GroundFunctionExpression>(arg);
            else if constexpr (std::is_same_v<Alternative, GroundArithmeticOperatorView>)
                return Data<formalism::datalog::GroundFunctionExpression>(merge_p2d(arg, context));
            else if constexpr (std::is_same_v<Alternative, GroundFunctionTermView<AuxiliaryTag>>)
                throw std::logic_error("AuxiliaryTag GroundFunctionTerm must not be merged.");
            else
                return Data<formalism::datalog::GroundFunctionExpression>(merge_p2d(arg, context).first.get_index());
        },
        element.get_variant());
}

template<OpKind O, typename T>
std::pair<formalism::datalog::UnaryOperatorView<O, to_datalog_payload_t<T>>, bool> merge_p2d(UnaryOperatorView<O, T> element, MergeDatalogContext& context)
{
    using T_DST = to_datalog_payload_t<T>;

    auto unary_ptr = context.builder.template get_builder<formalism::datalog::UnaryOperator<O, T_DST>>();
    auto& unary = *unary_ptr;
    unary.clear();

    unary.arg = merge_p2d(element.get_arg(), context);

    canonicalize(unary);
    return context.destination.get_or_create(unary);
}

template<OpKind O, typename T>
std::pair<formalism::datalog::BinaryOperatorView<O, to_datalog_payload_t<T>>, bool> merge_p2d(BinaryOperatorView<O, T> element, MergeDatalogContext& context)
{
    using T_DST = to_datalog_payload_t<T>;

    auto binary_ptr = context.builder.template get_builder<formalism::datalog::BinaryOperator<O, T_DST>>();
    auto& binary = *binary_ptr;
    binary.clear();

    binary.lhs = merge_p2d(element.get_lhs(), context);
    binary.rhs = merge_p2d(element.get_rhs(), context);

    canonicalize(binary);
    return context.destination.get_or_create(binary);
}

template<OpKind O, typename T>
std::pair<formalism::datalog::MultiOperatorView<O, to_datalog_payload_t<T>>, bool> merge_p2d(MultiOperatorView<O, T> element, MergeDatalogContext& context)
{
    using T_DST = to_datalog_payload_t<T>;

    auto multi_ptr = context.builder.template get_builder<formalism::datalog::MultiOperator<O, T_DST>>();
    auto& multi = *multi_ptr;
    multi.clear();

    for (const auto arg : element.get_args())
        multi.args.push_back(merge_p2d(arg, context));

    canonicalize(multi);
    return context.destination.get_or_create(multi);
}

template<typename T>
Data<formalism::datalog::ArithmeticOperator<to_datalog_payload_t<T>>> merge_p2d(ArithmeticOperatorView<T> element, MergeDatalogContext& context)
{
    using T_DST = to_datalog_payload_t<T>;

    return visit([&](auto&& arg) { return Data<formalism::datalog::ArithmeticOperator<T_DST>>(merge_p2d(arg, context).first.get_index()); },
                 element.get_variant());
}

template<typename T>
Data<formalism::datalog::BooleanOperator<to_datalog_payload_t<T>>> merge_p2d(BooleanOperatorView<T> element, MergeDatalogContext& context)
{
    using T_DST = to_datalog_payload_t<T>;

    return visit([&](auto&& arg) { return Data<formalism::datalog::BooleanOperator<T_DST>>(merge_p2d(arg, context).first.get_index()); },
                 element.get_variant());
}

}
