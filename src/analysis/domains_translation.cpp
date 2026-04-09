#include "tyr/analysis/domains.hpp"

namespace tyr::analysis
{
namespace
{

template<typename C>
VariableDomainView<C> to_variable_domain_view(const VariableDomain& domain, const C& context)
{
    auto result = VariableDomainView<C> {};
    result.objects.reserve(domain.objects.size());

    for (const auto object : domain.objects)
        result.objects.push_back(make_view(object, context));

    return result;
}

template<typename C>
VariableDomainViewList<C> to_variable_domain_view_list(const VariableDomainList& domains, const C& context)
{
    auto result = VariableDomainViewList<C> {};
    result.reserve(domains.size());

    for (const auto& domain : domains)
        result.push_back(to_variable_domain_view(domain, context));

    return result;
}

template<typename Element, typename C>
SimpleScopedDomainViewMap<Element, C> to_simple_scoped_domain_view_map(const SimpleScopedDomainMap<Element>& domains, const C& context)
{
    auto result = SimpleScopedDomainViewMap<Element, C> {};
    result.reserve(domains.size());

    for (const auto& [element, payload] : domains)
        result.emplace(make_view(element, context), to_variable_domain_view_list(payload, context));

    return result;
}

template<typename Element, typename C>
ScopedDomainViewMap<Element, C> to_scoped_domain_view_map(const ScopedDomainMap<Element>& domains, const C& context)
{
    auto result = ScopedDomainViewMap<Element, C> {};
    result.reserve(domains.size());

    for (const auto& [element, domain] : domains)
    {
        result.emplace(make_view(element, context),
                       SimpleScopedDomainView<Element, C> {
                           make_view(domain.element, context),
                           to_variable_domain_view_list(domain.payload, context),
                       });
    }

    return result;
}

template<typename C>
ConjunctiveConditionDomainView<C> to_conjunctive_condition_domain_view(const ConjunctiveConditionDomain& domain, const C& context)
{
    return ConjunctiveConditionDomainView<C> {
        make_view(domain.element, context),
        to_variable_domain_view_list(domain.payload, context),
    };
}

template<typename C>
ConjunctiveEffectDomainView<C> to_conjunctive_effect_domain_view(const ConjunctiveEffectDomain& domain, const C& context)
{
    return ConjunctiveEffectDomainView<C> {
        make_view(domain.element, context),
        to_variable_domain_view_list(domain.payload, context),
    };
}

template<typename C>
ConditionalEffectDomainView<C> to_conditional_effect_domain_view(const ConditionalEffectDomain& domain, const C& context)
{
    return ConditionalEffectDomainView<C> {
        make_view(domain.element, context),
        ConditionalEffectDomainViewData<C> {
            to_conjunctive_condition_domain_view(domain.payload.condition_domain, context),
            to_conjunctive_effect_domain_view(domain.payload.effect_domain, context),
        },
    };
}

template<typename C>
ConditionalEffectDomainViewMap<C> to_conditional_effect_domain_view_map(const ConditionalEffectDomainMap& domains, const C& context)
{
    auto result = ConditionalEffectDomainViewMap<C> {};
    result.reserve(domains.size());

    for (const auto& [element, domain] : domains)
        result.emplace(make_view(element, context), to_conditional_effect_domain_view(domain, context));

    return result;
}

template<typename C>
ActionDomainViewMap<C> to_action_domain_view_map(const ActionDomainMap& domains, const C& context)
{
    auto result = ActionDomainViewMap<C> {};
    result.reserve(domains.size());

    for (const auto& [element, domain] : domains)
    {
        result.emplace(make_view(element, context),
                       ActionDomainView<C> {
                           make_view(domain.element, context),
                           ActionDomainViewData<C> {
                               to_conjunctive_condition_domain_view(domain.payload.precondition_domain, context),
                               to_conditional_effect_domain_view_map(domain.payload.effect_domains, context),
                           },
                       });
    }

    return result;
}

}  // namespace

ProgramVariableDomainsView compute_variable_domain_views(const ProgramVariableDomains& domains, const formalism::datalog::Repository& repository)
{
    using C = ProgramVariableDomainsView::C;

    return ProgramVariableDomainsView {
        to_simple_scoped_domain_view_map<formalism::Predicate<formalism::StaticTag>, C>(domains.static_predicate_domains, repository),
        to_simple_scoped_domain_view_map<formalism::Predicate<formalism::FluentTag>, C>(domains.fluent_predicate_domains, repository),
        to_simple_scoped_domain_view_map<formalism::Function<formalism::StaticTag>, C>(domains.static_function_domains, repository),
        to_simple_scoped_domain_view_map<formalism::Function<formalism::FluentTag>, C>(domains.fluent_function_domains, repository),
        to_scoped_domain_view_map<formalism::datalog::Rule, C>(domains.rule_domains, repository),
    };
}

TaskVariableDomainsView compute_variable_domain_views(const TaskVariableDomains& domains, const formalism::planning::Repository& repository)
{
    using C = TaskVariableDomainsView::C;

    return TaskVariableDomainsView {
        to_simple_scoped_domain_view_map<formalism::Predicate<formalism::StaticTag>, C>(domains.static_predicate_domains, repository),
        to_simple_scoped_domain_view_map<formalism::Predicate<formalism::FluentTag>, C>(domains.fluent_predicate_domains, repository),
        to_simple_scoped_domain_view_map<formalism::Predicate<formalism::DerivedTag>, C>(domains.derived_predicate_domains, repository),
        to_simple_scoped_domain_view_map<formalism::Function<formalism::StaticTag>, C>(domains.static_function_domains, repository),
        to_simple_scoped_domain_view_map<formalism::Function<formalism::FluentTag>, C>(domains.fluent_function_domains, repository),
        to_action_domain_view_map<C>(domains.action_domains, repository),
        to_scoped_domain_view_map<formalism::planning::Axiom, C>(domains.axiom_domains, repository),
    };
}

}  // namespace tyr::analysis