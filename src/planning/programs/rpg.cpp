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

#include "tyr/planning/programs/rpg.hpp"

#include "common.hpp"
#include "tyr/analysis/domains.hpp"
#include "tyr/common/unordered_set.hpp"
#include "tyr/formalism/datalog/formatter.hpp"
#include "tyr/formalism/datalog/repository.hpp"
#include "tyr/formalism/datalog/views.hpp"
#include "tyr/formalism/planning/formatter.hpp"
#include "tyr/formalism/planning/merge_datalog.hpp"
#include "tyr/formalism/planning/repository.hpp"
#include "tyr/formalism/planning/views.hpp"

namespace f = tyr::formalism;
namespace d = tyr::datalog;
namespace fp = tyr::formalism::planning;
namespace fd = tyr::formalism::datalog;

namespace tyr::planning
{
namespace
{
template<typename Op>
struct DatalogNumericEffectOp;

template<>
struct DatalogNumericEffectOp<fp::OpAssign>
{
    using type = fd::OpAssign;
};
template<>
struct DatalogNumericEffectOp<fp::OpIncrease>
{
    using type = fd::OpIncrease;
};
template<>
struct DatalogNumericEffectOp<fp::OpDecrease>
{
    using type = fd::OpDecrease;
};
template<>
struct DatalogNumericEffectOp<fp::OpScaleUp>
{
    using type = fd::OpScaleUp;
};
template<>
struct DatalogNumericEffectOp<fp::OpScaleDown>
{
    using type = fd::OpScaleDown;
};

template<typename Op>
using DatalogNumericEffectOpT = typename DatalogNumericEffectOp<Op>::type;

void fill_delete_free_condition(fp::ActionView action,
                                fp::ConditionalEffectView cond_eff,
                                TranslationContext& translation_context,
                                formalism::planning::MergeDatalogContext& context,
                                Data<formalism::datalog::ConjunctiveCondition>& conj_cond)
{
    // Action parameter may get deleted.
    for (const auto& variable : action.get_variables())
        conj_cond.variables.push_back(merge_p2d(variable, context).first.get_index());
    for (const auto literal : action.get_condition().get_literals<formalism::StaticTag>())
        conj_cond.static_literals.push_back(merge_p2d(literal, translation_context.p2d.static_to_static_predicate, context).first.get_index());
    for (const auto literal : action.get_condition().get_literals<formalism::FluentTag>())
        if (literal.get_polarity())
            conj_cond.fluent_literals.push_back(merge_p2d(literal, translation_context.p2d.fluent_to_fluent_predicate, context).first.get_index());

    for (const auto variable : cond_eff.get_variables())
        conj_cond.variables.push_back(merge_p2d(variable, context).first.get_index());
    for (const auto literal : cond_eff.get_condition().template get_literals<formalism::StaticTag>())
        conj_cond.static_literals.push_back(merge_p2d(literal, translation_context.p2d.static_to_static_predicate, context).first.get_index());
    for (const auto literal : cond_eff.get_condition().template get_literals<formalism::FluentTag>())
        if (literal.get_polarity())
            conj_cond.fluent_literals.push_back(merge_p2d(literal, translation_context.p2d.fluent_to_fluent_predicate, context).first.get_index());
}

template<fp::NumericEffectOpKind Op>
auto merge_numeric_effect(fp::NumericEffectView<Op, formalism::FluentTag> effect, formalism::planning::MergeDatalogContext& context)
{
    using DOp = DatalogNumericEffectOpT<Op>;

    auto numeric_effect_ptr = context.builder.get_builder<fd::NumericEffect<DOp, formalism::FluentTag>>();
    auto& numeric_effect = *numeric_effect_ptr;
    numeric_effect.clear();

    numeric_effect.fterm = merge_p2d(effect.get_fterm(), context).first.get_index();
    numeric_effect.fexpr = merge_p2d(effect.get_fexpr(), context);

    canonicalize(numeric_effect);
    return context.destination.get_or_create(numeric_effect);
}

auto merge_numeric_effect_operator(fp::NumericEffectOperatorView<formalism::FluentTag> effect, formalism::planning::MergeDatalogContext& context)
{
    using Operator = fd::NumericEffectOperator<formalism::FluentTag>;
    using OperatorData = Data<Operator>;

    return visit(
        [&](auto&& arg) -> OperatorData
        {
            return OperatorData(typename OperatorData::Variant(merge_numeric_effect(arg, context).first.get_index()));
        },
        effect.get_variant());
}

auto create_cond_effect_rule(fp::ActionView action,
                             fp::ConditionalEffectView cond_eff,
                             fp::AtomView<formalism::FluentTag> effect,
                             TranslationContext& translation_context,
                             formalism::planning::MergeDatalogContext& context)
{
    auto rule_ptr = context.builder.get_builder<formalism::datalog::Rule>();
    auto& rule = *rule_ptr;
    rule.clear();

    auto conj_cond_ptr = context.builder.get_builder<formalism::datalog::ConjunctiveCondition>();
    auto& conj_cond = *conj_cond_ptr;
    conj_cond.clear();

    fill_delete_free_condition(action, cond_eff, translation_context, context, conj_cond);

    canonicalize(conj_cond);
    const auto new_conj_cond = context.destination.get_or_create(conj_cond).first;

    rule.variables = new_conj_cond.get_variables().get_data();
    rule.body = new_conj_cond.get_index();
    rule.head = merge_p2d(effect, translation_context.p2d.fluent_to_fluent_predicate, context).first.get_index();
    rule.cost = 1;

    canonicalize(rule);
    return context.destination.get_or_create(rule);
}

auto create_cond_numeric_effect_rule(fp::ActionView action,
                                     fp::ConditionalEffectView cond_eff,
                                     fp::NumericEffectOperatorView<formalism::FluentTag> effect,
                                     TranslationContext& translation_context,
                                     formalism::planning::MergeDatalogContext& context)
{
    auto rule_ptr = context.builder.get_builder<formalism::datalog::Rule>();
    auto& rule = *rule_ptr;
    rule.clear();

    auto conj_cond_ptr = context.builder.get_builder<formalism::datalog::ConjunctiveCondition>();
    auto& conj_cond = *conj_cond_ptr;
    conj_cond.clear();

    fill_delete_free_condition(action, cond_eff, translation_context, context, conj_cond);

    canonicalize(conj_cond);
    const auto new_conj_cond = context.destination.get_or_create(conj_cond).first;

    rule.variables = new_conj_cond.get_variables().get_data();
    rule.body = new_conj_cond.get_index();
    rule.head = merge_numeric_effect_operator(effect, context);
    rule.cost = 1;

    canonicalize(rule);
    return context.destination.get_or_create(rule);
}

void translate_action_to_delete_free_rules(fp::ActionView action,
                                           Data<fd::Program>& program,
                                           TranslationContext& translation_context,
                                           fp::MergeDatalogContext& context,
                                           RPGProgram::RuleToActionMapping& rule_to_action)
{
    for (const auto cond_eff : action.get_effects())
    {
        for (const auto literal : cond_eff.get_effect().get_literals())
        {
            if (!literal.get_polarity())
                continue;  /// ignore delete effects

            const auto rule = create_cond_effect_rule(action, cond_eff, literal.get_atom(), translation_context, context).first;

            program.rules.push_back(rule.get_index());
            rule_to_action.emplace(rule, action);
        }

        for (const auto numeric_effect : cond_eff.get_effect().get_numeric_effects())
        {
            const auto rule = create_cond_numeric_effect_rule(action, cond_eff, numeric_effect, translation_context, context).first;

            program.rules.push_back(rule.get_index());
            rule_to_action.emplace(rule, action);
        }
    }
}

auto create_program(fp::TaskView task, TranslationContext& translation_context, RPGProgram::RuleToActionMapping& rule_to_action, fd::Repository& destination)
{
    auto builder = fd::Builder();
    auto context = fp::MergeDatalogContext(builder, destination);
    auto program_ptr = builder.get_builder<fd::Program>();
    auto& program = *program_ptr;
    program.clear();

    for (const auto predicate : task.get_domain().get_predicates<f::StaticTag>())
    {
        const auto new_predicate = fp::merge_p2d(predicate, context).first;
        translation_context.d2p.static_to_static_predicate.emplace(new_predicate, predicate);
        translation_context.p2d.static_to_static_predicate.emplace(predicate, new_predicate);
        program.static_predicates.push_back(new_predicate.get_index());
    }
    for (const auto predicate : task.get_domain().get_predicates<f::FluentTag>())
    {
        const auto new_predicate = fp::merge_p2d(predicate, context).first;
        translation_context.d2p.fluent_to_fluent_predicate.emplace(new_predicate, predicate);
        translation_context.p2d.fluent_to_fluent_predicate.emplace(predicate, new_predicate);
        program.fluent_predicates.push_back(new_predicate.get_index());
    }

    for (const auto function : task.get_domain().get_functions<f::StaticTag>())
        program.static_functions.push_back(fp::merge_p2d(function, context).first.get_index());
    for (const auto function : task.get_domain().get_functions<f::FluentTag>())
        program.fluent_functions.push_back(fp::merge_p2d(function, context).first.get_index());

    for (const auto object : task.get_domain().get_constants())
        program.objects.push_back(fp::merge_p2d(object, context).first.get_index());
    for (const auto object : task.get_objects())
        program.objects.push_back(fp::merge_p2d(object, context).first.get_index());

    for (const auto atom : task.get_atoms<f::StaticTag>())
        program.static_atoms.push_back(fp::merge_p2d(atom, translation_context.p2d.static_to_static_predicate, context).first.get_index());
    for (const auto atom : task.get_atoms<f::FluentTag>())
        program.fluent_atoms.push_back(fp::merge_p2d(atom, translation_context.p2d.fluent_to_fluent_predicate, context).first.get_index());

    for (const auto fterm_value : task.get_fterm_values<f::StaticTag>())
        program.static_fterm_values.push_back(fp::merge_p2d(fterm_value, context).first.get_index());
    for (const auto fterm_value : task.get_fterm_values<f::FluentTag>())
        program.fluent_fterm_values.push_back(fp::merge_p2d(fterm_value, context).first.get_index());

    for (const auto action : task.get_domain().get_actions())
        translate_action_to_delete_free_rules(action, program, translation_context, context, rule_to_action);

    canonicalize(program);
    return destination.get_or_create(program).first;
}

auto create_program_context(fp::TaskView task, TranslationContext& translation_context, RPGProgram::RuleToActionMapping& rule_to_action)
{
    auto factory = std::make_shared<fd::RepositoryFactory>();
    auto repository = factory->create_shared();
    auto program = create_program(task, translation_context, rule_to_action, *repository);
    auto domains = analysis::compute_variable_domains(program);
    auto strata = analysis::compute_rule_stratification(program);
    auto listeners = analysis::compute_listeners(strata, *repository);

    return datalog::ProgramContext(program, std::move(repository), std::move(factory), std::move(domains), std::move(strata), std::move(listeners));
}

}

RPGProgram::RPGProgram(fp::TaskView task) :
    m_translation_context(),
    m_rule_to_action(),
    m_program_context(create_program_context(task, m_translation_context, m_rule_to_action)),
    m_program_workspace(m_program_context)
{
    // std::cout << m_program_context.get_program() << std::endl;
}

const TranslationContext& RPGProgram::get_translation_context() const noexcept { return m_translation_context; }

const RPGProgram::RuleToActionMapping& RPGProgram::get_rule_to_action_mapping() const noexcept { return m_rule_to_action; }

datalog::ProgramContext& RPGProgram::get_program_context() noexcept { return m_program_context; }

const datalog::ProgramContext& RPGProgram::get_program_context() const noexcept { return m_program_context; }

const datalog::ConstProgramWorkspace& RPGProgram::get_const_program_workspace() const noexcept { return m_program_workspace; }

}
