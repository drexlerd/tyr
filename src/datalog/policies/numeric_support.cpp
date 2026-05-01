#include "tyr/datalog/policies/numeric_support.hpp"

#include "tyr/formalism/arithmetic_operator_utils.hpp"
#include "tyr/formalism/boolean_operator_utils.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <limits>
#include <numeric>

namespace fd = tyr::formalism::datalog;
namespace f = tyr::formalism;

namespace tyr::datalog
{
namespace
{
ClosedInterval<float_t>
evaluate(float_t element, const FactSets&, const NumericSupportSelector&, std::vector<NumericSupportSelectorWorkspace::SelectionEntry>&)
{
    return ClosedInterval<float_t>(element, element);
}

ClosedInterval<float_t>
evaluate(fd::GroundFunctionTermView<f::StaticTag> element,
         const FactSets& fact_sets,
         const NumericSupportSelector&,
         std::vector<NumericSupportSelectorWorkspace::SelectionEntry>&)
{
    return fact_sets.get<f::StaticTag>().function[element];
}

ClosedInterval<float_t>
evaluate(fd::GroundFunctionTermView<f::FluentTag> element,
         const FactSets&,
         const NumericSupportSelector& selector,
         std::vector<NumericSupportSelectorWorkspace::SelectionEntry>& selection)
{
    return selector.select_fluent_interval(element.get_row(), selection);
}

ClosedInterval<float_t>
evaluate(fd::GroundFunctionExpressionView element,
         const FactSets& fact_sets,
         const NumericSupportSelector& selector,
         std::vector<NumericSupportSelectorWorkspace::SelectionEntry>& selection);

ClosedInterval<float_t>
evaluate(fd::GroundArithmeticOperatorView element,
         const FactSets& fact_sets,
         const NumericSupportSelector& selector,
         std::vector<NumericSupportSelectorWorkspace::SelectionEntry>& selection);

template<f::ArithmeticOpKind O>
ClosedInterval<float_t>
evaluate(fd::GroundUnaryOperatorView<O> element,
         const FactSets& fact_sets,
         const NumericSupportSelector& selector,
         std::vector<NumericSupportSelectorWorkspace::SelectionEntry>& selection)
{
    return f::apply(O {}, evaluate(element.get_arg(), fact_sets, selector, selection));
}

template<f::ArithmeticOpKind O>
ClosedInterval<float_t>
evaluate(fd::GroundBinaryOperatorView<O> element,
         const FactSets& fact_sets,
         const NumericSupportSelector& selector,
         std::vector<NumericSupportSelectorWorkspace::SelectionEntry>& selection)
{
    return f::apply(O {}, evaluate(element.get_lhs(), fact_sets, selector, selection), evaluate(element.get_rhs(), fact_sets, selector, selection));
}

template<f::ArithmeticOpKind O>
ClosedInterval<float_t>
evaluate(fd::GroundMultiOperatorView<O> element,
         const FactSets& fact_sets,
         const NumericSupportSelector& selector,
         std::vector<NumericSupportSelectorWorkspace::SelectionEntry>& selection)
{
    const auto child_fexprs = element.get_args();
    return std::accumulate(std::next(child_fexprs.begin()),
                           child_fexprs.end(),
                           evaluate(child_fexprs.front(), fact_sets, selector, selection),
                           [&](const auto& value, const auto& child_expr)
                           { return f::apply(O {}, value, evaluate(child_expr, fact_sets, selector, selection)); });
}

template<f::BooleanOpKind O>
bool evaluate(fd::GroundBinaryOperatorView<O> element,
              const FactSets& fact_sets,
              const NumericSupportSelector& selector,
              std::vector<NumericSupportSelectorWorkspace::SelectionEntry>& selection)
{
    return f::apply_existential(O {},
                                evaluate(element.get_lhs(), fact_sets, selector, selection),
                                evaluate(element.get_rhs(), fact_sets, selector, selection));
}

ClosedInterval<float_t>
evaluate(fd::GroundFunctionExpressionView element,
         const FactSets& fact_sets,
         const NumericSupportSelector& selector,
         std::vector<NumericSupportSelectorWorkspace::SelectionEntry>& selection)
{
    return visit([&](auto&& arg) { return evaluate(arg, fact_sets, selector, selection); }, element.get_variant());
}

ClosedInterval<float_t>
evaluate(fd::GroundArithmeticOperatorView element,
         const FactSets& fact_sets,
         const NumericSupportSelector& selector,
         std::vector<NumericSupportSelectorWorkspace::SelectionEntry>& selection)
{
    return visit([&](auto&& arg) { return evaluate(arg, fact_sets, selector, selection); }, element.get_variant());
}

bool evaluate(fd::GroundBooleanOperatorView element,
              const FactSets& fact_sets,
              const NumericSupportSelector& selector,
              std::vector<NumericSupportSelectorWorkspace::SelectionEntry>& selection)
{
    return visit([&](auto&& arg) { return evaluate(arg, fact_sets, selector, selection); }, element.get_variant());
}
}

NumericInitialValues::NumericInitialValues(fd::ProgramView program)
{
    const auto fterm_values = program.get_fterm_values<f::FluentTag>();
    m_entries.reserve(fterm_values.size());
    for (const auto fterm_value : fterm_values)
        m_entries.emplace(fterm_value.get_fterm().get_row(), fterm_value.get_value());
}

float_t NumericInitialValues::get(fd::FunctionBindingView<f::FluentTag> binding) const
{
    const auto it = m_entries.find(binding);
    return it == m_entries.end() ? std::numeric_limits<float_t>::quiet_NaN() : it->second;
}

void NumericSupportSelectorWorkspace::clear() noexcept
{
    selection.clear();
    preferred_selection.clear();
}

NumericSupportSelector::NumericSupportSelector(const FactSets& fact_sets,
                                               const NumericIntervalAnnotations& annotations,
                                               const NumericInitialValues& initial_values) :
    m_fact_sets(fact_sets),
    m_annotations(annotations),
    m_initial_values(initial_values)
{
}

bool NumericSupportSelector::is_supported(fd::GroundBooleanOperatorView element, std::vector<NumericSupportSelectorWorkspace::SelectionEntry>& selection) const
{
    return evaluate(element, m_fact_sets, *this, selection);
}

ClosedInterval<float_t>
NumericSupportSelector::select_fluent_interval(fd::FunctionBindingView<f::FluentTag> binding,
                                               std::vector<NumericSupportSelectorWorkspace::SelectionEntry>& selection) const
{
    if (const auto* entry = find_selection_entry(binding, selection))
        return entry->interval;

    const auto current = current_interval(binding);
    if (empty(current))
        return ClosedInterval<float_t>();

    const auto cost = get_current_interval_cost(binding, current);
    if (cost == std::numeric_limits<Cost>::max())
        return ClosedInterval<float_t>();

    selection.push_back(NumericSupportSelectorWorkspace::SelectionEntry { binding, current, nullptr, cost, support_score(binding, current, cost) });
    return current;
}

const NumericIntervalAnnotations::Entries*
NumericSupportSelector::find_entries(fd::FunctionBindingView<f::FluentTag> binding) const
{
    const auto relation_it = m_annotations.partitions().find(binding.get_relation());
    if (relation_it == m_annotations.partitions().end())
        return nullptr;

    const auto row_it = relation_it->second.find(binding.get_index().row);
    return row_it == relation_it->second.end() ? nullptr : &row_it->second;
}

ClosedInterval<float_t> NumericSupportSelector::current_interval(fd::FunctionBindingView<f::FluentTag> binding) const
{
    return m_fact_sets.get<f::FluentTag>().function[binding];
}

Cost NumericSupportSelector::get_current_interval_cost(fd::FunctionBindingView<f::FluentTag> binding, ClosedInterval<float_t> current) const
{
    const auto* entries = find_entries(binding);
    if (!entries)
        return std::numeric_limits<Cost>::max();

    assert(std::is_sorted(entries->begin(), entries->end(), [](const auto& lhs, const auto& rhs) { return get_cost(lhs.annotation) < get_cost(rhs.annotation); }));

    auto best_cost = std::numeric_limits<Cost>::max();
    auto covered = ClosedInterval<float_t>();

    for (const auto& entry : *entries)
    {
        const auto candidate_cost = get_cost(entry.annotation);
        if (!is_available(binding, entry.interval))
            continue;

        covered = empty(covered) ? entry.interval : hull(covered, entry.interval);

        if (!empty(covered) && subset(current, covered))
        {
            best_cost = candidate_cost;
            break;
        }
    }

    return best_cost;
}

NumericSupportSelectorWorkspace::Score
NumericSupportSelector::support_score(fd::FunctionBindingView<f::FluentTag> binding, ClosedInterval<float_t> interval, Cost cost) const noexcept
{
    const auto initial_value = m_initial_values.get(binding);
    const auto distance = std::isfinite(initial_value) ? distance_to_value(interval, initial_value) : float_t(0);
    return { distance, width(interval), cost };
}

bool NumericSupportSelector::is_available(fd::FunctionBindingView<f::FluentTag> binding, ClosedInterval<float_t> interval) const
{
    const auto current = current_interval(binding);
    return !empty(current) && subset(interval, current);
}

NumericSupportSelectorWorkspace::SelectionEntry*
NumericSupportSelector::find_selection_entry(fd::FunctionBindingView<f::FluentTag> binding,
                                             std::vector<NumericSupportSelectorWorkspace::SelectionEntry>& selection) const
{
    for (auto& entry : selection)
        if (m_binding_equal(entry.binding, binding))
            return &entry;
    return nullptr;
}

}
