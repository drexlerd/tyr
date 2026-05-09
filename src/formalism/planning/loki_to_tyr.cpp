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

#include "loki_to_tyr.hpp"

#include "tyr/planning/lifted_task.hpp"

namespace tyr::formalism::planning
{

/**
 * Prepare common
 */
void LokiToTyrTranslator::prepare(loki::FunctionSkeleton function_skeleton)
{
    prepare(function_skeleton->get_parameters());
    prepare(function_skeleton->get_type());
}
void LokiToTyrTranslator::prepare(loki::Object object) { prepare(object->get_bases()); }
void LokiToTyrTranslator::prepare(loki::Parameter parameter) { prepare(parameter->get_variable()); }
void LokiToTyrTranslator::prepare(loki::Predicate predicate) { prepare(predicate->get_parameters()); }
void LokiToTyrTranslator::prepare(loki::Requirements requirements) {}
void LokiToTyrTranslator::prepare(loki::Type type) { prepare(type->get_bases()); }
void LokiToTyrTranslator::prepare(loki::Variable variabl) {}

/**
 * Prepare lifted
 */

void LokiToTyrTranslator::prepare(loki::Term term)
{
    std::visit([&](auto&& arg) { return this->prepare(arg); }, term->get_object_or_variable());
}

void LokiToTyrTranslator::prepare(loki::Atom atom)
{
    prepare(atom->get_predicate());
    prepare(atom->get_terms());
}
void LokiToTyrTranslator::prepare(loki::Literal literal) { prepare(literal->get_atom()); }
void LokiToTyrTranslator::prepare(loki::FunctionExpressionNumber function_expression) {}
void LokiToTyrTranslator::prepare(loki::FunctionExpressionBinaryOperator function_expression)
{
    prepare(function_expression->get_left_function_expression());
    prepare(function_expression->get_right_function_expression());
}
void LokiToTyrTranslator::prepare(loki::FunctionExpressionMultiOperator function_expression) { this->prepare(function_expression->get_function_expressions()); }
void LokiToTyrTranslator::prepare(loki::FunctionExpressionMinus function_expression) { this->prepare(function_expression->get_function_expression()); }
void LokiToTyrTranslator::prepare(loki::FunctionExpressionFunction function_expression)
{
    m_fexpr_functions.insert(function_expression->get_function()->get_function_skeleton()->get_name());

    this->prepare(function_expression->get_function());
}
void LokiToTyrTranslator::prepare(loki::FunctionExpression function_expression)
{
    std::visit([&](auto&& arg) { return this->prepare(arg); }, function_expression->get_function_expression());
}
void LokiToTyrTranslator::prepare(loki::Function function)
{
    prepare(function->get_function_skeleton());
    prepare(function->get_terms());
}
void LokiToTyrTranslator::prepare(loki::Condition condition)
{
    if (const auto condition_and = std::get_if<loki::ConditionAnd>(&condition->get_condition()))
    {
        for (const auto& part : (*condition_and)->get_conditions())
        {
            if (const auto condition_literal = std::get_if<loki::ConditionLiteral>(&part->get_condition()))
            {
                prepare((*condition_literal)->get_literal());
            }
            else if (const auto condition_numeric = std::get_if<loki::ConditionNumericConstraint>(&part->get_condition()))
            {
                prepare((*condition_numeric)->get_left_function_expression());
                prepare((*condition_numeric)->get_right_function_expression());
            }
            else
            {
                // std::visit([](auto&& arg) { std::cout << arg << std::endl; }, part->get_condition());

                throw std::logic_error("Expected literal in conjunctive condition.");
            }
        }
    }
    else if (const auto condition_literal = std::get_if<loki::ConditionLiteral>(&condition->get_condition()))
    {
        prepare((*condition_literal)->get_literal());
    }
    else
    {
        // std::cout << std::visit([](auto&& arg) { return arg.str(); }, *condition_ptr) << std::endl;

        throw std::logic_error("Expected conjunctive condition.");
    }
}

void LokiToTyrTranslator::prepare(loki::Effect effect)
{
    const auto prepare_effect_func = [&](loki::Effect arg_effect)
    {
        auto tmp_effect = arg_effect;

        // 2. Prepare universal part
        if (const auto& tmp_effect_forall = std::get_if<loki::EffectCompositeForall>(&tmp_effect->get_effect()))
        {
            prepare((*tmp_effect_forall)->get_parameters());

            tmp_effect = (*tmp_effect_forall)->get_effect();
        }

        // 3. Prepare conditional part
        if (const auto& tmp_effect_when = std::get_if<loki::EffectCompositeWhen>(&tmp_effect->get_effect()))
        {
            if (const auto condition_and = std::get_if<loki::ConditionAnd>(&(*tmp_effect_when)->get_condition()->get_condition()))
            {
                for (const auto& part : (*condition_and)->get_conditions())
                {
                    if (const auto condition_literal = std::get_if<loki::ConditionLiteral>(&part->get_condition()))
                    {
                        prepare((*condition_literal)->get_literal());
                    }
                    else
                    {
                        // std::cout << std::visit([](auto&& arg) { return arg.str(); }, *part) << std::endl;

                        throw std::logic_error("Expected literal in conjunctive condition.");
                    }
                }
            }
            else if (const auto condition_literal = std::get_if<loki::ConditionLiteral>(&(*tmp_effect_when)->get_condition()->get_condition()))
            {
                prepare((*condition_literal)->get_literal());
            }

            tmp_effect = (*tmp_effect_when)->get_effect();
        }

        // 4. Parse simple effect
        if (const auto& effect_literal = std::get_if<loki::EffectLiteral>(&tmp_effect->get_effect()))
        {
            prepare((*effect_literal)->get_literal());

            // Found predicate affected by an effect
            m_fluent_predicates.insert((*effect_literal)->get_literal()->get_atom()->get_predicate()->get_name());
        }
        else if (const auto& effect_numeric = std::get_if<loki::EffectNumeric>(&tmp_effect->get_effect()))
        {
            // Found function affected by an effect
            m_effect_function_skeletons.insert((*effect_numeric)->get_function()->get_function_skeleton()->get_name());

            prepare((*effect_numeric)->get_function_expression());
        }
        else
        {
            // std::cout << std::visit([](auto&& arg) { return arg.str(); }, *tmp_effect) << std::endl;

            throw std::logic_error("Expected simple effect.");
        }
    };

    // 1. Prepare conjunctive part
    if (const auto& effect_and = std::get_if<loki::EffectAnd>(&effect->get_effect()))
    {
        for (const auto& nested_effect : (*effect_and)->get_effects())
        {
            prepare_effect_func(nested_effect);
        }
    }
    else
    {
        prepare_effect_func(effect);
    }
}
void LokiToTyrTranslator::prepare(loki::Action action)
{
    prepare(action->get_parameters());
    prepare(action->get_condition());
    prepare(action->get_effect());
}
void LokiToTyrTranslator::prepare(loki::Axiom axiom)
{
    prepare(axiom->get_parameters());
    prepare(axiom->get_literal());
    prepare(axiom->get_condition());

    m_derived_predicates.insert(axiom->get_literal()->get_atom()->get_predicate()->get_name());
}
void LokiToTyrTranslator::prepare(loki::FunctionValue function_value) { prepare(function_value->get_function()); }
void LokiToTyrTranslator::prepare(loki::OptimizationMetric metric) { prepare(metric->get_function_expression()); }

void LokiToTyrTranslator::prepare(loki::Domain domain)
{
    prepare(domain->get_requirements());
    prepare(domain->get_types());
    prepare(domain->get_constants());
    prepare(domain->get_predicates());
    prepare(domain->get_function_skeletons());
    prepare(domain->get_actions());
    prepare(domain->get_axioms());
}

void LokiToTyrTranslator::prepare(loki::Problem problem)
{
    prepare(problem->get_domain());
    prepare(problem->get_requirements());
    prepare(problem->get_objects());
    prepare(problem->get_predicates());
    prepare(problem->get_initial_literals());
    prepare(problem->get_initial_function_values());
    prepare(problem->get_goal_condition());
    prepare(problem->get_optimization_metric());
    prepare(problem->get_axioms());

    for (const auto& derived_predicate : problem->get_predicates())
    {
        m_derived_predicates.insert(derived_predicate->get_name());
    }
}

/**
 * Common translations.
 */

void LokiToTyrTranslator::ParameterIndexMapping::push_parameters(const IndexList<Variable>& parameters)
{
    for (const auto parameter : parameters)
        map.emplace(parameter, map.size());
}

void LokiToTyrTranslator::ParameterIndexMapping::pop_parameters(const IndexList<Variable>& parameters)
{
    for (const auto parameter : parameters)
        map.erase(parameter);
}

ParameterIndex LokiToTyrTranslator::ParameterIndexMapping::lookup_parameter_index(Index<Variable> variable) { return map.at(variable); }

FunctionViewVariant LokiToTyrTranslator::translate_common(loki::FunctionSkeleton element, Builder& builder, Repository& context)
{
    auto build_function = [&](auto fact_tag) -> FunctionViewVariant
    {
        using Tag = std::decay_t<decltype(fact_tag)>;

        auto function_ptr = builder.template get_builder<Function<Tag>>();
        auto& function = *function_ptr;
        function.clear();
        function.name = element->get_name();
        function.arity = element->get_parameters().size();
        canonicalize(function);
        return context.get_or_create(function).first;
    };

    if (element->get_name() == "total-cost")
        return build_function(AuxiliaryTag {});
    else if (m_effect_function_skeletons.contains(element->get_name()))
        return build_function(FluentTag {});
    else
        return build_function(StaticTag {});
}

Index<Object> LokiToTyrTranslator::translate_common(loki::Object element, Builder& builder, Repository& context)
{
    auto object_ptr = builder.template get_builder<Object>();
    auto& object = *object_ptr;
    object.clear();
    object.name = element->get_name();
    canonicalize(object);
    return context.get_or_create(object).first.get_index();
}

Index<Variable> LokiToTyrTranslator::translate_common(loki::Parameter element, Builder& builder, Repository& context)
{
    return translate_common(element->get_variable(), builder, context);
}

PredicateViewVariant LokiToTyrTranslator::translate_common(loki::Predicate element, Builder& builder, Repository& context)
{
    auto build_predicate = [&](auto fact_tag) -> PredicateViewVariant
    {
        using Tag = std::decay_t<decltype(fact_tag)>;

        auto predicate_ptr = builder.template get_builder<Predicate<Tag>>();
        auto& predicate = *predicate_ptr;
        predicate.clear();
        predicate.name = element->get_name();
        predicate.arity = element->get_parameters().size();
        canonicalize(predicate);
        return context.get_or_create(predicate).first;
    };

    if (m_fluent_predicates.count(element->get_name()) && !m_derived_predicates.count(element->get_name()))
        return build_predicate(FluentTag {});
    else if (m_derived_predicates.count(element->get_name()))
        return build_predicate(DerivedTag {});
    else
        return build_predicate(StaticTag {});
}

Index<Variable> LokiToTyrTranslator::translate_common(loki::Variable element, Builder& builder, Repository& context)
{
    auto variable_ptr = builder.template get_builder<Variable>();
    auto& variable = *variable_ptr;
    variable.clear();
    variable.name = element->get_name();
    canonicalize(variable);
    return context.get_or_create(variable).first.get_index();
}

namespace
{

template<typename T>
auto to_binding(View<Index<T>, Repository> element, const IndexList<Object>& objects, Repository& context)
{
    return context.get_or_create(Data<RelationBinding<T>>(element.get_index(), element.get_arity(), objects));
}

}

/**
 * Lifted translation.
 */
Data<Term> LokiToTyrTranslator::translate_lifted(loki::Term element, Builder& builder, Repository& context)
{
    return std::visit(
        [&](auto&& arg) -> Data<Term>
        {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, loki::Object>)
                return Data<Term>(translate_common(arg, builder, context));
            else if constexpr (std::is_same_v<T, loki::Variable>)
                return Data<Term>(m_param_map.lookup_parameter_index(translate_common(arg, builder, context)));
            else
                static_assert(dependent_false<T>::value, "Missing case for type");
        },
        element->get_object_or_variable());
}

AtomViewVariant LokiToTyrTranslator::translate_lifted(loki::Atom element, Builder& builder, Repository& context)
{
    auto predicate_view_variant = translate_common(element->get_predicate(), builder, context);

    auto build_atom = [&](auto fact_tag, auto predicate) -> AtomViewVariant
    {
        using Tag = std::decay_t<decltype(fact_tag)>;

        auto atom_ptr = builder.template get_builder<Atom<Tag>>();
        auto& atom = *atom_ptr;
        atom.clear();
        atom.predicate = predicate.get_index();
        atom.terms = this->translate_lifted(element->get_terms(), builder, context);
        canonicalize(atom);
        return context.get_or_create(atom).first;
    };

    return std::visit(
        [&](auto&& arg) -> AtomViewVariant
        {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, PredicateView<StaticTag>>)
                return build_atom(StaticTag {}, arg);
            else if constexpr (std::is_same_v<T, PredicateView<FluentTag>>)
                return build_atom(FluentTag {}, arg);
            else if constexpr (std::is_same_v<T, PredicateView<DerivedTag>>)
                return build_atom(DerivedTag {}, arg);
            else
                static_assert(dependent_false<T>::value, "Missing case for type");
        },
        predicate_view_variant);
}

LiteralViewVariant LokiToTyrTranslator::translate_lifted(loki::Literal element, Builder& builder, Repository& context)
{
    auto atom_view_variant = translate_lifted(element->get_atom(), builder, context);

    auto build_literal = [&](auto fact_tag, auto atom) -> LiteralViewVariant
    {
        using Tag = std::decay_t<decltype(fact_tag)>;

        auto literal_ptr = builder.template get_builder<Literal<Tag>>();
        auto& literal = *literal_ptr;
        literal.clear();
        literal.atom = atom.get_index();
        literal.polarity = element->get_polarity();
        canonicalize(literal);
        return context.get_or_create(literal).first;
    };

    return std::visit(
        [&](auto&& arg) -> LiteralViewVariant
        {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, AtomView<StaticTag>>)
                return build_literal(StaticTag {}, arg);
            else if constexpr (std::is_same_v<T, AtomView<FluentTag>>)
                return build_literal(FluentTag {}, arg);
            else if constexpr (std::is_same_v<T, AtomView<DerivedTag>>)
                return build_literal(DerivedTag {}, arg);
            else
                static_assert(dependent_false<T>::value, "Missing case for type");
        },
        atom_view_variant);
}

Data<FunctionExpression> LokiToTyrTranslator::translate_lifted(loki::FunctionExpressionNumber element, Builder& builder, Repository& context)
{
    return Data<FunctionExpression>(float_t(element->get_number()));
}

Data<FunctionExpression> LokiToTyrTranslator::translate_lifted(loki::FunctionExpressionBinaryOperator element, Builder& builder, Repository& context)
{
    auto build_binary_op = [&](auto op_tag) -> Data<FunctionExpression>
    {
        using Tag = std::decay_t<decltype(op_tag)>;

        auto binary_ptr = builder.template get_builder<BinaryOperator<Tag, Data<FunctionExpression>>>();
        auto& binary = *binary_ptr;
        binary.clear();
        binary.lhs = translate_lifted(element->get_left_function_expression(), builder, context);
        binary.rhs = translate_lifted(element->get_right_function_expression(), builder, context);
        canonicalize(binary);
        return Data<FunctionExpression>(Data<ArithmeticOperator<Data<FunctionExpression>>>(context.get_or_create(binary).first.get_index()));
    };

    switch (element->get_binary_operator())
    {
        case loki::BinaryOperatorEnum::PLUS:
            return build_binary_op(Add {});
        case loki::BinaryOperatorEnum::MINUS:
            return build_binary_op(Sub {});
        case loki::BinaryOperatorEnum::MUL:
            return build_binary_op(Mul {});
        case loki::BinaryOperatorEnum::DIV:
            return build_binary_op(Div {});
        default:
            throw std::runtime_error("Unexpected case");
    }
}

Data<FunctionExpression> LokiToTyrTranslator::translate_lifted(loki::FunctionExpressionMultiOperator element, Builder& builder, Repository& context)
{
    auto build_multi_op = [&](auto op_tag) -> Data<FunctionExpression>
    {
        using Tag = std::decay_t<decltype(op_tag)>;

        auto multi_ptr = builder.template get_builder<MultiOperator<Tag, Data<FunctionExpression>>>();
        auto& multi = *multi_ptr;
        multi.clear();
        multi.args = translate_lifted(element->get_function_expressions(), builder, context);
        canonicalize(multi);
        return Data<FunctionExpression>(Data<ArithmeticOperator<Data<FunctionExpression>>>(context.get_or_create(multi).first.get_index()));
    };

    switch (element->get_multi_operator())
    {
        case loki::MultiOperatorEnum::PLUS:
            return build_multi_op(Add {});
        case loki::MultiOperatorEnum::MUL:
            return build_multi_op(Mul {});
        default:
            throw std::runtime_error("Unexpected case");
    }
}

Data<FunctionExpression> LokiToTyrTranslator::translate_lifted(loki::FunctionExpressionMinus element, Builder& builder, Repository& context)
{
    auto minus_ptr = builder.template get_builder<UnaryOperator<Sub, Data<FunctionExpression>>>();
    auto& minus = *minus_ptr;
    minus.clear();
    minus.arg = translate_lifted(element->get_function_expression(), builder, context);
    canonicalize(minus);
    return Data<FunctionExpression>(Data<ArithmeticOperator<Data<FunctionExpression>>>(context.get_or_create(minus).first.get_index()));
}

Data<FunctionExpression> LokiToTyrTranslator::translate_lifted(loki::FunctionExpressionFunction element, Builder& builder, Repository& context)
{
    const auto fterm_view_variant = translate_lifted(element->get_function(), builder, context);

    return std::visit(
        [&](auto&& arg) -> Data<FunctionExpression>
        {
            using T = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<T, FunctionTermView<StaticTag>>)
                return Data<FunctionExpression>(arg.get_index());
            else if constexpr (std::is_same_v<T, FunctionTermView<FluentTag>>)
                return Data<FunctionExpression>(arg.get_index());
            else if constexpr (std::is_same_v<T, FunctionTermView<AuxiliaryTag>>)
                throw std::runtime_error("Cannot create FunctionExpression over auxiliary function term.");
            else
                static_assert(dependent_false<T>::value, "Missing case for type");
        },
        fterm_view_variant);
}

Data<FunctionExpression> LokiToTyrTranslator::translate_lifted(loki::FunctionExpression element, Builder& builder, Repository& context)
{
    return std::visit([&](auto&& arg) { return translate_lifted(arg, builder, context); }, element->get_function_expression());
}

FunctionTermViewVariant LokiToTyrTranslator::translate_lifted(loki::Function element, Builder& builder, Repository& context)
{
    auto function_view_variant = translate_common(element->get_function_skeleton(), builder, context);

    auto build_function_term = [&](auto fact_tag, auto function) -> FunctionTermViewVariant
    {
        using Tag = std::decay_t<decltype(fact_tag)>;

        auto fterm_ptr = builder.template get_builder<FunctionTerm<Tag>>();
        auto& fterm = *fterm_ptr;
        fterm.clear();
        fterm.function = function.get_index();
        fterm.terms = this->translate_lifted(element->get_terms(), builder, context);
        canonicalize(fterm);
        return context.get_or_create(fterm).first;
    };

    return std::visit(
        [&](auto&& arg) -> FunctionTermViewVariant
        {
            using T = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<T, FunctionView<StaticTag>>)
                return build_function_term(StaticTag {}, arg);
            else if constexpr (std::is_same_v<T, FunctionView<FluentTag>>)
                return build_function_term(FluentTag {}, arg);
            else if constexpr (std::is_same_v<T, FunctionView<AuxiliaryTag>>)
                return build_function_term(AuxiliaryTag {}, arg);
            else
                static_assert(dependent_false<T>::value, "Missing case for type");
        },
        function_view_variant);
}

Data<BooleanOperator<Data<FunctionExpression>>>
LokiToTyrTranslator::translate_lifted(loki::ConditionNumericConstraint element, Builder& builder, Repository& context)
{
    auto build_binary_op = [&](auto op_tag) -> Data<BooleanOperator<Data<FunctionExpression>>>
    {
        using Tag = std::decay_t<decltype(op_tag)>;

        auto binary_ptr = builder.template get_builder<BinaryOperator<Tag, Data<FunctionExpression>>>();
        auto& binary = *binary_ptr;
        binary.clear();
        binary.lhs = translate_lifted(element->get_left_function_expression(), builder, context);
        binary.rhs = translate_lifted(element->get_right_function_expression(), builder, context);
        canonicalize(binary);
        return Data<BooleanOperator<Data<FunctionExpression>>>(context.get_or_create(binary).first.get_index());
    };

    switch (element->get_binary_comparator())
    {
        case loki::BinaryComparatorEnum::EQUAL:
            return build_binary_op(Eq {});
        case loki::BinaryComparatorEnum::UNEQUAL:
            return build_binary_op(Ne {});
        case loki::BinaryComparatorEnum::LESS_EQUAL:
            return build_binary_op(Le {});
        case loki::BinaryComparatorEnum::LESS:
            return build_binary_op(Lt {});
        case loki::BinaryComparatorEnum::GREATER_EQUAL:
            return build_binary_op(Ge {});
        case loki::BinaryComparatorEnum::GREATER:
            return build_binary_op(Gt {});
        default:
            throw std::runtime_error("Unexpected case");
    }
}

Index<ConjunctiveCondition>
LokiToTyrTranslator::translate_lifted(loki::Condition element, const IndexList<Variable>& parameters, Builder& builder, Repository& context)
{
    auto conj_condition_ptr = builder.template get_builder<ConjunctiveCondition>();
    auto& conj_condition = *conj_condition_ptr;
    conj_condition.clear();

    conj_condition.variables = parameters;

    const auto func_insert_literal = [](LiteralViewVariant literal_view_variant,
                                        IndexList<Literal<StaticTag>>& static_literals,
                                        IndexList<Literal<FluentTag>>& fluent_literals,
                                        IndexList<Literal<DerivedTag>>& derived_literals)
    {
        std::visit(
            [&](auto&& arg)
            {
                using T = std::decay_t<decltype(arg)>;

                if constexpr (std::is_same_v<T, LiteralView<StaticTag>>)
                    static_literals.push_back(arg.get_index());
                else if constexpr (std::is_same_v<T, LiteralView<FluentTag>>)
                    fluent_literals.push_back(arg.get_index());
                else if constexpr (std::is_same_v<T, LiteralView<DerivedTag>>)
                    derived_literals.push_back(arg.get_index());
                else
                    static_assert(dependent_false<T>::value, "Missing case for type");
            },
            literal_view_variant);
    };

    return std::visit(
        [&](auto&& condition) -> Index<ConjunctiveCondition>
        {
            using ConditionT = std::decay_t<decltype(condition)>;

            if constexpr (std::is_same_v<ConditionT, loki::ConditionAnd>)
            {
                for (const auto& part : condition->get_conditions())
                {
                    std::visit(
                        [&](auto&& subcondition)
                        {
                            using SubConditionT = std::decay_t<decltype(subcondition)>;

                            if constexpr (std::is_same_v<SubConditionT, loki::ConditionLiteral>)
                            {
                                const auto literal_view_variant = translate_lifted(subcondition->get_literal(), builder, context);

                                func_insert_literal(literal_view_variant,
                                                    conj_condition.static_literals,
                                                    conj_condition.fluent_literals,
                                                    conj_condition.derived_literals);
                            }
                            else if constexpr (std::is_same_v<SubConditionT, loki::ConditionNumericConstraint>)
                            {
                                const auto numeric_constraint = translate_lifted(subcondition, builder, context);

                                conj_condition.numeric_constraints.push_back(numeric_constraint);
                            }
                            else
                            {
                                // std::cout << std::visit([](auto&& arg) { return arg.str(); }, *part) << std::endl;
                                throw std::logic_error("Unexpected condition.");
                            }
                        },
                        part->get_condition());
                }

                canonicalize(conj_condition);
                return context.get_or_create(conj_condition).first.get_index();
            }
            else if constexpr (std::is_same_v<ConditionT, loki::ConditionLiteral>)
            {
                const auto literal_view_variant = translate_lifted(condition->get_literal(), builder, context);

                func_insert_literal(literal_view_variant, conj_condition.static_literals, conj_condition.fluent_literals, conj_condition.derived_literals);

                canonicalize(conj_condition);
                return context.get_or_create(conj_condition).first.get_index();
            }
            else if constexpr (std::is_same_v<ConditionT, loki::ConditionNumericConstraint>)
            {
                const auto numeric_constraint = translate_lifted(condition, builder, context);

                conj_condition.numeric_constraints.push_back(numeric_constraint);

                canonicalize(conj_condition);
                return context.get_or_create(conj_condition).first.get_index();
            }
            else
            {
                // std::cout << std::visit([](auto&& arg) { return arg.str(); }, *part) << std::endl;
                throw std::logic_error("Unexpected condition.");
            }
        },
        element->get_condition());
}

NumericEffectViewVariant LokiToTyrTranslator::translate_lifted(loki::EffectNumeric element, Builder& builder, Repository& context)
{
    auto fterm_view_variant = translate_lifted(element->get_function(), builder, context);

    auto build_numeric_effect_term_helper = [&](auto fact_tag, auto op_tag, auto fterm) -> NumericEffectViewVariant
    {
        using Tag = std::decay_t<decltype(fact_tag)>;
        using Op = std::decay_t<decltype(op_tag)>;

        auto numeric_effect_ptr = builder.template get_builder<NumericEffect<Op, Tag>>();
        auto& numeric_effect = *numeric_effect_ptr;
        numeric_effect.clear();

        numeric_effect.fterm = fterm.get_index();
        numeric_effect.fexpr = this->translate_lifted(element->get_function_expression(), builder, context);
        canonicalize(numeric_effect);
        return context.get_or_create(numeric_effect).first;
    };

    auto build_numeric_effect_term = [&](auto fact_tag, auto fterm) -> NumericEffectViewVariant
    {
        using Tag = std::decay_t<decltype(fact_tag)>;

        if constexpr (std::is_same_v<Tag, AuxiliaryTag>)
        {
            if (element->get_assign_operator() != loki::AssignOperatorEnum::INCREASE)
                throw std::runtime_error("Auxiliary numeric effect must use INCREASE operator.");

            return build_numeric_effect_term_helper(Tag {}, Increase {}, fterm);
        }
        else
        {
            switch (element->get_assign_operator())
            {
                case loki::AssignOperatorEnum::ASSIGN:
                    return build_numeric_effect_term_helper(Tag {}, Assign {}, fterm);
                case loki::AssignOperatorEnum::INCREASE:
                    return build_numeric_effect_term_helper(Tag {}, Increase {}, fterm);
                case loki::AssignOperatorEnum::DECREASE:
                    return build_numeric_effect_term_helper(Tag {}, Decrease {}, fterm);
                case loki::AssignOperatorEnum::SCALE_UP:
                    return build_numeric_effect_term_helper(Tag {}, ScaleUp {}, fterm);
                case loki::AssignOperatorEnum::SCALE_DOWN:
                    return build_numeric_effect_term_helper(Tag {}, ScaleDown {}, fterm);
                default:
                    throw std::runtime_error("Unexpected case.");
            }
        }
    };

    return std::visit(
        [&](auto&& arg) -> NumericEffectViewVariant
        {
            using T = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<T, FunctionTermView<StaticTag>>)
                throw std::runtime_error("Cannot create NumericEffect over static function term.");
            else if constexpr (std::is_same_v<T, FunctionTermView<FluentTag>>)
                return build_numeric_effect_term(FluentTag {}, arg);
            else if constexpr (std::is_same_v<T, FunctionTermView<AuxiliaryTag>>)
                return build_numeric_effect_term(AuxiliaryTag {}, arg);
            else
                static_assert(dependent_false<T>::value, "Missing case for type");
        },
        fterm_view_variant);
}

IndexList<ConditionalEffect>
LokiToTyrTranslator::translate_lifted(loki::Effect element, const IndexList<Variable>& parameters, Builder& builder, Repository& context)
{
    using ConditionalEffectData = UnorderedMap<Index<ConjunctiveCondition>,
                                               std::tuple<IndexList<Variable>,
                                                          IndexList<Literal<FluentTag>>,
                                                          DataList<NumericEffectOperator<FluentTag>>,
                                                          ::cista::optional<Data<NumericEffectOperator<AuxiliaryTag>>>>>;

    const auto translate_effect_func = [&](loki::Effect effect, ConditionalEffectData& ref_conditional_effect_data)
    {
        auto tmp_effect = effect;

        auto universal_parameters = IndexList<Variable> {};

        /* 1. Parse universal part. */

        std::visit(
            [&](auto&& subeffect)
            {
                using SubEffectT = std::decay_t<decltype(subeffect)>;

                if constexpr (std::is_same_v<SubEffectT, loki::EffectCompositeForall>)
                {
                    universal_parameters = translate_common(subeffect->get_parameters(), builder, context);

                    tmp_effect = subeffect->get_effect();
                }
            },
            tmp_effect->get_effect());

        ///---------- Push parameters and parse scope -------------
        m_param_map.push_parameters(universal_parameters);
        {
            /* 2. Parse conditional part */
            auto conjunctive_condition = std::visit(
                [&](auto&& subeffect)
                {
                    using SubEffectT = std::decay_t<decltype(subeffect)>;

                    auto all_parameters = parameters;
                    all_parameters.insert(all_parameters.end(), universal_parameters.begin(), universal_parameters.end());

                    if constexpr (std::is_same_v<SubEffectT, loki::EffectCompositeWhen>)
                    {
                        auto conjunctive_condition = translate_lifted(subeffect->get_condition(), all_parameters, builder, context);

                        tmp_effect = subeffect->get_effect();

                        return conjunctive_condition;
                    }
                    else
                    {
                        // Create empty conjunctive condition for unconditional effects
                        auto conj_cond_ptr = builder.template get_builder<ConjunctiveCondition>();
                        auto& conj_cond = *conj_cond_ptr;
                        conj_cond.clear();
                        canonicalize(conj_cond);
                        return context.get_or_create(conj_cond).first.get_index();
                    }
                },
                tmp_effect->get_effect());

            // Fetch container to store the effects
            auto& effect_data = ref_conditional_effect_data[conjunctive_condition];
            auto& stored_universal = std::get<0>(effect_data);
            if (stored_universal.empty())
                stored_universal = universal_parameters;
            else
                assert(stored_universal.size() == universal_parameters.size() && "Same guard but different forall-scope.");
            auto& data_fluent_literals = std::get<1>(effect_data);
            auto& data_fluent_numeric_effects = std::get<2>(effect_data);
            auto& data_auxiliary_numeric_effect = std::get<3>(effect_data);

            /* 3. Parse effect part */
            std::visit(
                [&](auto&& subeffect)
                {
                    using SubEffectT = std::decay_t<decltype(subeffect)>;

                    if constexpr (std::is_same_v<SubEffectT, loki::EffectLiteral>)
                    {
                        const auto literal_view_variant = translate_lifted(subeffect->get_literal(), builder, context);

                        std::visit(
                            [&](auto&& subsubeffect)
                            {
                                using SubSubEffectT = std::decay_t<decltype(subsubeffect)>;

                                if constexpr (std::is_same_v<SubSubEffectT, LiteralView<StaticTag>>)
                                    throw std::logic_error("Effect literal cannot be Static!");
                                else if constexpr (std::is_same_v<SubSubEffectT, LiteralView<FluentTag>>)
                                    data_fluent_literals.push_back(subsubeffect.get_index());
                                else if constexpr (std::is_same_v<SubSubEffectT, LiteralView<DerivedTag>>)
                                    throw std::runtime_error("Effect literal cannot be Derived!");
                                else
                                    static_assert(dependent_false<SubSubEffectT>::value, "Unexpected case.");
                            },
                            literal_view_variant);
                    }
                    else if constexpr (std::is_same_v<SubEffectT, loki::EffectNumeric>)
                    {
                        const auto numeric_effect_view_variant = translate_lifted(subeffect, builder, context);

                        std::visit(
                            [&](auto&& subsubeffect)
                            {
                                using SubSubEffectT = std::decay_t<decltype(subsubeffect)>;

                                if constexpr (std::is_same_v<SubSubEffectT, NumericEffectView<Assign, FluentTag>>)
                                    data_fluent_numeric_effects.push_back(Data<NumericEffectOperator<FluentTag>>(subsubeffect.get_index()));
                                else if constexpr (std::is_same_v<SubSubEffectT, NumericEffectView<Increase, FluentTag>>)
                                    data_fluent_numeric_effects.push_back(Data<NumericEffectOperator<FluentTag>>(subsubeffect.get_index()));
                                else if constexpr (std::is_same_v<SubSubEffectT, NumericEffectView<Decrease, FluentTag>>)
                                    data_fluent_numeric_effects.push_back(Data<NumericEffectOperator<FluentTag>>(subsubeffect.get_index()));
                                else if constexpr (std::is_same_v<SubSubEffectT, NumericEffectView<ScaleUp, FluentTag>>)
                                    data_fluent_numeric_effects.push_back(Data<NumericEffectOperator<FluentTag>>(subsubeffect.get_index()));
                                else if constexpr (std::is_same_v<SubSubEffectT, NumericEffectView<ScaleDown, FluentTag>>)
                                    data_fluent_numeric_effects.push_back(Data<NumericEffectOperator<FluentTag>>(subsubeffect.get_index()));
                                else if constexpr (std::is_same_v<SubSubEffectT, NumericEffectView<Increase, AuxiliaryTag>>)
                                {
                                    assert(!data_auxiliary_numeric_effect);
                                    data_auxiliary_numeric_effect = Data<NumericEffectOperator<AuxiliaryTag>>(subsubeffect.get_index());
                                }
                                else
                                    static_assert(dependent_false<SubSubEffectT>::value, "Unexpected case.");
                            },
                            numeric_effect_view_variant);
                    }
                    else
                    {
                        throw std::runtime_error("Unexpected effect");
                    }
                },
                tmp_effect->get_effect());
        }
        ///---------- Pop parameters -------------
        m_param_map.pop_parameters(universal_parameters);
    };

    /* Parse the effect */
    auto conditional_effect_data = ConditionalEffectData {};
    // Parse conjunctive part
    std::visit(
        [&](auto&& effect)
        {
            using EffectT = std::decay_t<decltype(effect)>;

            if constexpr (std::is_same_v<EffectT, loki::EffectAnd>)
            {
                for (const auto& nested_effect : effect->get_effects())
                {
                    translate_effect_func(nested_effect, conditional_effect_data);
                }
            }
            else
            {
                translate_effect_func(element, conditional_effect_data);
            }
        },
        element->get_effect());

    /* Instantiate conditional effects. */
    auto conditional_effects = IndexList<ConditionalEffect> {};

    for (const auto& [cond_conjunctive_condition, value] : conditional_effect_data)
    {
        const auto& [cond_effect_universal_parameters, cond_effect_fluent_literals, cond_effect_fluent_numeric_effects, cond_effect_auxiliary_numeric_effects] =
            value;

        auto conj_effect_ptr = builder.template get_builder<ConjunctiveEffect>();
        auto& conj_effect = *conj_effect_ptr;
        conj_effect.clear();
        conj_effect.literals = cond_effect_fluent_literals;
        conj_effect.numeric_effects = cond_effect_fluent_numeric_effects;
        conj_effect.auxiliary_numeric_effect = cond_effect_auxiliary_numeric_effects;
        canonicalize(conj_effect);
        const auto conj_effect_index = context.get_or_create(conj_effect).first.get_index();

        auto cond_effect_ptr = builder.template get_builder<ConditionalEffect>();
        auto& cond_effect = *cond_effect_ptr;
        cond_effect.clear();
        cond_effect.variables = cond_effect_universal_parameters;
        cond_effect.condition = cond_conjunctive_condition;
        cond_effect.effect = conj_effect_index;
        canonicalize(cond_effect);
        const auto cond_effect_index = context.get_or_create(cond_effect).first.get_index();

        conditional_effects.push_back(cond_effect_index);
    }

    return conditional_effects;
}

Index<Action> LokiToTyrTranslator::translate_lifted(loki::Action element, Builder& builder, Repository& context)
{
    auto action_ptr = builder.template get_builder<Action>();
    auto& action = *action_ptr;
    action.clear();
    action.original_arity = element->get_original_arity();
    action.name = element->get_name();

    // 1. Translate conditions
    auto parameters = translate_common(element->get_parameters(), builder, context);
    action.variables = parameters;

    ///---------- Push parameters and parse scope -------------
    m_param_map.push_parameters(parameters);
    {
        auto conjunctive_condition = Index<ConjunctiveCondition>::max();
        if (element->get_condition().has_value())
        {
            conjunctive_condition = translate_lifted(element->get_condition().value(), parameters, builder, context);
        }
        else
        {
            // Create empty one
            auto conj_cond_ptr = builder.template get_builder<ConjunctiveCondition>();
            auto& conj_cond = *conj_cond_ptr;
            conj_cond.clear();
            canonicalize(conj_cond);
            conjunctive_condition = context.get_or_create(conj_cond).first.get_index();
        }
        action.condition = conjunctive_condition;

        // 2. Translate effects
        auto conditional_effects = IndexList<ConditionalEffect> {};
        if (element->get_effect().has_value())
        {
            const auto conditional_effects_ = translate_lifted(element->get_effect().value(), parameters, builder, context);
            conditional_effects = conditional_effects_;
        }
        action.effects = conditional_effects;
    }
    ///---------- Pop parameters -------------
    m_param_map.pop_parameters(parameters);

    canonicalize(action);
    return context.get_or_create(action).first.get_index();
}

Index<Axiom> LokiToTyrTranslator::translate_lifted(loki::Axiom element, Builder& builder, Repository& context)
{
    auto axiom_ptr = builder.template get_builder<Axiom>();
    auto& axiom = *axiom_ptr;
    axiom.clear();

    auto parameters = translate_common(element->get_parameters(), builder, context);
    axiom.variables = parameters;

    ///---------- Push parameters and parse scope -------------
    m_param_map.push_parameters(parameters);
    {
        axiom.body = translate_lifted(element->get_condition(), parameters, builder, context);
        const auto literal_view_variant = translate_lifted(element->get_literal(), builder, context);

        std::visit(
            [&](auto&& arg)
            {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, LiteralView<DerivedTag>>)
                    axiom.head = arg.get_atom().get_index();
                else
                    throw std::runtime_error("ToMimirStructures::translate_lifted: Expected Literal<DerivedTag> in axiom head.");
            },
            literal_view_variant);
    }
    ///---------- Pop parameters -------------
    m_param_map.pop_parameters(parameters);

    canonicalize(axiom);
    return context.get_or_create(axiom).first.get_index();
}

/**
 * Grounded translation.
 */

Index<Object> LokiToTyrTranslator::translate_grounded(loki::Term element, Builder& builder, Repository& context)
{
    return std::visit(
        [&](auto&& arg) -> Index<Object>
        {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, loki::Object>)
                return translate_common(arg, builder, context);
            else if constexpr (std::is_same_v<T, loki::Variable>)
                throw std::runtime_error("Expected ground term.");
            else
                static_assert(dependent_false<T>::value, "Missing case for type");
        },
        element->get_object_or_variable());
}

GroundAtomViewVariant LokiToTyrTranslator::translate_grounded(loki::Atom element, Builder& builder, Repository& context)
{
    auto predicate_view_variant = translate_common(element->get_predicate(), builder, context);

    auto build_atom = [&](auto fact_tag, auto predicate)
    {
        using Tag = std::decay_t<decltype(fact_tag)>;

        auto atom_ptr = builder.template get_builder<GroundAtom<Tag>>();
        auto& atom = *atom_ptr;
        atom.clear();
        atom.binding = to_binding(predicate, this->translate_grounded(element->get_terms(), builder, context), context).first.get_index();
        canonicalize(atom);
        return context.get_or_create(atom).first;
    };

    return std::visit(
        [&](auto&& arg) -> GroundAtomViewVariant
        {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, PredicateView<StaticTag>>)
                return build_atom(StaticTag {}, arg);
            else if constexpr (std::is_same_v<T, PredicateView<FluentTag>>)
                return build_atom(FluentTag {}, arg);
            else if constexpr (std::is_same_v<T, PredicateView<DerivedTag>>)
                return build_atom(DerivedTag {}, arg);
            else
                static_assert(dependent_false<T>::value, "Missing case for type");
        },
        predicate_view_variant);
}

GroundAtomOrFactViewVariant LokiToTyrTranslator::translate_grounded(loki::Atom element, Builder& builder, Repository& context, FDRContext& fdr_context)
{
    auto atom_variant = translate_grounded(element, builder, context);

    return std::visit(
        [&](auto&& arg) -> GroundAtomOrFactViewVariant
        {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, GroundAtomView<StaticTag>>)
                return arg;
            else if constexpr (std::is_same_v<T, GroundAtomView<FluentTag>>)
                return fdr_context.get_fact_view(arg);
            else if constexpr (std::is_same_v<T, GroundAtomView<DerivedTag>>)
                return arg;
            else
                static_assert(dependent_false<T>::value, "Missing case for type");
        },
        atom_variant);
}

GroundLiteralViewVariant LokiToTyrTranslator::translate_grounded(loki::Literal element, Builder& builder, Repository& context)
{
    auto atom_view_variant = translate_grounded(element->get_atom(), builder, context);

    auto build_literal = [&](auto fact_tag, auto atom)
    {
        using Tag = std::decay_t<decltype(fact_tag)>;

        auto literal_ptr = builder.template get_builder<GroundLiteral<Tag>>();
        auto& literal = *literal_ptr;
        literal.clear();
        literal.atom = atom.get_index();
        literal.polarity = element->get_polarity();
        canonicalize(literal);
        return context.get_or_create(literal).first;
    };

    return std::visit(
        [&](auto&& arg) -> GroundLiteralViewVariant
        {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, GroundAtomView<StaticTag>>)
                return build_literal(StaticTag {}, arg);
            else if constexpr (std::is_same_v<T, GroundAtomView<FluentTag>>)
                return build_literal(FluentTag {}, arg);
            else if constexpr (std::is_same_v<T, GroundAtomView<DerivedTag>>)
                return build_literal(DerivedTag {}, arg);
            else
                static_assert(dependent_false<T>::value, "Missing case for type");
        },
        atom_view_variant);
}

GroundLiteralOrFactViewVariant LokiToTyrTranslator::translate_grounded(loki::Literal element, Builder& builder, Repository& context, FDRContext& fdr_context)
{
    auto literal_view_variant = translate_grounded(element, builder, context);

    return std::visit(
        [&](auto&& arg) -> GroundLiteralOrFactViewVariant
        {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, GroundLiteralView<StaticTag>>)
                return arg;
            else if constexpr (std::is_same_v<T, GroundLiteralView<FluentTag>>)
                return std::make_pair(fdr_context.get_fact_view(arg.get_atom()), arg.get_polarity());
            else if constexpr (std::is_same_v<T, GroundLiteralView<DerivedTag>>)
                return arg;
            else
                static_assert(dependent_false<T>::value, "Missing case for type");
        },
        literal_view_variant);
}

Data<GroundFunctionExpression> LokiToTyrTranslator::translate_grounded(loki::FunctionExpressionNumber element, Builder& builder, Repository& context)
{
    return Data<GroundFunctionExpression>(float_t(element->get_number()));
}

Data<GroundFunctionExpression> LokiToTyrTranslator::translate_grounded(loki::FunctionExpressionBinaryOperator element, Builder& builder, Repository& context)
{
    auto build_binary_op = [&](auto op_tag) -> Data<GroundFunctionExpression>
    {
        using Tag = std::decay_t<decltype(op_tag)>;

        auto binary_ptr = builder.template get_builder<BinaryOperator<Tag, Data<GroundFunctionExpression>>>();
        auto& binary = *binary_ptr;
        binary.clear();
        binary.lhs = translate_grounded(element->get_left_function_expression(), builder, context);
        binary.rhs = translate_grounded(element->get_right_function_expression(), builder, context);
        canonicalize(binary);
        return Data<GroundFunctionExpression>(Data<ArithmeticOperator<Data<GroundFunctionExpression>>>(context.get_or_create(binary).first.get_index()));
    };

    switch (element->get_binary_operator())
    {
        case loki::BinaryOperatorEnum::PLUS:
            return build_binary_op(Add {});
        case loki::BinaryOperatorEnum::MINUS:
            return build_binary_op(Sub {});
        case loki::BinaryOperatorEnum::MUL:
            return build_binary_op(Mul {});
        case loki::BinaryOperatorEnum::DIV:
            return build_binary_op(Div {});
        default:
            throw std::runtime_error("Unexpected case");
    }
}

Data<GroundFunctionExpression> LokiToTyrTranslator::translate_grounded(loki::FunctionExpressionMultiOperator element, Builder& builder, Repository& context)
{
    auto build_multi_op = [&](auto op_tag) -> Data<GroundFunctionExpression>
    {
        using Tag = std::decay_t<decltype(op_tag)>;

        auto multi_ptr = builder.template get_builder<MultiOperator<Tag, Data<GroundFunctionExpression>>>();
        auto& multi = *multi_ptr;
        multi.clear();
        multi.args = translate_grounded(element->get_function_expressions(), builder, context);
        canonicalize(multi);
        return Data<GroundFunctionExpression>(Data<ArithmeticOperator<Data<GroundFunctionExpression>>>(context.get_or_create(multi).first.get_index()));
    };

    switch (element->get_multi_operator())
    {
        case loki::MultiOperatorEnum::PLUS:
            return build_multi_op(Add {});
        case loki::MultiOperatorEnum::MUL:
            return build_multi_op(Mul {});
        default:
            throw std::runtime_error("Unexpected case");
    }
}

Data<GroundFunctionExpression> LokiToTyrTranslator::translate_grounded(loki::FunctionExpressionMinus element, Builder& builder, Repository& context)
{
    auto minus_ptr = builder.template get_builder<UnaryOperator<Sub, Data<GroundFunctionExpression>>>();
    auto& minus = *minus_ptr;
    minus.clear();
    minus.arg = translate_grounded(element->get_function_expression(), builder, context);
    canonicalize(minus);
    return Data<GroundFunctionExpression>(Data<ArithmeticOperator<Data<GroundFunctionExpression>>>(context.get_or_create(minus).first.get_index()));
}

Data<GroundFunctionExpression> LokiToTyrTranslator::translate_grounded(loki::FunctionExpressionFunction element, Builder& builder, Repository& context)
{
    const auto fterm_view_variant = translate_grounded(element->get_function(), builder, context);

    return std::visit(
        [&](auto&& arg) -> Data<GroundFunctionExpression>
        {
            using T = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<T, GroundFunctionTermView<StaticTag>>)
                return Data<GroundFunctionExpression>(arg.get_index());
            else if constexpr (std::is_same_v<T, GroundFunctionTermView<FluentTag>>)
                return Data<GroundFunctionExpression>(arg.get_index());
            else if constexpr (std::is_same_v<T, GroundFunctionTermView<AuxiliaryTag>>)
                return Data<GroundFunctionExpression>(arg.get_index());
            else
                static_assert(dependent_false<T>::value, "Missing case for type");
        },
        fterm_view_variant);
}

Data<GroundFunctionExpression> LokiToTyrTranslator::translate_grounded(loki::FunctionExpression element, Builder& builder, Repository& context)
{
    return std::visit([&](auto&& arg) { return translate_grounded(arg, builder, context); }, element->get_function_expression());
}

GroundFunctionTermViewVariant LokiToTyrTranslator::translate_grounded(loki::Function element, Builder& builder, Repository& context)
{
    auto function_view_variant = translate_common(element->get_function_skeleton(), builder, context);

    auto build_function_term = [&](auto fact_tag, auto function) -> GroundFunctionTermViewVariant
    {
        using Tag = std::decay_t<decltype(fact_tag)>;

        auto fterm_ptr = builder.template get_builder<GroundFunctionTerm<Tag>>();
        auto& fterm = *fterm_ptr;
        fterm.clear();
        fterm.binding = to_binding(function, this->translate_grounded(element->get_terms(), builder, context), context).first.get_index();
        canonicalize(fterm);
        return context.get_or_create(fterm).first;
    };

    return std::visit(
        [&](auto&& arg) -> GroundFunctionTermViewVariant
        {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, FunctionView<StaticTag>>)
                return build_function_term(StaticTag {}, arg);
            else if constexpr (std::is_same_v<T, FunctionView<FluentTag>>)
                return build_function_term(FluentTag {}, arg);
            else if constexpr (std::is_same_v<T, FunctionView<AuxiliaryTag>>)
                return build_function_term(AuxiliaryTag {}, arg);
            else
                static_assert(dependent_false<T>::value, "Missing case for type");
        },
        function_view_variant);
}

GroundFunctionTermValueViewVariant LokiToTyrTranslator::translate_grounded(loki::FunctionValue element, Builder& builder, Repository& context)
{
    auto fterm_view_variant = translate_grounded(element->get_function(), builder, context);

    auto build_fterm_value = [&](auto fact_tag, auto fterm) -> GroundFunctionTermValueViewVariant
    {
        using Tag = std::decay_t<decltype(fact_tag)>;

        auto fterm_value_ptr = builder.template get_builder<GroundFunctionTermValue<Tag>>();
        auto& fterm_value = *fterm_value_ptr;
        fterm_value.clear();
        fterm_value.fterm = fterm.get_index();
        fterm_value.value = element->get_number();
        canonicalize(fterm_value);
        return context.get_or_create(fterm_value).first;
    };

    return std::visit(
        [&](auto&& arg) -> GroundFunctionTermValueViewVariant
        {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, GroundFunctionTermView<StaticTag>>)
                return build_fterm_value(StaticTag {}, arg);
            else if constexpr (std::is_same_v<T, GroundFunctionTermView<FluentTag>>)
                return build_fterm_value(FluentTag {}, arg);
            else if constexpr (std::is_same_v<T, GroundFunctionTermView<AuxiliaryTag>>)
                return build_fterm_value(AuxiliaryTag {}, arg);
            else
                static_assert(dependent_false<T>::value, "Missing case for type");
        },
        fterm_view_variant);
}

Data<BooleanOperator<Data<GroundFunctionExpression>>>
LokiToTyrTranslator::translate_grounded(loki::ConditionNumericConstraint element, Builder& builder, Repository& context)
{
    auto build_binary_op = [&](auto op_tag) -> Data<BooleanOperator<Data<GroundFunctionExpression>>>
    {
        using Tag = std::decay_t<decltype(op_tag)>;

        auto binary_ptr = builder.template get_builder<BinaryOperator<Tag, Data<GroundFunctionExpression>>>();
        auto& binary = *binary_ptr;
        binary.clear();
        binary.lhs = translate_grounded(element->get_left_function_expression(), builder, context);
        binary.rhs = translate_grounded(element->get_right_function_expression(), builder, context);
        canonicalize(binary);
        return Data<BooleanOperator<Data<GroundFunctionExpression>>>(context.get_or_create(binary).first.get_index());
    };

    switch (element->get_binary_comparator())
    {
        case loki::BinaryComparatorEnum::EQUAL:
            return build_binary_op(Eq {});
        case loki::BinaryComparatorEnum::LESS_EQUAL:
            return build_binary_op(Le {});
        case loki::BinaryComparatorEnum::LESS:
            return build_binary_op(Lt {});
        case loki::BinaryComparatorEnum::GREATER_EQUAL:
            return build_binary_op(Ge {});
        case loki::BinaryComparatorEnum::GREATER:
            return build_binary_op(Gt {});
        default:
            throw std::runtime_error("Unexpected case");
    }
}

Index<GroundConjunctiveCondition>
LokiToTyrTranslator::translate_grounded(loki::Condition element, Builder& builder, Repository& context, FDRContext& fdr_context)
{
    auto conj_condition_ptr = builder.template get_builder<GroundConjunctiveCondition>();
    auto& conj_condition = *conj_condition_ptr;
    conj_condition.clear();

    const auto func_insert_literal = [](GroundLiteralOrFactViewVariant literal_or_fact_view_variant,
                                        IndexList<GroundLiteral<StaticTag>>& static_literals,
                                        IndexList<GroundLiteral<DerivedTag>>& derived_literals,
                                        DataList<FDRFact<FluentTag>>& positive_facts,
                                        DataList<FDRFact<FluentTag>>& negative_facts)
    {
        std::visit(
            [&](auto&& arg)
            {
                using T = std::decay_t<decltype(arg)>;

                if constexpr (std::is_same_v<T, GroundLiteralView<StaticTag>>)
                    static_literals.push_back(arg.get_index());
                else if constexpr (std::is_same_v<T, std::pair<FDRFactView<FluentTag>, bool>>)
                {
                    if (arg.second)
                        positive_facts.push_back(arg.first.get_data());
                    else
                        negative_facts.push_back(arg.first.get_data());
                }
                else if constexpr (std::is_same_v<T, GroundLiteralView<DerivedTag>>)
                    derived_literals.push_back(arg.get_index());
                else
                    static_assert(dependent_false<T>::value, "Missing case for type");
            },
            literal_or_fact_view_variant);
    };

    return std::visit(
        [&](auto&& condition) -> Index<GroundConjunctiveCondition>
        {
            using ConditionT = std::decay_t<decltype(condition)>;

            if constexpr (std::is_same_v<ConditionT, loki::ConditionAnd>)
            {
                for (const auto& part : condition->get_conditions())
                {
                    std::visit(
                        [&](auto&& subcondition)
                        {
                            using SubConditionT = std::decay_t<decltype(subcondition)>;

                            if constexpr (std::is_same_v<SubConditionT, loki::ConditionLiteral>)
                            {
                                const auto literal_or_fact_view_variant = translate_grounded(subcondition->get_literal(), builder, context, fdr_context);

                                func_insert_literal(literal_or_fact_view_variant,
                                                    conj_condition.static_literals,
                                                    conj_condition.derived_literals,
                                                    conj_condition.positive_facts,
                                                    conj_condition.negative_facts);
                            }
                            else if constexpr (std::is_same_v<SubConditionT, loki::ConditionNumericConstraint>)
                            {
                                const auto numeric_constraint = translate_grounded(subcondition, builder, context);

                                conj_condition.numeric_constraints.push_back(numeric_constraint);
                            }
                            else
                            {
                                // std::cout << std::visit([](auto&& arg) { return arg.str(); }, *part) << std::endl;
                                throw std::logic_error("Unexpected condition.");
                            }
                        },
                        part->get_condition());
                }

                canonicalize(conj_condition);
                return context.get_or_create(conj_condition).first.get_index();
            }
            else if constexpr (std::is_same_v<ConditionT, loki::ConditionLiteral>)
            {
                const auto index_literal_variant = translate_grounded(condition->get_literal(), builder, context, fdr_context);

                func_insert_literal(index_literal_variant,
                                    conj_condition.static_literals,
                                    conj_condition.derived_literals,
                                    conj_condition.positive_facts,
                                    conj_condition.negative_facts);

                canonicalize(conj_condition);
                return context.get_or_create(conj_condition).first.get_index();
            }
            else if constexpr (std::is_same_v<ConditionT, loki::ConditionNumericConstraint>)
            {
                const auto numeric_constraint = translate_grounded(condition, builder, context);

                conj_condition.numeric_constraints.push_back(numeric_constraint);

                canonicalize(conj_condition);
                return context.get_or_create(conj_condition).first.get_index();
            }
            else
            {
                // std::cout << std::visit([](auto&& arg) { return arg.str(); }, *condition_ptr) << std::endl;
                throw std::logic_error("Unexpected condition.");
            }
        },
        element->get_condition());
}

Index<Metric> LokiToTyrTranslator::translate_grounded(loki::OptimizationMetric element, Builder& builder, Repository& context)
{
    auto metric_ptr = builder.template get_builder<Metric>();
    auto& metric = *metric_ptr;
    metric.clear();

    metric.fexpr = translate_grounded(element->get_function_expression(), builder, context);
    switch (element->get_optimization_metric())
    {
        case loki::OptimizationMetricEnum::MINIMIZE:
        {
            metric.objective = Minimize {};
            break;
        }
        case loki::OptimizationMetricEnum::MAXIMIZE:
        {
            metric.objective = Maximize {};
            break;
        }
        default:
            throw std::runtime_error("Unexpected case.");
    }

    canonicalize(metric);
    return context.get_or_create(metric).first.get_index();
}

PlanningDomain LokiToTyrTranslator::translate(const loki::Domain& element)
{
    auto builder = Builder();
    auto factory = std::make_shared<RepositoryFactory>();
    auto context = factory->create_shared();

    /* Perform static type analysis */
    prepare(element);

    auto domain_ptr = builder.get_builder<planning::Domain>();
    auto& domain = *domain_ptr;
    domain.clear();

    /* Name */
    domain.name = element->get_name();

    /* Requirements section */

    /* Constants section */
    domain.constants = translate_common(element->get_constants(), builder, *context);

    /* Predicates section */
    const auto func_insert_predicate = [](PredicateViewVariant predicate_view_variant,
                                          IndexList<Predicate<StaticTag>>& static_predicates,
                                          IndexList<Predicate<FluentTag>>& fluent_predicates,
                                          IndexList<Predicate<DerivedTag>>& derived_predicates)
    {
        std::visit(
            [&](auto&& arg)
            {
                using T = std::decay_t<decltype(arg)>;

                if constexpr (std::is_same_v<T, PredicateView<StaticTag>>)
                    static_predicates.push_back(arg.get_index());
                else if constexpr (std::is_same_v<T, PredicateView<FluentTag>>)
                    fluent_predicates.push_back(arg.get_index());
                else if constexpr (std::is_same_v<T, PredicateView<DerivedTag>>)
                    derived_predicates.push_back(arg.get_index());
                else
                    static_assert(dependent_false<T>::value, "Missing case for type");
            },
            predicate_view_variant);
    };

    for (const auto& predicate_view_variant : translate_common(element->get_predicates(), builder, *context))
    {
        func_insert_predicate(predicate_view_variant, domain.static_predicates, domain.fluent_predicates, domain.derived_predicates);
    }

    /* Functions section */
    const auto func_insert_function = [](FunctionViewVariant function_view_variant,
                                         IndexList<Function<StaticTag>>& static_functions,
                                         IndexList<Function<FluentTag>>& fluent_functions,
                                         ::cista::optional<Index<Function<AuxiliaryTag>>>& auxiliary_function)
    {
        std::visit(
            [&](auto&& arg)
            {
                using T = std::decay_t<decltype(arg)>;

                if constexpr (std::is_same_v<T, FunctionView<StaticTag>>)
                    static_functions.push_back(arg.get_index());
                else if constexpr (std::is_same_v<T, FunctionView<FluentTag>>)
                    fluent_functions.push_back(arg.get_index());
                else if constexpr (std::is_same_v<T, FunctionView<AuxiliaryTag>>)
                {
                    assert(!auxiliary_function);
                    auxiliary_function = arg.get_index();
                }
                else
                    static_assert(dependent_false<T>::value, "Missing case for type");
            },
            function_view_variant);
    };

    for (const auto& function_view_variant : translate_common(element->get_function_skeletons(), builder, *context))
    {
        func_insert_function(function_view_variant, domain.static_functions, domain.fluent_functions, domain.auxiliary_function);
    }

    /* Structures section */
    domain.actions = translate_lifted(element->get_actions(), builder, *context);
    domain.axioms = translate_lifted(element->get_axioms(), builder, *context);

    canonicalize(domain);
    return PlanningDomain(context->get_or_create(domain).first, context, std::move(factory));
}

PlanningTask LokiToTyrTranslator::translate(const loki::Problem& element, PlanningDomain domain)
{
    auto builder = Builder();

    /* Perform static type analysis */
    prepare(element);

    auto task_ptr = builder.get_builder<planning::Task>();
    auto& task = *task_ptr;
    task.clear();

    const auto& factory = domain.get_repository_factory();
    auto task_context = factory->create_shared(domain.get_repository().get());

    auto fdr_context = std::make_shared<FDRContext>(task_context);

    /* Name */
    task.name = element->get_name();

    /* Domain */
    task.domain = domain.get_domain().get_index();

    /* Requirements section */

    /* Objects section */
    task.objects = translate_common(element->get_objects(), builder, *task_context);

    /* Predicates section */
    const auto func_insert_predicate = [](PredicateViewVariant predicate_view_variant, IndexList<Predicate<DerivedTag>>& derived_predicates)
    {
        std::visit(
            [&](auto&& arg)
            {
                using T = std::decay_t<decltype(arg)>;

                if constexpr (std::is_same_v<T, PredicateView<StaticTag>>)
                    throw std::runtime_error("Static predicate definition in task is not supported");
                else if constexpr (std::is_same_v<T, PredicateView<FluentTag>>)
                    throw std::runtime_error("Fluent predicate definition in task is not supported");
                else if constexpr (std::is_same_v<T, PredicateView<DerivedTag>>)
                    derived_predicates.push_back(arg.get_index());
                else
                    static_assert(dependent_false<T>::value, "Missing case for type");
            },
            predicate_view_variant);
    };

    for (const auto& predicate_view_variant : translate_common(element->get_predicates(), builder, *task_context))
    {
        func_insert_predicate(predicate_view_variant, task.derived_predicates);
    }

    /* Initial section */
    const auto func_insert_ground_atom = [&](GroundLiteralOrFactViewVariant literal_or_fact_view_variant,
                                             IndexList<GroundAtom<StaticTag>>& static_atoms,
                                             IndexList<GroundAtom<FluentTag>>& fluent_atoms)
    {
        std::visit(
            [&](auto&& arg)
            {
                using T = std::decay_t<decltype(arg)>;

                if constexpr (std::is_same_v<T, GroundLiteralView<StaticTag>>)
                    static_atoms.push_back(arg.get_atom().get_index());
                else if constexpr (std::is_same_v<T, std::pair<FDRFactView<FluentTag>, bool>>)
                    fluent_atoms.push_back(arg.first.get_atom().value().get_index());  // we know it must have a value
                else if constexpr (std::is_same_v<T, GroundLiteralView<DerivedTag>>)
                    throw std::runtime_error("Derived ground atoms are not allowed to be defined in the initial section.");
                else
                    static_assert(dependent_false<T>::value, "Missing case for type");
            },
            literal_or_fact_view_variant);
    };

    for (const auto& literal : element->get_initial_literals())
    {
        const auto literal_or_fact_view_variant = translate_grounded(literal, builder, *task_context, *fdr_context);

        func_insert_ground_atom(literal_or_fact_view_variant, task.static_atoms, task.fluent_atoms);
    }

    const auto func_insert_fterm_values = [](GroundFunctionTermValueViewVariant fterm_value_view_variant,
                                             IndexList<GroundFunctionTermValue<StaticTag>>& static_fterm_values,
                                             IndexList<GroundFunctionTermValue<FluentTag>>& fluent_fterm_values,
                                             ::cista::optional<Index<GroundFunctionTermValue<AuxiliaryTag>>>& auxiliary_fterm_value)
    {
        std::visit(
            [&](auto&& arg)
            {
                using T = std::decay_t<decltype(arg)>;

                if constexpr (std::is_same_v<T, GroundFunctionTermValueView<StaticTag>>)
                    static_fterm_values.push_back(arg.get_index());
                else if constexpr (std::is_same_v<T, GroundFunctionTermValueView<FluentTag>>)
                    fluent_fterm_values.push_back(arg.get_index());
                else if constexpr (std::is_same_v<T, GroundFunctionTermValueView<AuxiliaryTag>>)
                {
                    assert(!auxiliary_fterm_value);
                    auxiliary_fterm_value = arg.get_index();
                }
                else
                    static_assert(dependent_false<T>::value, "Missing case for type");
            },
            fterm_value_view_variant);
    };

    for (const auto fterm_value_view_variant : translate_grounded(element->get_initial_function_values(), builder, *task_context))
    {
        func_insert_fterm_values(fterm_value_view_variant, task.static_fterm_values, task.fluent_fterm_values, task.auxiliary_fterm_value);
    }

    /* Goal section */

    if (element->get_goal_condition().has_value())
    {
        task.goal = translate_grounded(element->get_goal_condition().value(), builder, *task_context, *fdr_context);
    }
    else
    {
        // Create empty conjunctive condition
        auto conj_cond_ptr = builder.get_builder<GroundConjunctiveCondition>();
        auto& conj_cond = *conj_cond_ptr;
        conj_cond.clear();
        canonicalize(conj_cond);
        task.goal = task_context->get_or_create(conj_cond).first.get_index();
    }

    /* Metric section */
    if (element->get_optimization_metric().has_value())
    {
        task.metric = translate_grounded(element->get_optimization_metric().value(), builder, *task_context);
    }
    else
    {
        task.metric = std::nullopt;
    }

    /* Structures section */
    task.axioms = translate_lifted(element->get_axioms(), builder, *task_context);

    canonicalize(task);
    return PlanningTask(task_context->get_or_create(task).first, std::move(fdr_context), task_context, std::move(domain));
}

}
