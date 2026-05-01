#ifndef TYR_SOLVER_POLICIES_NUMERIC_SUPPORT_HPP_
#define TYR_SOLVER_POLICIES_NUMERIC_SUPPORT_HPP_

#include "tyr/common/closed_interval.hpp"
#include "tyr/common/config.hpp"
#include "tyr/common/equal_to.hpp"
#include "tyr/datalog/fact_sets.hpp"
#include "tyr/datalog/policies/annotation_types.hpp"
#include "tyr/formalism/datalog/repository.hpp"
#include "tyr/formalism/datalog/views.hpp"

#include <algorithm>
#include <cassert>
#include <limits>
#include <vector>

namespace tyr::datalog
{
class NumericSupportSelectorWorkspace
{
public:
    struct SelectionEntry
    {
        formalism::datalog::FunctionBindingView<formalism::FluentTag> binding;
        ClosedInterval<float_t> interval;
        const NumericIntervalAnnotation* annotation;
        Cost cost;

        bool operator<(const SelectionEntry& other) const noexcept { return cost < other.cost; }
    };

    void clear() noexcept;

    std::vector<SelectionEntry> selection;
};

class NumericSupportSelector
{
public:
    NumericSupportSelector(const FactSets& fact_sets, const NumericIntervalAnnotations& annotations);

    ClosedInterval<float_t> select_fluent_interval(formalism::datalog::FunctionBindingView<formalism::FluentTag> binding,
                                                   std::vector<NumericSupportSelectorWorkspace::SelectionEntry>& selection) const;

    template<typename AggregationFunction>
    Cost
    get_constraint_cost(formalism::datalog::GroundBooleanOperatorView constraint, NumericSupportSelectorWorkspace& workspace, AggregationFunction agg) const
    {
        return get_greedy_support_cost(workspace.selection, agg, [&](auto& selection) { return is_supported(constraint, selection); });
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
                if (is_available(entry.binding, candidate.interval) && get_cost(candidate.annotation) <= entry.cost
                    && subset(candidate.interval, entry.interval))
                    callback(entry.binding, candidate.interval, candidate.annotation);
        }

        return cost;
    }

private:
    template<typename AggregationFunction, typename IsSupported>
    Cost
    get_greedy_support_cost(std::vector<NumericSupportSelectorWorkspace::SelectionEntry>& selection, AggregationFunction agg, IsSupported is_supported) const
    {
        selection.clear();

        // The first support check evaluates the constraint with current fact intervals.
        // During that evaluation, each referenced fluent function term is lazily added to selection.
        if (!is_supported(selection))
            return std::numeric_limits<Cost>::max();

        // Refine cheaper function supports first. Annotation entries for each binding are already sorted by cost.
        std::sort(selection.begin(), selection.end());

        for (size_t pos = 0; pos < selection.size(); ++pos)
        {
            const auto binding = selection[pos].binding;
            const auto* entries = find_entries(binding);
            assert(entries);

            assert(std::is_sorted(entries->begin(),
                                  entries->end(),
                                  [](const auto& lhs, const auto& rhs) { return get_cost(lhs.annotation) < get_cost(rhs.annotation); }));

            const auto end = std::upper_bound(entries->begin(),
                                              entries->end(),
                                              selection[pos].cost,
                                              [](Cost cost, const auto& entry) { return cost < get_cost(entry.annotation); });

            // Try only witnesses that do not exceed the current support cost. Since entries are cost-sorted,
            // the first candidate that keeps the full constraint supported is the cheapest local replacement.
            for (auto it = entries->begin(); it != end; ++it)
            {
                const auto& candidate = *it;
                if (!is_available(binding, candidate.interval))
                    continue;

                const auto candidate_cost = get_cost(candidate.annotation);
                const auto old_entry = selection[pos];

                selection[pos] = NumericSupportSelectorWorkspace::SelectionEntry { binding, candidate.interval, &candidate, candidate_cost };

                if (is_supported(selection))
                    break;

                selection[pos] = old_entry;
            }
        }

        auto cost = AggregationFunction::identity();
        for (const auto& entry : selection)
            cost = agg(cost, entry.cost);

        return cost;
    }

    bool is_supported(formalism::datalog::GroundBooleanOperatorView element, std::vector<NumericSupportSelectorWorkspace::SelectionEntry>& selection) const;

    const NumericIntervalAnnotations::Entries* find_entries(formalism::datalog::FunctionBindingView<formalism::FluentTag> binding) const;
    ClosedInterval<float_t> current_interval(formalism::datalog::FunctionBindingView<formalism::FluentTag> binding) const;
    Cost get_current_interval_cost(formalism::datalog::FunctionBindingView<formalism::FluentTag> binding, ClosedInterval<float_t> current) const;
    bool is_available(formalism::datalog::FunctionBindingView<formalism::FluentTag> binding, ClosedInterval<float_t> interval) const;
    NumericSupportSelectorWorkspace::SelectionEntry* find_selection_entry(formalism::datalog::FunctionBindingView<formalism::FluentTag> binding,
                                                                          std::vector<NumericSupportSelectorWorkspace::SelectionEntry>& selection) const;

    FactSets m_fact_sets;
    const NumericIntervalAnnotations& m_annotations;
    EqualTo<formalism::datalog::FunctionBindingView<formalism::FluentTag>> m_binding_equal;
};
}

#endif
