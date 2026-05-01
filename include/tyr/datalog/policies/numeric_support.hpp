#ifndef TYR_SOLVER_POLICIES_NUMERIC_SUPPORT_HPP_
#define TYR_SOLVER_POLICIES_NUMERIC_SUPPORT_HPP_

#include "tyr/common/closed_interval.hpp"
#include "tyr/common/config.hpp"
#include "tyr/common/equal_to.hpp"
#include "tyr/common/hash.hpp"
#include "tyr/datalog/fact_sets.hpp"
#include "tyr/datalog/policies/annotation_types.hpp"
#include "tyr/formalism/arithmetic_operator_utils.hpp"
#include "tyr/formalism/boolean_operator_utils.hpp"
#include "tyr/formalism/datalog/program_view.hpp"
#include "tyr/formalism/datalog/repository.hpp"
#include "tyr/formalism/datalog/views.hpp"

#include <algorithm>
#include <limits>
#include <numeric>
#include <vector>

namespace tyr::datalog
{
class NumericInitialValues
{
public:
    using Binding = formalism::datalog::FunctionBindingView<formalism::FluentTag>;

    explicit NumericInitialValues(formalism::datalog::ProgramView program)
    {
        const auto fterm_values = program.get_fterm_values<formalism::FluentTag>();
        m_entries.reserve(fterm_values.size());
        for (const auto fterm_value : fterm_values)
            m_entries.emplace(fterm_value.get_fterm().get_row(), fterm_value.get_value());
    }

    float_t get(Binding binding) const
    {
        const auto it = m_entries.find(binding);
        return it == m_entries.end() ? std::numeric_limits<float_t>::quiet_NaN() : it->second;
    }

private:
    UnorderedMap<Binding, float_t> m_entries;
};

class NumericSupportSelector
{
public:
    NumericSupportSelector(const FactSets& fact_sets, const NumericIntervalAnnotations& annotations, const NumericInitialValues& initial_values) :
        m_fact_sets(fact_sets),
        m_annotations(annotations),
        m_initial_values(initial_values)
    {
    }

    template<typename AggregationFunction>
    Cost get_constraint_cost(formalism::datalog::GroundBooleanOperatorView constraint, AggregationFunction agg) const
    {
        return get_greedy_support_cost(agg, [&](auto& selection) { return evaluate(constraint, selection); });
    }

private:
    using Binding = formalism::datalog::FunctionBindingView<formalism::FluentTag>;

    struct SelectionEntry
    {
        Binding binding;
        ClosedInterval<float_t> interval;
        Cost cost;
    };

    using Selection = std::vector<SelectionEntry>;

    template<typename AggregationFunction, typename IsSupported>
    Cost get_greedy_support_cost(AggregationFunction agg, IsSupported is_supported) const
    {
        auto selection = Selection {};

        if (!is_supported(selection))
            return std::numeric_limits<Cost>::max();

        for (size_t pos = 0; pos < selection.size(); ++pos)
        {
            const auto binding = selection[pos].binding;
            const auto* entries = find_entries(binding);
            assert(entries);

            for (const auto& candidate : *entries)
            {
                if (candidate.row != binding.get_index().row)
                    continue;

                const auto old_interval = selection[pos].interval;
                const auto old_cost = selection[pos].cost;

                selection[pos].interval = candidate.interval;
                selection[pos].cost = get_cost(candidate.annotation);

                if (is_supported(selection))
                    break;

                selection[pos].interval = old_interval;
                selection[pos].cost = old_cost;
            }
        }

        auto cost = AggregationFunction::identity();
        for (const auto& entry : selection)
            cost = agg(cost, entry.cost);

        return cost;
    }

    ClosedInterval<float_t> evaluate(float_t element, Selection&) const { return ClosedInterval<float_t>(element, element); }

    ClosedInterval<float_t> evaluate(formalism::datalog::GroundFunctionTermView<formalism::StaticTag> element, Selection&) const
    {
        return m_fact_sets.get<formalism::StaticTag>().function[element];
    }

    ClosedInterval<float_t> evaluate(formalism::datalog::GroundFunctionTermView<formalism::FluentTag> element, Selection& selection) const
    {
        if (const auto* entry = find_selection_entry(element.get_row(), selection))
            return entry->interval;

        const auto* entries = find_entries(element.get_row());
        if (!entries)
            return ClosedInterval<float_t>();

        const auto* entry = select_initial_entry(element.get_row(), *entries);
        if (!entry)
            return ClosedInterval<float_t>();

        selection.push_back(SelectionEntry { element.get_row(), entry->interval, get_cost(entry->annotation) });
        return entry->interval;
    }

    ClosedInterval<float_t> evaluate(formalism::datalog::GroundFunctionExpressionView element, Selection& selection) const
    {
        return visit([&](auto&& arg) { return evaluate(arg, selection); }, element.get_variant());
    }

    ClosedInterval<float_t> evaluate(formalism::datalog::GroundArithmeticOperatorView element, Selection& selection) const
    {
        return visit([&](auto&& arg) { return evaluate(arg, selection); }, element.get_variant());
    }

    template<formalism::ArithmeticOpKind O>
    ClosedInterval<float_t> evaluate(formalism::datalog::GroundUnaryOperatorView<O> element, Selection& selection) const
    {
        return formalism::apply(O {}, evaluate(element.get_arg(), selection));
    }

    template<formalism::ArithmeticOpKind O>
    ClosedInterval<float_t> evaluate(formalism::datalog::GroundBinaryOperatorView<O> element, Selection& selection) const
    {
        return formalism::apply(O {}, evaluate(element.get_lhs(), selection), evaluate(element.get_rhs(), selection));
    }

    template<formalism::ArithmeticOpKind O>
    ClosedInterval<float_t> evaluate(formalism::datalog::GroundMultiOperatorView<O> element, Selection& selection) const
    {
        const auto child_fexprs = element.get_args();
        return std::accumulate(std::next(child_fexprs.begin()),
                               child_fexprs.end(),
                               evaluate(child_fexprs.front(), selection),
                               [&](const auto& value, const auto& child_expr)
                               { return formalism::apply(O {}, value, evaluate(child_expr, selection)); });
    }

    template<formalism::BooleanOpKind O>
    bool evaluate(formalism::datalog::GroundBinaryOperatorView<O> element, Selection& selection) const
    {
        return formalism::apply_existential(O {}, evaluate(element.get_lhs(), selection), evaluate(element.get_rhs(), selection));
    }

    bool evaluate(formalism::datalog::GroundBooleanOperatorView element, Selection& selection) const
    {
        return visit([&](auto&& arg) { return evaluate(arg, selection); }, element.get_variant());
    }

    const NumericIntervalAnnotations::Entries* find_entries(Binding binding) const
    {
        const auto it = m_annotations.partitions().find(binding.get_relation());
        return it == m_annotations.partitions().end() ? nullptr : &it->second;
    }

    const NumericIntervalAnnotation* select_initial_entry(Binding binding, const NumericIntervalAnnotations::Entries& entries) const
    {
        const auto* best = static_cast<const NumericIntervalAnnotation*>(nullptr);
        const auto initial_value = m_initial_values.get(binding);
        const auto row = binding.get_index().row;

        for (const auto& entry : entries)
        {
            if (entry.row != row)
                continue;

            if (!best)
            {
                best = &entry;
                continue;
            }

            const auto entry_width = width(entry.interval);
            const auto best_width = width(best->interval);
            if (entry_width > best_width)
            {
                best = &entry;
                continue;
            }

            if (entry_width == best_width && is_closer_to_initial(entry.interval, best->interval, initial_value))
                best = &entry;
        }

        return best;
    }

    SelectionEntry* find_selection_entry(Binding binding, Selection& selection) const
    {
        for (auto& entry : selection)
            if (m_binding_equal(entry.binding, binding))
                return &entry;
        return nullptr;
    }

    static bool is_closer_to_initial(ClosedInterval<float_t> lhs, ClosedInterval<float_t> rhs, float_t initial_value)
    {
        return !std::isnan(initial_value) && distance_to_value(lhs, initial_value) < distance_to_value(rhs, initial_value);
    }

    FactSets m_fact_sets;
    const NumericIntervalAnnotations& m_annotations;
    const NumericInitialValues& m_initial_values;
    EqualTo<formalism::datalog::FunctionBindingView<formalism::FluentTag>> m_binding_equal;
};
}

#endif
