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

class NumericSupportSelectorWorkspace
{
public:
    using Binding = formalism::datalog::FunctionBindingView<formalism::FluentTag>;

    struct SelectionEntry
    {
        Binding binding;
        ClosedInterval<float_t> interval;
        const NumericIntervalAnnotation* annotation;
        Cost cost;
    };

    using Selection = std::vector<SelectionEntry>;

    void clear() noexcept { selection.clear(); }

    Selection selection;
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
    Cost get_constraint_cost(formalism::datalog::GroundBooleanOperatorView constraint, NumericSupportSelectorWorkspace& workspace, AggregationFunction agg) const
    {
        return get_greedy_support_cost(workspace.selection, agg, [&](auto& selection) { return evaluate(constraint, selection); });
    }

    template<typename AggregationFunction, typename Callback>
    Cost for_each_constraint_support(formalism::datalog::GroundBooleanOperatorView constraint,
                                     NumericSupportSelectorWorkspace& workspace,
                                     AggregationFunction agg,
                                     Callback callback) const
    {
        const auto cost = get_constraint_cost(constraint, workspace, agg);
        if (cost == std::numeric_limits<Cost>::max())
            return cost;

        for (const auto& entry : workspace.selection)
        {
            if (entry.annotation)
            {
                callback(entry.binding, entry.interval, entry.annotation->annotation);
                continue;
            }

            const auto* entries = find_entries(entry.binding);
            if (!entries)
                continue;

            for (const auto& candidate : *entries)
                if (is_available(entry.binding, candidate.interval) && get_cost(candidate.annotation) <= entry.cost && subset(candidate.interval, entry.interval))
                    callback(entry.binding, candidate.interval, candidate.annotation);
        }

        return cost;
    }

private:
    using Binding = formalism::datalog::FunctionBindingView<formalism::FluentTag>;
    using SelectionEntry = NumericSupportSelectorWorkspace::SelectionEntry;
    using Selection = NumericSupportSelectorWorkspace::Selection;

    template<typename AggregationFunction, typename IsSupported>
    Cost get_greedy_support_cost(Selection& selection, AggregationFunction agg, IsSupported is_supported) const
    {
        selection.clear();

        if (!is_supported(selection))
            return std::numeric_limits<Cost>::max();

        for (size_t pos = 0; pos < selection.size(); ++pos)
        {
            const auto binding = selection[pos].binding;
            const auto* entries = find_entries(binding);
            assert(entries);

            for (const auto& candidate : *entries)
            {
                if (!is_available(binding, candidate.interval))
                    continue;

                const auto candidate_cost = get_cost(candidate.annotation);
                if (candidate_cost > selection[pos].cost)
                    continue;

                const auto old_interval = selection[pos].interval;
                const auto* old_annotation = selection[pos].annotation;
                const auto old_cost = selection[pos].cost;

                selection[pos].interval = candidate.interval;
                selection[pos].annotation = &candidate;
                selection[pos].cost = candidate_cost;

                if (is_supported(selection))
                    break;

                selection[pos].interval = old_interval;
                selection[pos].annotation = old_annotation;
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

        const auto current = current_interval(element.get_row());
        if (empty(current))
            return ClosedInterval<float_t>();

        const auto cost = get_current_interval_cost(element.get_row(), current);
        if (cost == std::numeric_limits<Cost>::max())
            return ClosedInterval<float_t>();

        selection.push_back(SelectionEntry { element.get_row(), current, nullptr, cost });
        return current;
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
        const auto relation_it = m_annotations.partitions().find(binding.get_relation());
        if (relation_it == m_annotations.partitions().end())
            return nullptr;

        const auto row_it = relation_it->second.find(binding.get_index().row);
        return row_it == relation_it->second.end() ? nullptr : &row_it->second;
    }

    ClosedInterval<float_t> current_interval(Binding binding) const
    {
        return m_fact_sets.get<formalism::FluentTag>().function[binding];
    }

    Cost get_current_interval_cost(Binding binding, ClosedInterval<float_t> current) const
    {
        const auto* entries = find_entries(binding);
        if (!entries)
            return std::numeric_limits<Cost>::max();

        auto best_cost = std::numeric_limits<Cost>::max();

        for (const auto& entry : *entries)
        {
            const auto candidate_cost = get_cost(entry.annotation);
            if (candidate_cost >= best_cost || !is_available(binding, entry.interval))
                continue;

            auto covered = ClosedInterval<float_t>();
            for (const auto& covering_entry : *entries)
                if (get_cost(covering_entry.annotation) <= candidate_cost && is_available(binding, covering_entry.interval))
                    covered = empty(covered) ? covering_entry.interval : hull(covered, covering_entry.interval);

            if (!empty(covered) && subset(current, covered))
                best_cost = candidate_cost;
        }

        return best_cost;
    }

    bool is_available(Binding binding, ClosedInterval<float_t> interval) const
    {
        const auto current = current_interval(binding);
        return !empty(current) && subset(interval, current);
    }

    SelectionEntry* find_selection_entry(Binding binding, Selection& selection) const
    {
        for (auto& entry : selection)
            if (m_binding_equal(entry.binding, binding))
                return &entry;
        return nullptr;
    }

    FactSets m_fact_sets;
    const NumericIntervalAnnotations& m_annotations;
    const NumericInitialValues& m_initial_values;
    EqualTo<formalism::datalog::FunctionBindingView<formalism::FluentTag>> m_binding_equal;
};
}

#endif
