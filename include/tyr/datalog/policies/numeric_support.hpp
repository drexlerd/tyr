#ifndef TYR_SOLVER_POLICIES_NUMERIC_SUPPORT_HPP_
#define TYR_SOLVER_POLICIES_NUMERIC_SUPPORT_HPP_

#include "tyr/common/closed_interval.hpp"
#include "tyr/common/comparators.hpp"
#include "tyr/common/config.hpp"
#include "tyr/common/equal_to.hpp"
#include "tyr/common/hash.hpp"
#include "tyr/datalog/fact_sets.hpp"
#include "tyr/datalog/policies/annotation_types.hpp"
#include "tyr/formalism/arithmetic_operator_utils.hpp"
#include "tyr/formalism/boolean_operator_utils.hpp"
#include "tyr/formalism/datalog/expression_properties.hpp"
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
        build_index();
    }

    template<typename AggregationFunction>
    Cost get_constraint_cost(formalism::datalog::GroundBooleanOperatorView constraint, AggregationFunction agg) const
    {
        const auto fterms = formalism::datalog::collect_fterms<formalism::FluentTag>(constraint);
        auto bindings = std::vector<Binding> {};
        bindings.reserve(fterms.size());

        for (const auto fterm : fterms)
            bindings.push_back(fterm.get_row());

        return get_greedy_support_cost(bindings, agg, [&](const auto& selection) { return evaluate(constraint, selection); });
    }

private:
    using Binding = formalism::datalog::FunctionBindingView<formalism::FluentTag>;
    using Function = formalism::datalog::FunctionView<formalism::FluentTag>;
    using BindingIndex = Index<formalism::RelationBinding<formalism::Function<formalism::FluentTag>>>;

    struct EntryRef
    {
        Binding binding;
        const NumericIntervalAnnotation* entry;
    };

    struct BindingRange
    {
        Binding binding;
        float_t initial_value;
        size_t begin;
        size_t end;
    };

    struct SelectionEntry
    {
        Binding binding;
        ClosedInterval<float_t> interval;
        Cost cost;
    };

    using Selection = std::vector<SelectionEntry>;

    template<typename AggregationFunction, typename IsSupported>
    Cost get_greedy_support_cost(const std::vector<Binding>& bindings, AggregationFunction agg, IsSupported is_supported) const
    {
        if (bindings.empty())
            return is_supported(Selection {}) ? Cost(0) : std::numeric_limits<Cost>::max();

        auto selection = Selection {};
        selection.reserve(bindings.size());

        for (const auto binding : bindings)
        {
            const auto* range = find_range(binding);
            if (!range)
                return std::numeric_limits<Cost>::max();

            const auto* entry = select_initial_entry(*range);
            if (!entry)
                return std::numeric_limits<Cost>::max();

            selection.push_back(SelectionEntry { binding, entry->interval, get_cost(entry->annotation) });
        }

        if (!is_supported(selection))
            return std::numeric_limits<Cost>::max();

        for (size_t pos = 0; pos < selection.size(); ++pos)
        {
            const auto binding = selection[pos].binding;
            const auto* range = find_range(binding);
            if (!range)
                return std::numeric_limits<Cost>::max();

            for (auto candidate_pos = range->begin; candidate_pos != range->end; ++candidate_pos)
            {
                const auto* candidate = m_entries[candidate_pos].entry;
                const auto old_interval = selection[pos].interval;
                const auto old_cost = selection[pos].cost;

                selection[pos].interval = candidate->interval;
                selection[pos].cost = get_cost(candidate->annotation);

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

    ClosedInterval<float_t> evaluate(float_t element, const Selection&) const { return ClosedInterval<float_t>(element, element); }

    ClosedInterval<float_t> evaluate(formalism::datalog::GroundFunctionTermView<formalism::StaticTag> element, const Selection&) const
    {
        return m_fact_sets.get<formalism::StaticTag>().function[element];
    }

    ClosedInterval<float_t> evaluate(formalism::datalog::GroundFunctionTermView<formalism::FluentTag> element, const Selection& selection) const
    {
        for (const auto& entry : selection)
            if (m_binding_equal(entry.binding, element.get_row()))
                return entry.interval;

        return ClosedInterval<float_t>();
    }

    ClosedInterval<float_t> evaluate(formalism::datalog::GroundFunctionExpressionView element, const Selection& selection) const
    {
        return visit([&](auto&& arg) { return evaluate(arg, selection); }, element.get_variant());
    }

    ClosedInterval<float_t> evaluate(formalism::datalog::GroundArithmeticOperatorView element, const Selection& selection) const
    {
        return visit([&](auto&& arg) { return evaluate(arg, selection); }, element.get_variant());
    }

    template<formalism::ArithmeticOpKind O>
    ClosedInterval<float_t> evaluate(formalism::datalog::GroundUnaryOperatorView<O> element, const Selection& selection) const
    {
        return formalism::apply(O {}, evaluate(element.get_arg(), selection));
    }

    template<formalism::ArithmeticOpKind O>
    ClosedInterval<float_t> evaluate(formalism::datalog::GroundBinaryOperatorView<O> element, const Selection& selection) const
    {
        return formalism::apply(O {}, evaluate(element.get_lhs(), selection), evaluate(element.get_rhs(), selection));
    }

    template<formalism::ArithmeticOpKind O>
    ClosedInterval<float_t> evaluate(formalism::datalog::GroundMultiOperatorView<O> element, const Selection& selection) const
    {
        const auto child_fexprs = element.get_args();
        return std::accumulate(std::next(child_fexprs.begin()),
                               child_fexprs.end(),
                               evaluate(child_fexprs.front(), selection),
                               [&](const auto& value, const auto& child_expr)
                               { return formalism::apply(O {}, value, evaluate(child_expr, selection)); });
    }

    template<formalism::BooleanOpKind O>
    bool evaluate(formalism::datalog::GroundBinaryOperatorView<O> element, const Selection& selection) const
    {
        return formalism::apply_existential(O {}, evaluate(element.get_lhs(), selection), evaluate(element.get_rhs(), selection));
    }

    bool evaluate(formalism::datalog::GroundBooleanOperatorView element, const Selection& selection) const
    {
        return visit([&](auto&& arg) { return evaluate(arg, selection); }, element.get_variant());
    }

    void build_index()
    {
        m_entries.reserve(m_annotations.size());
        for (const auto& [function, entries] : m_annotations.partitions())
        {
            for (const auto& entry : entries)
                m_entries.push_back(EntryRef { make_binding(function, entry.row), &entry });
        }

        std::sort(m_entries.begin(),
                  m_entries.end(),
                  [&](const auto& lhs, const auto& rhs) { return m_binding_less(lhs.binding, rhs.binding); });

        for (size_t begin = 0; begin < m_entries.size();)
        {
            auto end = begin + 1;
            while (end < m_entries.size() && m_binding_equal(m_entries[begin].binding, m_entries[end].binding))
                ++end;

            const auto binding = m_entries[begin].binding;
            const auto initial_value = get_initial_value(binding);
            m_ranges.push_back(BindingRange { binding, initial_value, begin, end });

            std::sort(m_entries.begin() + begin,
                      m_entries.begin() + end,
                      [&](const auto& lhs, const auto& rhs) { return is_better_greedy_candidate(*lhs.entry, *rhs.entry, initial_value); });

            begin = end;
        }
    }

    const BindingRange* find_range(Binding binding) const
    {
        const auto it = std::lower_bound(m_ranges.begin(),
                                         m_ranges.end(),
                                         binding,
                                         [&](const auto& range, const auto& value) { return m_binding_less(range.binding, value); });

        return it != m_ranges.end() && m_binding_equal(it->binding, binding) ? &*it : nullptr;
    }

    const NumericIntervalAnnotation* select_initial_entry(const BindingRange& range) const
    {
        const auto* best = static_cast<const NumericIntervalAnnotation*>(nullptr);

        for (auto pos = range.begin; pos != range.end; ++pos)
        {
            const auto& entry = *m_entries[pos].entry;

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

            if (entry_width == best_width && distance_to_value(entry.interval, range.initial_value) < distance_to_value(best->interval, range.initial_value))
                best = &entry;
        }

        return best;
    }

    float_t get_initial_value(Binding binding) const
    {
        return m_initial_values.get(binding);
    }

    static Binding make_binding(Function function, Index<formalism::Row> row)
    {
        return make_view(BindingIndex { function.get_index(), row }, function.get_context());
    }

    static bool is_better_greedy_candidate(const NumericIntervalAnnotation& lhs, const NumericIntervalAnnotation& rhs, float_t initial_value)
    {
        const auto lhs_distance = distance_to_value(lhs.interval, initial_value);
        const auto rhs_distance = distance_to_value(rhs.interval, initial_value);
        if (lhs_distance != rhs_distance)
            return lhs_distance < rhs_distance;

        const auto lhs_cost = get_cost(lhs.annotation);
        const auto rhs_cost = get_cost(rhs.annotation);
        if (lhs_cost != rhs_cost)
            return lhs_cost < rhs_cost;

        return width(lhs.interval) < width(rhs.interval);
    }

    FactSets m_fact_sets;
    const NumericIntervalAnnotations& m_annotations;
    const NumericInitialValues& m_initial_values;
    std::vector<EntryRef> m_entries;
    std::vector<BindingRange> m_ranges;
    EqualTo<formalism::datalog::FunctionBindingView<formalism::FluentTag>> m_binding_equal;
    Less<formalism::datalog::FunctionBindingView<formalism::FluentTag>> m_binding_less;
};
}

#endif
