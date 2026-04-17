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

#ifndef TYR_SOLVER_POLICIES_CARE_HPP_
#define TYR_SOLVER_POLICIES_CARE_HPP_

#include "tyr/common/closed_interval.hpp"
#include "tyr/datalog/assignment.hpp"
#include "tyr/datalog/assignment_sets.hpp"
#include "tyr/datalog/assignment_sets_accessor_concept.hpp"
#include "tyr/datalog/fact_sets.hpp"
#include "tyr/datalog/workspaces/facts.hpp"
#include "tyr/formalism/datalog/declarations.hpp"
#include "tyr/formalism/datalog/grounder_decl.hpp"
#include "tyr/formalism/datalog/repository.hpp"

namespace tyr::datalog
{

/**
 * Assignment-set subpolicies
 */

template<formalism::FactKind T>
struct NoCarePredicateAssignmentSetAccessor
{
    const PredicateAssignmentSets<T>& assignment_sets;

    struct Checker
    {
        const PredicateAssignmentSet<T>& set;

        template<typename Assignment>
        bool is_consistent(const Assignment& assignment, bool polarity) const;
    };

    Checker make_checker(Index<formalism::Predicate<T>> predicate) const { return Checker { assignment_sets.at(predicate) }; }
};

template<formalism::FactKind T>
struct CarePredicateAssignmentSetAccessor
{
    const PredicateAssignmentSets<T>& assignment_sets;

    struct Checker
    {
        const PredicateAssignmentSet<T>& set;

        template<typename Assignment>
        bool is_consistent(const Assignment& assignment, bool polarity) const;
    };

    Checker make_checker(Index<formalism::Predicate<T>> predicate) const { return Checker { assignment_sets.at(predicate) }; }
};

template<>
struct CarePredicateAssignmentSetAccessor<formalism::FluentTag>
{
    const PredicateAssignmentSets<formalism::FluentTag>& assignment_sets;
    const PredicateAssignmentSets<formalism::FluentTag>& care_assignment_sets;

    struct Checker
    {
        const PredicateAssignmentSet<formalism::FluentTag>& set;
        const PredicateAssignmentSet<formalism::FluentTag>& care_set;

        template<typename Assignment>
        bool is_consistent(const Assignment& assignment, bool polarity) const;
    };

    Checker make_checker(Index<formalism::Predicate<formalism::FluentTag>> predicate) const
    {
        return Checker { assignment_sets.at(predicate), care_assignment_sets.at(predicate) };
    }
};

template<formalism::FactKind T>
struct NoCareFunctionAssignmentSetAccessor
{
    const FunctionAssignmentSets<T>& assignment_sets;

    struct Checker
    {
        const FunctionAssignmentSet<T>& set;

        bool intersect_interval(const VertexAssignment& assignment, ClosedInterval<float_t>& interval) const;

        bool intersect_interval(const EdgeAssignment& assignment, ClosedInterval<float_t>& interval) const;
    };

    Checker make_checker(Index<formalism::Function<T>> function) const { return Checker { assignment_sets.at(function) }; }
};

template<formalism::FactKind T>
using CareFunctionAssignmentSetAccessor = NoCareFunctionAssignmentSetAccessor<T>;

/**
 * Tagged wrappers
 */

template<formalism::FactKind T>
struct TaggedNoCareAssignmentSetAccessor
{
    NoCarePredicateAssignmentSetAccessor<T> predicate;
    NoCareFunctionAssignmentSetAccessor<T> function;

    explicit TaggedNoCareAssignmentSetAccessor(const TaggedAssignmentSets<T>& assignment_sets) :
        predicate { assignment_sets.predicate },
        function { assignment_sets.function }
    {
    }
};

template<formalism::FactKind T>
struct TaggedCareAssignmentSetAccessor;

template<>
struct TaggedCareAssignmentSetAccessor<formalism::StaticTag>
{
    NoCarePredicateAssignmentSetAccessor<formalism::StaticTag> predicate;
    NoCareFunctionAssignmentSetAccessor<formalism::StaticTag> function;

    explicit TaggedCareAssignmentSetAccessor(const TaggedAssignmentSets<formalism::StaticTag>& assignment_sets) :
        predicate { assignment_sets.predicate },
        function { assignment_sets.function }
    {
    }
};

template<>
struct TaggedCareAssignmentSetAccessor<formalism::FluentTag>
{
    CarePredicateAssignmentSetAccessor<formalism::FluentTag> predicate;
    NoCareFunctionAssignmentSetAccessor<formalism::FluentTag> function;

    TaggedCareAssignmentSetAccessor(const TaggedAssignmentSets<formalism::FluentTag>& assignment_sets,
                                    const PredicateAssignmentSets<formalism::FluentTag>& care_assignment_sets) :
        predicate { assignment_sets.predicate, care_assignment_sets },
        function { assignment_sets.function }
    {
    }
};

/**
 * Combined wrappers
 */

struct NoCareAssignmentSetAccessor
{
    TaggedNoCareAssignmentSetAccessor<formalism::StaticTag> static_policy;
    TaggedNoCareAssignmentSetAccessor<formalism::FluentTag> fluent_policy;

    NoCareAssignmentSetAccessor(const ConstFactsWorkspace& cws, const FactsWorkspace& ws) :
        static_policy { cws.assignment_sets },
        fluent_policy { ws.assignment_sets }
    {
    }

    template<formalism::FactKind T>
    const auto& get() const noexcept
    {
        if constexpr (std::is_same_v<T, formalism::StaticTag>)
            return static_policy;
        else
            return fluent_policy;
    }
};

struct CareAssignmentSetAccessor
{
    TaggedCareAssignmentSetAccessor<formalism::StaticTag> static_policy;
    TaggedCareAssignmentSetAccessor<formalism::FluentTag> fluent_policy;

    CareAssignmentSetAccessor(const ConstFactsWorkspace& cws, const FactsWorkspace& ws) :
        static_policy { cws.assignment_sets },
        fluent_policy { ws.assignment_sets, ws.care_assignment_sets }
    {
    }

    template<formalism::FactKind T>
    const auto& get() const noexcept
    {
        if constexpr (std::is_same_v<T, formalism::StaticTag>)
            return static_policy;
        else
            return fluent_policy;
    }
};

/**
 * Implementations
 */

/**
 * NoCarePredicateAssignmentSetAccessor
 */

template<formalism::FactKind T>
template<typename Assignment>
bool NoCarePredicateAssignmentSetAccessor<T>::Checker::is_consistent(const Assignment& assignment, bool polarity) const
{
    return set.at(assignment) == polarity;
}

/**
 * CarePredicateAssignmentSetAccessor<FluentTag>
 */

template<typename Assignment>
bool CarePredicateAssignmentSetAccessor<formalism::FluentTag>::Checker::is_consistent(const Assignment& assignment, bool polarity) const
{
    // Outside care => don't care.
    if (!care_set.at(assignment))
        return true;

    return set.at(assignment) == polarity;
}

/**
 * NoCareFunctionAssignmentSetAccessor
 */

template<formalism::FactKind T>
bool NoCareFunctionAssignmentSetAccessor<T>::Checker::intersect_interval(const VertexAssignment& assignment, ClosedInterval<float_t>& interval) const
{
    interval = intersect(interval, set.at(assignment));
    return !empty(interval);
}

template<formalism::FactKind T>
bool NoCareFunctionAssignmentSetAccessor<T>::Checker::intersect_interval(const EdgeAssignment& assignment, ClosedInterval<float_t>& interval) const
{
    interval = intersect(interval, set.at(assignment));
    return !empty(interval);
}
}

#endif
