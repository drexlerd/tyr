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

#ifndef TYR_DATALOG_ASSIGNMENT_SETS_ACCESSOR_HPP_
#define TYR_DATALOG_ASSIGNMENT_SETS_ACCESSOR_HPP_

#include "tyr/common/closed_interval.hpp"
#include "tyr/common/semantics.hpp"
#include "tyr/datalog/assignment.hpp"
#include "tyr/datalog/assignment_sets.hpp"
#include "tyr/datalog/workspaces/facts.hpp"
#include "tyr/formalism/datalog/declarations.hpp"
#include "tyr/formalism/datalog/grounder_decl.hpp"
#include "tyr/formalism/datalog/repository.hpp"

namespace tyr::datalog
{

/**
 * Assignment-set subpolicies
 */

template<SemanticTag S, formalism::FactKind T>
struct PredicateAssignmentSetAccessor
{
    const PredicateAssignmentSets<T>& assignment_sets;

    struct Checker
    {
        const PredicateAssignmentSet<T>& set;
        bool polarity;
        size_t arity;

        template<typename Assignment>
        bool is_consistent(const Assignment& assignment) const;
    };

    Checker make_checker(Index<formalism::Predicate<T>> predicate, bool polarity, size_t arity) const
    {
        return Checker { assignment_sets.at(predicate), polarity, arity };
    }
};

template<>
struct PredicateAssignmentSetAccessor<CareSemanticTag, formalism::FluentTag>
{
    const PredicateAssignmentSets<formalism::FluentTag>& assignment_sets;
    const PredicateAssignmentSets<formalism::FluentTag>& care_assignment_sets;

    struct Checker
    {
        const PredicateAssignmentSet<formalism::FluentTag>& set;
        const PredicateAssignmentSet<formalism::FluentTag>& care_set;
        bool polarity;
        size_t arity;

        template<typename Assignment>
        bool is_consistent(const Assignment& assignment) const;
    };

    Checker make_checker(Index<formalism::Predicate<formalism::FluentTag>> predicate, bool polarity, size_t arity) const
    {
        return Checker { assignment_sets.at(predicate), care_assignment_sets.at(predicate), polarity, arity };
    }
};

template<SemanticTag S, formalism::FactKind T>
struct FunctionAssignmentSetAccessor
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

/**
 * Tagged wrappers
 */

template<SemanticTag S, formalism::FactKind T>
struct TaggedAssignmentSetAccessor
{
    PredicateAssignmentSetAccessor<S, T> predicate;
    FunctionAssignmentSetAccessor<S, T> function;

    explicit TaggedAssignmentSetAccessor(const TaggedAssignmentSets<T>& assignment_sets) :
        predicate { assignment_sets.predicate },
        function { assignment_sets.function }
    {
    }
};

template<>
struct TaggedAssignmentSetAccessor<CareSemanticTag, formalism::FluentTag>
{
    PredicateAssignmentSetAccessor<CareSemanticTag, formalism::FluentTag> predicate;
    FunctionAssignmentSetAccessor<CareSemanticTag, formalism::FluentTag> function;

    TaggedAssignmentSetAccessor(const TaggedAssignmentSets<formalism::FluentTag>& assignment_sets,
                                const PredicateAssignmentSets<formalism::FluentTag>& care_assignment_sets) :
        predicate { assignment_sets.predicate, care_assignment_sets },
        function { assignment_sets.function }
    {
    }
};

/**
 * Combined wrappers
 */

template<SemanticTag S>
struct AssignmentSetAccessor
{
    TaggedAssignmentSetAccessor<S, formalism::StaticTag> static_policy;
    TaggedAssignmentSetAccessor<S, formalism::FluentTag> fluent_policy;

    AssignmentSetAccessor(const ConstFactsWorkspace& cws, const FactsWorkspace& ws) :
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

template<>
struct AssignmentSetAccessor<CareSemanticTag>
{
    TaggedAssignmentSetAccessor<CareSemanticTag, formalism::StaticTag> static_policy;
    TaggedAssignmentSetAccessor<CareSemanticTag, formalism::FluentTag> fluent_policy;

    AssignmentSetAccessor(const ConstFactsWorkspace& cws, const FactsWorkspace& ws) :
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
 * PredicateAssignmentSetAccessor
 */

template<SemanticTag S, formalism::FactKind T>
template<typename Assignment>
bool PredicateAssignmentSetAccessor<S, T>::Checker::is_consistent(const Assignment& assignment) const
{
    return set.at(assignment) == polarity;
}

/**
 * PredicateAssignmentSetAccessor<CareSemanticTag, FluentTag>
 */

template<>
inline bool PredicateAssignmentSetAccessor<CareSemanticTag, formalism::FluentTag>::Checker::is_consistent(const VertexAssignment& assignment) const
{
    if (arity > 1)
        return true;

    if (!care_set.at(assignment))
        return true;

    return set.at(assignment) == polarity;
}

template<>
inline bool PredicateAssignmentSetAccessor<CareSemanticTag, formalism::FluentTag>::Checker::is_consistent(const EdgeAssignment& assignment) const
{
    if (arity > 2)
        return true;

    if (!care_set.at(assignment))
        return true;

    return set.at(assignment) == polarity;
}

/**
 * FunctionAssignmentSetAccessor
 */

template<SemanticTag S, formalism::FactKind T>
bool FunctionAssignmentSetAccessor<S, T>::Checker::intersect_interval(const VertexAssignment& assignment, ClosedInterval<float_t>& interval) const
{
    interval = intersect(interval, set.at(assignment));
    return !empty(interval);
}

template<SemanticTag S, formalism::FactKind T>
bool FunctionAssignmentSetAccessor<S, T>::Checker::intersect_interval(const EdgeAssignment& assignment, ClosedInterval<float_t>& interval) const
{
    interval = intersect(interval, set.at(assignment));
    return !empty(interval);
}

}  // namespace tyr::datalog

#endif