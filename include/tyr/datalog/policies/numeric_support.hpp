#ifndef TYR_SOLVER_POLICIES_NUMERIC_SUPPORT_HPP_
#define TYR_SOLVER_POLICIES_NUMERIC_SUPPORT_HPP_

#include "tyr/common/closed_interval.hpp"
#include "tyr/common/comparators.hpp"
#include "tyr/common/config.hpp"
#include "tyr/common/equal_to.hpp"
#include "tyr/common/hash.hpp"
#include "tyr/datalog/fact_sets.hpp"
#include "tyr/datalog/policies/annotation_types.hpp"
#include "tyr/formalism/datalog/program_view.hpp"
#include "tyr/formalism/datalog/repository.hpp"
#include "tyr/formalism/datalog/views.hpp"

#include <algorithm>
#include <limits>
#include <tuple>
#include <vector>

namespace tyr::datalog
{
class NumericInitialValues
{
public:
    explicit NumericInitialValues(formalism::datalog::ProgramView program);

    float_t get(formalism::datalog::FunctionBindingView<formalism::FluentTag> binding) const;

private:
    UnorderedMap<formalism::datalog::FunctionBindingView<formalism::FluentTag>, float_t> m_entries;
};

class NumericSupportSelectorWorkspace
{
public:
    struct Score
    {
        float_t distance_to_initial;
        float_t width;
        Cost cost;

        auto identifying_members() const noexcept { return std::tie(distance_to_initial, width, cost); }
    };

    struct SelectionEntry
    {
        formalism::datalog::FunctionBindingView<formalism::FluentTag> binding;
        ClosedInterval<float_t> interval;
        const NumericIntervalAnnotation* annotation;
        Cost cost;
        Score score;

        bool operator<(const SelectionEntry& other) const noexcept { return Less<Score> {}(score, other.score); }
    };

    void clear() noexcept;

    std::vector<SelectionEntry> selection;
    std::vector<SelectionEntry> preferred_selection;
};

class NumericSupportSelector
{
public:
    NumericSupportSelector(const FactSets& fact_sets, const NumericIntervalAnnotations& annotations, const NumericInitialValues& initial_values);

    ClosedInterval<float_t>
    select_fluent_interval(formalism::datalog::FunctionBindingView<formalism::FluentTag> binding,
                           std::vector<NumericSupportSelectorWorkspace::SelectionEntry>& selection) const;

    template<typename AggregationFunction>
    Cost get_constraint_cost(formalism::datalog::GroundBooleanOperatorView constraint, NumericSupportSelectorWorkspace& workspace, AggregationFunction agg) const
    {
        return get_greedy_support_cost(workspace.selection, workspace.preferred_selection, agg, [&](auto& selection) { return is_supported(constraint, selection); });
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
    template<typename AggregationFunction, typename IsSupported>
    Cost get_greedy_support_cost(std::vector<NumericSupportSelectorWorkspace::SelectionEntry>& selection,
                                 std::vector<NumericSupportSelectorWorkspace::SelectionEntry>& candidates,
                                 AggregationFunction agg,
                                 IsSupported is_supported) const
    {
        selection.clear();
        candidates.clear();

        if (!is_supported(selection))
            return std::numeric_limits<Cost>::max();

        for (size_t pos = 0; pos < selection.size(); ++pos)
        {
            const auto binding = selection[pos].binding;
            const auto* entries = find_entries(binding);
            assert(entries);

            candidates.clear();

            for (const auto& candidate : *entries)
            {
                if (!is_available(binding, candidate.interval))
                    continue;

                const auto candidate_cost = get_cost(candidate.annotation);
                if (candidate_cost > selection[pos].cost)
                    continue;

                candidates.push_back(NumericSupportSelectorWorkspace::SelectionEntry {
                    binding,
                    candidate.interval,
                    &candidate,
                    candidate_cost,
                    support_score(binding, candidate.interval, candidate_cost) });
            }

            std::sort(candidates.begin(), candidates.end());

            for (const auto& candidate : candidates)
            {
                const auto old_entry = selection[pos];

                selection[pos] = candidate;

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
    NumericSupportSelectorWorkspace::Score
    support_score(formalism::datalog::FunctionBindingView<formalism::FluentTag> binding, ClosedInterval<float_t> interval, Cost cost) const noexcept;
    bool is_available(formalism::datalog::FunctionBindingView<formalism::FluentTag> binding, ClosedInterval<float_t> interval) const;
    NumericSupportSelectorWorkspace::SelectionEntry*
    find_selection_entry(formalism::datalog::FunctionBindingView<formalism::FluentTag> binding,
                         std::vector<NumericSupportSelectorWorkspace::SelectionEntry>& selection) const;

    FactSets m_fact_sets;
    const NumericIntervalAnnotations& m_annotations;
    const NumericInitialValues& m_initial_values;
    EqualTo<formalism::datalog::FunctionBindingView<formalism::FluentTag>> m_binding_equal;
};
}

#endif
