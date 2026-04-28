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

#ifndef TYR_FORMALISM_PLANNING_INVARIANTS_CONSTRAINTS_HPP_
#define TYR_FORMALISM_PLANNING_INVARIANTS_CONSTRAINTS_HPP_

#include "tyr/formalism/planning/invariants/invariant.hpp"
#include "tyr/formalism/planning/mutable/atom.hpp"
#include "tyr/formalism/planning/mutable/literal.hpp"

#include <map>
#include <optional>
#include <variant>
#include <vector>

namespace tyr::formalism::planning::invariant
{

struct InvariantParameter
{
    size_t index;

    auto identifying_members() const noexcept { return std::tie(index); }

    friend bool operator==(const InvariantParameter& lhs, const InvariantParameter& rhs) { return tyr::EqualTo<InvariantParameter> {}(lhs, rhs); }

    friend bool operator<(const InvariantParameter& lhs, const InvariantParameter& rhs) { return tyr::Less<InvariantParameter> {}(lhs, rhs); }
};

struct VariableTerm
{
    ParameterIndex index;

    auto identifying_members() const noexcept { return std::tie(index); }

    friend bool operator==(const VariableTerm& lhs, const VariableTerm& rhs) { return tyr::EqualTo<VariableTerm> {}(lhs, rhs); }

    friend bool operator<(const VariableTerm& lhs, const VariableTerm& rhs) { return tyr::Less<VariableTerm> {}(lhs, rhs); }
};

struct ObjectTerm
{
    Index<Object> index;

    auto identifying_members() const noexcept { return std::tie(index); }

    friend bool operator==(const ObjectTerm& lhs, const ObjectTerm& rhs) { return tyr::EqualTo<ObjectTerm> {}(lhs, rhs); }

    friend bool operator<(const ObjectTerm& lhs, const ObjectTerm& rhs) { return tyr::Less<ObjectTerm> {}(lhs, rhs); }
};

using ConstraintTerm = std::variant<InvariantParameter, VariableTerm, ObjectTerm>;

struct InequalityDisjunction
{
    std::vector<std::pair<ConstraintTerm, ConstraintTerm>> parts;

    explicit InequalityDisjunction(std::vector<std::pair<ConstraintTerm, ConstraintTerm>> parts_);
};

struct EqualityConjunction
{
    std::vector<std::pair<ConstraintTerm, ConstraintTerm>> equalities;

    explicit EqualityConjunction(std::vector<std::pair<ConstraintTerm, ConstraintTerm>> equalities_);

    bool is_consistent() const;
    const Map<ConstraintTerm, ConstraintTerm>& get_representative() const;

private:
    mutable bool m_computed = false;
    mutable bool m_consistent = true;
    mutable Map<ConstraintTerm, ConstraintTerm> m_representative;

    void compute() const;
};

struct ConstraintSystem
{
    std::vector<std::vector<EqualityConjunction>> equality_DNFs;
    std::vector<InequalityDisjunction> inequality_disjunctions;
    std::vector<ConstraintTerm> not_constant;

    void add_equality_conjunction(const EqualityConjunction& eq);
    void add_equality_DNF(std::vector<EqualityConjunction> dnf);
    void add_inequality_disjunction(const InequalityDisjunction& disj);
    void add_not_constant(ConstraintTerm term);
    void extend(const ConstraintSystem& other);

    bool is_solvable() const;

private:
    static EqualityConjunction combine_equality_conjunctions(const std::vector<EqualityConjunction>& conjunctions);
};

ConstraintTerm make_constraint_term(ParameterIndex parameter);
ConstraintTerm make_constraint_term(Index<Object> object);
ConstraintTerm make_constraint_term(const Data<Term>& term);
ConstraintTerm make_invariant_parameter_term(size_t index);
EqualityConjunction make_cover_equality_conjunction(const MutableAtom<FluentTag>& pattern, const MutableAtom<FluentTag>& atom, const Invariant& inv);

void ensure_cover(ConstraintSystem& system, const MutableAtom<FluentTag>& pattern, const MutableAtom<FluentTag>& atom, const Invariant& inv);
void ensure_inequality(ConstraintSystem& system, const MutableAtom<FluentTag>& lhs, const MutableAtom<FluentTag>& rhs);
void ensure_conjunction_sat(ConstraintSystem& system, const MutableLiteralList<FluentTag>& lits);

}

#endif
