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

#include "tyr/datalog/fact_sets.hpp"
#include "tyr/formalism/datalog/formatter.hpp"
#include "tyr/formalism/datalog/repository.hpp"

#include <boost/dynamic_bitset.hpp>
#include <limits>

namespace f = tyr::formalism;
namespace fd = tyr::formalism::datalog;

namespace tyr::datalog
{

/**
 * PredicateFactSet
 */

template<f::FactKind T>
PredicateFactSet<T>::PredicateFactSet(fd::PredicateView<T> predicate, const fd::Repository& repository) :
    m_predicate(predicate),
    m_repository(repository),
    m_predicate_index(m_predicate.get_index()),
    m_bindings(),
    m_bitset()
{
}

template<f::FactKind T>
void PredicateFactSet<T>::reset() noexcept
{
    m_bindings.clear();
    m_bitset.reset();
}

template<f::FactKind T>
bool PredicateFactSet<T>::insert(const PredicateFactSet<T>& other)
{
    return insert(other.get_bindings());
}

template<f::FactKind T>
bool PredicateFactSet<T>::insert(fd::GroundAtomView<T> ground_atom)
{
    return insert(ground_atom.get_row());
}

template<f::FactKind T>
bool PredicateFactSet<T>::insert(fd::PredicateBindingView<T> binding)
{
    const auto i = uint_t(binding.get_index().row);

    if (!tyr::test(i, m_bitset))
    {
        tyr::set(i, true, m_bitset);
        m_bindings.push_back(binding.get_index().row);
        return true;
    }

    return false;
}

template<f::FactKind T>
bool PredicateFactSet<T>::insert(fd::PredicateBindingForwardRangeView<T> bindings)
{
    auto changed = false;
    for (const auto binding : bindings)
        changed |= insert(binding);
    return changed;
}

template<f::FactKind T>
bool PredicateFactSet<T>::insert(const std::vector<fd::PredicateBindingView<T>>& bindings)
{
    auto changed = false;
    for (const auto binding : bindings)
        changed |= insert(binding);
    return changed;
}

template<f::FactKind T>
bool PredicateFactSet<T>::contains(fd::PredicateBindingView<T> binding) const noexcept
{
    return tyr::test(uint_t(binding.get_index().row), m_bitset);
}

template<f::FactKind T>
fd::PredicateBindingForwardRangeView<T> PredicateFactSet<T>::get_bindings() const noexcept
{
    return make_view(f::RelationBindingsForwardRange { m_predicate_index, m_bindings }, m_repository);
}

template class PredicateFactSet<f::StaticTag>;
template class PredicateFactSet<f::FluentTag>;

/**
 * PredicateFactSets
 */

template<f::FactKind T>
PredicateFactSets<T>::PredicateFactSets(fd::PredicateListView<T> predicates, const fd::Repository& repository) : m_sets()
{
    /* Validate inputs. */
    for (uint_t i = 0; i < predicates.size(); ++i)
        assert(uint_t(predicates[i].get_index()) == i);

    /* Initialize sets. */
    for (const auto predicate : predicates)
        m_sets.emplace_back(PredicateFactSet<T>(predicate, repository));
}

template<f::FactKind T>
void PredicateFactSets<T>::reset() noexcept
{
    for (auto& set : m_sets)
        set.reset();
}

template<f::FactKind T>
bool PredicateFactSets<T>::insert(const PredicateFactSets<T>& other)
{
    assert(m_sets.size() == other.m_sets.size());

    auto changed = false;
    for (uint_t i = 0; i < m_sets.size(); ++i)
        changed |= m_sets[i].insert(other.m_sets[i]);
    return changed;
}

template<f::FactKind T>
bool PredicateFactSets<T>::insert(fd::GroundAtomView<T> ground_atom)
{
    return insert(ground_atom.get_row());
}

template<f::FactKind T>
bool PredicateFactSets<T>::insert(fd::PredicateBindingView<T> binding)
{
    return m_sets[uint_t(binding.get_index().relation)].insert(binding);
}

template<f::FactKind T>
bool PredicateFactSets<T>::insert(fd::PredicateBindingForwardRangeView<T> bindings)
{
    auto changed = false;
    for (const auto binding : bindings)
        changed |= insert(binding);
    return changed;
}

template<f::FactKind T>
bool PredicateFactSets<T>::contains(fd::PredicateBindingView<T> binding) const noexcept
{
    return m_sets[uint_t(binding.get_index().relation)].contains(binding);
}

template<f::FactKind T>
const std::vector<PredicateFactSet<T>>& PredicateFactSets<T>::get_sets() const noexcept
{
    return m_sets;
}

template class PredicateFactSets<f::StaticTag>;
template class PredicateFactSets<f::FluentTag>;

/**
 * FunctionFactSet
 */

template<f::FactKind T>
FunctionFactSet<T>::FunctionFactSet(fd::FunctionView<T> function, const fd::Repository& repository) :
    m_function(function),
    m_repository(repository),
    m_function_index(function.get_index()),
    m_bindings(),
    m_values()
{
}

template<f::FactKind T>
void FunctionFactSet<T>::reset() noexcept
{
    m_remap.clear();
    m_bindings.clear();
    m_values.clear();
}

template<f::FactKind T>
bool FunctionFactSet<T>::insert(const FunctionFactSet& other)
{
    return insert(other.get_bindings(), other.get_values());
}

template<f::FactKind T>
bool FunctionFactSet<T>::insert(fd::FunctionBindingView<T> binding, ClosedInterval<float_t> interval)
{
    const auto i = uint_t(binding.get_index().row);

    if (i < m_remap.size())
    {
        const auto pos = m_remap[i];
        if (pos != std::numeric_limits<uint_t>::max())
        {
            const auto old_interval = m_values[pos];
            const auto new_interval = hull(old_interval, interval);
            m_values[pos] = new_interval;
            return old_interval != new_interval;
        }
    }

    const auto pos = uint_t(m_bindings.size());
    tyr::set(i, pos, m_remap, std::numeric_limits<uint_t>::max());

    m_bindings.push_back(binding.get_index().row);
    m_values.push_back(interval);
    return true;
}

template<f::FactKind T>
bool FunctionFactSet<T>::insert(fd::FunctionBindingView<T> binding, float_t value)
{
    return insert(binding, ClosedInterval<float_t>(value, value));
}

template<f::FactKind T>
bool FunctionFactSet<T>::insert(fd::GroundFunctionTermView<T> fterm, ClosedInterval<float_t> interval)
{
    return insert(fterm.get_row(), interval);
}

template<f::FactKind T>
bool FunctionFactSet<T>::insert(fd::GroundFunctionTermView<T> fterm, float_t value)
{
    return insert(fterm.get_row(), value);
}

template<f::FactKind T>
bool FunctionFactSet<T>::insert(fd::FunctionBindingRandomAccessRangeView<T> bindings, const std::vector<ClosedInterval<float_t>>& intervals)
{
    assert(bindings.size() == intervals.size());

    auto changed = false;
    for (uint_t i = 0; i < bindings.size(); ++i)
        changed |= insert(bindings[i], intervals[i]);
    return changed;
}

template<f::FactKind T>
bool FunctionFactSet<T>::insert(fd::FunctionBindingRandomAccessRangeView<T> bindings, const std::vector<float_t>& values)
{
    assert(bindings.size() == values.size());

    auto changed = false;
    for (uint_t i = 0; i < bindings.size(); ++i)
        changed |= insert(bindings[i], values[i]);
    return changed;
}

template<f::FactKind T>
bool FunctionFactSet<T>::insert(const std::vector<fd::FunctionBindingView<T>>& bindings, const std::vector<ClosedInterval<float_t>>& intervals)
{
    assert(bindings.size() == intervals.size());

    auto changed = false;
    for (uint_t i = 0; i < bindings.size(); ++i)
        changed |= insert(bindings[i], intervals[i]);
    return changed;
}

template<f::FactKind T>
bool FunctionFactSet<T>::insert(const std::vector<fd::FunctionBindingView<T>>& bindings, const std::vector<float_t>& values)
{
    assert(bindings.size() == values.size());

    auto changed = false;
    for (uint_t i = 0; i < bindings.size(); ++i)
        changed |= insert(bindings[i], values[i]);
    return changed;
}

template<f::FactKind T>
bool FunctionFactSet<T>::insert(fd::GroundFunctionTermValueView<T> fterm_value)
{
    return insert(fterm_value.get_fterm().get_row(), fterm_value.get_value());
}

template<f::FactKind T>
bool FunctionFactSet<T>::insert(fd::GroundFunctionTermValueListView<T> fterm_values)
{
    auto changed = false;
    for (const auto fterm_value : fterm_values)
        changed |= insert(fterm_value);
    return changed;
}

template<f::FactKind T>
ClosedInterval<float_t> FunctionFactSet<T>::operator[](formalism::datalog::FunctionBindingView<T> binding) const noexcept
{
    const auto row = binding.get_index().row;
    const auto i = uint_t(row);

    if (i >= m_remap.size())
        return {};

    return tyr::get(m_remap[i], m_values, ClosedInterval<float_t>());
}

template<f::FactKind T>
ClosedInterval<float_t> FunctionFactSet<T>::operator[](fd::GroundFunctionTermView<T> fterm) const noexcept
{
    return (*this)[fterm.get_row()];
}

template<f::FactKind T>
const std::vector<uint_t>& FunctionFactSet<T>::get_remap() const noexcept
{
    return m_remap;
}

template<f::FactKind T>
fd::FunctionBindingRandomAccessRangeView<T> FunctionFactSet<T>::get_bindings() const noexcept
{
    return make_view(f::RelationBindingsRandomAccessRange { m_function_index, m_bindings }, m_repository);
}

template<f::FactKind T>
const std::vector<ClosedInterval<float_t>>& FunctionFactSet<T>::get_values() const noexcept
{
    return m_values;
}

template class FunctionFactSet<f::StaticTag>;
template class FunctionFactSet<f::FluentTag>;

/**
 * FunctionFactSets
 */

template<f::FactKind T>
FunctionFactSets<T>::FunctionFactSets(fd::FunctionListView<T> functions, const fd::Repository& repository) : m_sets()
{
    /* Validate inputs. */
    for (uint_t i = 0; i < functions.size(); ++i)
        assert(uint_t(functions[i].get_index()) == i);

    /* Initialize sets. */
    for (const auto function : functions)
        m_sets.emplace_back(FunctionFactSet<T>(function, repository));
}

template<f::FactKind T>
void FunctionFactSets<T>::reset() noexcept
{
    for (auto& set : m_sets)
        set.reset();
}

template<f::FactKind T>
bool FunctionFactSets<T>::insert(const FunctionFactSets& other)
{
    assert(m_sets.size() == other.m_sets.size());

    auto changed = false;
    for (uint_t i = 0; i < m_sets.size(); ++i)
        changed |= m_sets[i].insert(other.m_sets[i]);
    return changed;
}

template<f::FactKind T>
bool FunctionFactSets<T>::insert(fd::GroundFunctionTermView<T> function_term, ClosedInterval<float_t> interval)
{
    return m_sets[uint_t(function_term.get_function().get_index())].insert(function_term, interval);
}

template<f::FactKind T>
bool FunctionFactSets<T>::insert(fd::FunctionBindingView<T> binding, ClosedInterval<float_t> interval)
{
    return m_sets[uint_t(binding.get_relation().get_index())].insert(binding, interval);
}

template<f::FactKind T>
bool FunctionFactSets<T>::insert(fd::FunctionBindingView<T> binding, float_t value)
{
    return insert(binding, ClosedInterval<float_t>(value, value));
}

template<f::FactKind T>
bool FunctionFactSets<T>::insert(fd::GroundFunctionTermView<T> function_term, float_t value)
{
    return insert(function_term, ClosedInterval<float_t>(value, value));
}

template<f::FactKind T>
bool FunctionFactSets<T>::insert(fd::GroundFunctionTermListView<T> function_terms, const std::vector<float_t>& values)
{
    assert(function_terms.size() == values.size());

    auto changed = false;
    for (size_t i = 0; i < function_terms.size(); ++i)
        changed |= insert(function_terms[i], values[i]);
    return changed;
}

template<f::FactKind T>
bool FunctionFactSets<T>::insert(fd::GroundFunctionTermValueView<T> fterm_value)
{
    return m_sets[uint_t(fterm_value.get_fterm().get_function().get_index())].insert(fterm_value.get_fterm(), fterm_value.get_value());
}

template<f::FactKind T>
bool FunctionFactSets<T>::insert(fd::GroundFunctionTermValueListView<T> fterm_values)
{
    auto changed = false;
    for (const auto fterm_value : fterm_values)
        changed |= insert(fterm_value);
    return changed;
}

template<f::FactKind T>
ClosedInterval<float_t> FunctionFactSets<T>::operator[](formalism::datalog::FunctionBindingView<T> binding) const noexcept
{
    return m_sets[uint_t(binding.get_relation().get_index())][binding];
}

template<f::FactKind T>
ClosedInterval<float_t> FunctionFactSets<T>::operator[](fd::GroundFunctionTermView<T> fterm) const noexcept
{
    return (*this)[fterm.get_row()];
}

template<f::FactKind T>
const std::vector<FunctionFactSet<T>>& FunctionFactSets<T>::get_sets() const noexcept
{
    return m_sets;
}

template class FunctionFactSets<f::StaticTag>;
template class FunctionFactSets<f::FluentTag>;

/**
 * TaggedFactSets
 */

template<f::FactKind T>
TaggedFactSets<T>::TaggedFactSets(fd::PredicateListView<T> predicates, fd::FunctionListView<T> functions, const fd::Repository& repository) :
    predicate(predicates, repository),
    function(functions, repository)
{
}

template<f::FactKind T>
TaggedFactSets<T>::TaggedFactSets(fd::PredicateListView<T> predicates,
                                  fd::FunctionListView<T> functions,
                                  fd::GroundAtomListView<T> atoms,
                                  fd::GroundFunctionTermValueListView<T> fterm_values,
                                  const fd::Repository& repository) :
    TaggedFactSets(predicates, functions, repository)
{
    for (const auto atom : atoms)
        predicate.insert(atom);
    function.insert(fterm_values);
}

template<f::FactKind T>
void TaggedFactSets<T>::insert(const TaggedFactSets<T>& other)
{
    predicate.insert(other.predicate);
    function.insert(other.function);
}

template<f::FactKind T>
void TaggedFactSets<T>::reset() noexcept
{
    predicate.reset();
    function.reset();
}

template struct TaggedFactSets<f::StaticTag>;
template struct TaggedFactSets<f::FluentTag>;

/**
 * FactSets
 */

FactSets::FactSets(const TaggedFactSets<f::StaticTag>& static_sets, const TaggedFactSets<f::FluentTag>& fluent_sets) noexcept :
    static_sets(static_sets),
    fluent_sets(fluent_sets)
{
}

template<f::FactKind T>
const TaggedFactSets<T>& FactSets::get() const noexcept
{
    if constexpr (std::is_same_v<T, f::StaticTag>)
        return static_sets;
    else if constexpr (std::is_same_v<T, f::FluentTag>)
        return fluent_sets;
    else
        static_assert(dependent_false<T>::value, "Missing case");
}

template const TaggedFactSets<f::StaticTag>& FactSets::get<f::StaticTag>() const noexcept;
template const TaggedFactSets<f::FluentTag>& FactSets::get<f::FluentTag>() const noexcept;

}
