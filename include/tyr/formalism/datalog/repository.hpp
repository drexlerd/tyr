/*
 * Copyright (C) 2025 Dominik Drexler
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

#ifndef TYR_FORMALISM_DATALOG_REPOSITORY_HPP_
#define TYR_FORMALISM_DATALOG_REPOSITORY_HPP_

// Include specialization headers first
#include "tyr/formalism/datalog/datas.hpp"
#include "tyr/formalism/datalog/indices.hpp"
//
#include "tyr/buffer/declarations.hpp"
#include "tyr/buffer/indexed_hash_set.hpp"
#include "tyr/buffer/segmented_buffer.hpp"
#include "tyr/common/equal_to.hpp"
#include "tyr/common/hash.hpp"
#include "tyr/common/tuple.hpp"
#include "tyr/formalism/datalog/declarations.hpp"
#include "tyr/formalism/datalog/relation_table_repository.hpp"
#include "tyr/formalism/function_view.hpp"
#include "tyr/formalism/predicate_view.hpp"

#include <cassert>
#include <optional>
#include <tuple>
#include <type_traits>
#include <utility>

namespace tyr::formalism::datalog
{

class Repository
{
private:
    template<typename T>
    struct Slot
    {
        buffer::IndexedHashSet<T> container {};
        size_t parent_size = 0;
    };

    template<typename T>
    struct RepositoryEntry
    {
        using slot_type = Slot<T>;

        slot_type slot;
    };

    using RepositoryStorage = std::tuple<RepositoryEntry<Variable>,
                                         RepositoryEntry<Object>,
                                         RepositoryEntry<Binding>,
                                         RepositoryEntry<Predicate<StaticTag>>,
                                         RepositoryEntry<Predicate<FluentTag>>,
                                         RepositoryEntry<Atom<StaticTag>>,
                                         RepositoryEntry<Atom<FluentTag>>,
                                         RepositoryEntry<GroundAtom<StaticTag>>,
                                         RepositoryEntry<GroundAtom<FluentTag>>,
                                         RepositoryEntry<Literal<StaticTag>>,
                                         RepositoryEntry<Literal<FluentTag>>,
                                         RepositoryEntry<GroundLiteral<StaticTag>>,
                                         RepositoryEntry<GroundLiteral<FluentTag>>,
                                         RepositoryEntry<Function<StaticTag>>,
                                         RepositoryEntry<Function<FluentTag>>,
                                         RepositoryEntry<FunctionTerm<StaticTag>>,
                                         RepositoryEntry<FunctionTerm<FluentTag>>,
                                         RepositoryEntry<GroundFunctionTerm<StaticTag>>,
                                         RepositoryEntry<GroundFunctionTerm<FluentTag>>,
                                         RepositoryEntry<GroundFunctionTermValue<StaticTag>>,
                                         RepositoryEntry<GroundFunctionTermValue<FluentTag>>,
                                         RepositoryEntry<UnaryOperator<OpSub, Data<FunctionExpression>>>,
                                         RepositoryEntry<BinaryOperator<OpAdd, Data<FunctionExpression>>>,
                                         RepositoryEntry<BinaryOperator<OpSub, Data<FunctionExpression>>>,
                                         RepositoryEntry<BinaryOperator<OpMul, Data<FunctionExpression>>>,
                                         RepositoryEntry<BinaryOperator<OpDiv, Data<FunctionExpression>>>,
                                         RepositoryEntry<MultiOperator<OpAdd, Data<FunctionExpression>>>,
                                         RepositoryEntry<MultiOperator<OpMul, Data<FunctionExpression>>>,
                                         RepositoryEntry<BinaryOperator<OpEq, Data<FunctionExpression>>>,
                                         RepositoryEntry<BinaryOperator<OpNe, Data<FunctionExpression>>>,
                                         RepositoryEntry<BinaryOperator<OpLe, Data<FunctionExpression>>>,
                                         RepositoryEntry<BinaryOperator<OpLt, Data<FunctionExpression>>>,
                                         RepositoryEntry<BinaryOperator<OpGe, Data<FunctionExpression>>>,
                                         RepositoryEntry<BinaryOperator<OpGt, Data<FunctionExpression>>>,
                                         RepositoryEntry<UnaryOperator<OpSub, Data<GroundFunctionExpression>>>,
                                         RepositoryEntry<BinaryOperator<OpAdd, Data<GroundFunctionExpression>>>,
                                         RepositoryEntry<BinaryOperator<OpSub, Data<GroundFunctionExpression>>>,
                                         RepositoryEntry<BinaryOperator<OpMul, Data<GroundFunctionExpression>>>,
                                         RepositoryEntry<BinaryOperator<OpDiv, Data<GroundFunctionExpression>>>,
                                         RepositoryEntry<MultiOperator<OpAdd, Data<GroundFunctionExpression>>>,
                                         RepositoryEntry<MultiOperator<OpMul, Data<GroundFunctionExpression>>>,
                                         RepositoryEntry<BinaryOperator<OpEq, Data<GroundFunctionExpression>>>,
                                         RepositoryEntry<BinaryOperator<OpNe, Data<GroundFunctionExpression>>>,
                                         RepositoryEntry<BinaryOperator<OpLe, Data<GroundFunctionExpression>>>,
                                         RepositoryEntry<BinaryOperator<OpLt, Data<GroundFunctionExpression>>>,
                                         RepositoryEntry<BinaryOperator<OpGe, Data<GroundFunctionExpression>>>,
                                         RepositoryEntry<BinaryOperator<OpGt, Data<GroundFunctionExpression>>>,
                                         RepositoryEntry<ConjunctiveCondition>,
                                         RepositoryEntry<Rule>,
                                         RepositoryEntry<GroundConjunctiveCondition>,
                                         RepositoryEntry<GroundRule>,
                                         RepositoryEntry<Program>>;

    const Repository* m_parent;
    RepositoryStorage m_repository;
    buffer::SegmentedBuffer m_arena;
    RelationTableRepository m_relation_repository;
    size_t m_num_objects;

    /**
     * Clear
     */

    template<typename T>
    void clear_entry(RepositoryEntry<T>& entry) noexcept
    {
        entry.slot.container.clear();
        entry.slot.parent_size = m_parent ? m_parent->template size<T>() : size_t { 0 };
    }

    void clear_entries() noexcept
    {
        std::apply([&](auto&... entry) { (clear_entry(entry), ...); }, m_repository);
    }

public:
    Repository(size_t num_objects, const Repository* parent = nullptr) :
        m_parent(parent),
        m_repository(),
        m_arena(),
        m_relation_repository(num_objects, m_parent ? &m_parent->m_relation_repository : nullptr),
        m_num_objects(num_objects)
    {
        clear_entries();
    }

    template<typename T>
    std::optional<View<Index<T>, Repository>> find_with_hash(const Data<T>& builder, size_t h) const noexcept
    {
        const auto& entry = std::get<RepositoryEntry<T>>(m_repository);
        const auto& container = entry.slot.container;
        assert(h == container.hash(builder) && "The given hash does not match container internal's hash.");

        if (auto ptr = container.find_with_hash(builder, h))
            return View<Index<T>, Repository>(ptr->index, *this);

        return m_parent ? m_parent->template find_with_hash<T>(builder, h) : std::nullopt;
    }

    template<typename T>
    std::optional<View<Index<T>, Repository>> find(const Data<T>& builder) const noexcept
    {
        const auto& entry = std::get<RepositoryEntry<T>>(m_repository);
        const auto& container = entry.slot.container;
        const auto h = container.hash(builder);

        return find_with_hash<T>(builder, h);
    }

    template<typename T>
    std::pair<View<Index<T>, Repository>, bool> get_or_create(Data<T>& builder, buffer::Buffer& buf)
    {
        auto& entry = std::get<RepositoryEntry<T>>(m_repository);
        auto& slot = entry.slot;
        auto& container = slot.container;
        const auto h = container.hash(builder);

        if (m_parent)
            if (auto ptr = m_parent->template find_with_hash<T>(builder, h))
                return { *ptr, false };

        // Manually assign index to continue indexing.
        builder.index.value = slot.parent_size + container.size();

        const auto [ptr, success] = container.insert_with_hash(h, builder, buf, m_arena);

        return { View<Index<T>, Repository>(ptr->index, *this), success };
    }

    /// @brief Access the element with the given index.
    template<typename T>
    const Data<T>& operator[](Index<T> index) const noexcept
    {
        assert(index != Index<T>::max() && "Unassigned index.");

        const auto& entry = std::get<RepositoryEntry<T>>(m_repository);
        const auto& slot = entry.slot;
        const auto parent_size = slot.parent_size;

        // In parent range -> delegate
        if (index.value < parent_size)
        {
            assert(m_parent);
            return (*m_parent)[index];
        }

        // Local range -> shift down
        index.value -= parent_size;

        return slot.container[index];
    }

    template<typename T>
    const Data<T>& front() const
    {
        const auto& entry = std::get<RepositoryEntry<T>>(m_repository);
        const auto& slot = entry.slot;

        if (slot.parent_size > 0)
        {
            assert(m_parent);
            return m_parent->template front<T>();  // recurse to root-most non-empty
        }

        return slot.container.front();
    }

    /// @brief Get the number of stored elements.
    template<typename T>
    size_t size() const noexcept
    {
        const auto& entry = std::get<RepositoryEntry<T>>(m_repository);
        const auto& slot = entry.slot;

        return slot.parent_size + slot.container.size();
    }

    /// @brief Clear the repository but keep memory allocated.
    void clear() noexcept
    {
        m_arena.clear();
        clear_entries();
        m_relation_repository.clear();
    }

    // Get the canonical repository which is repository whose container stores the data of the element corresponding to the given index.
    template<typename T>
    const Repository& get_canonical_context(Index<T> index) const noexcept
    {
        const auto& entry = std::get<RepositoryEntry<T>>(m_repository);

        if (index.value < entry.slot.parent_size)
        {
            assert(m_parent && "Element not found in the repository chain.");
            return m_parent->get_canonical_context(index);
        }

        return *this;
    }

    /**
     * Predicate relation table
     */

    template<FactKind T>
    std::optional<View<std::pair<Index<Predicate<T>>, Index<Binding2>>, Repository>>
    find_with_hash(Index<Predicate<T>> g, const IndexList<Object>& builder, size_t h) const noexcept
    {
        const auto row_or_nullopt = m_relation_repository.find_with_hash(g, builder, h);
        if (row_or_nullopt)
            return make_view(std::make_pair(g.get_index(), *row_or_nullopt), *this);
        return std::nullopt;
    }

    template<FactKind T>
    std::optional<View<std::pair<Index<Predicate<T>>, Index<Binding2>>, Repository>> find(Index<Predicate<T>> g,
                                                                                          const IndexList<Object>& builder) const noexcept
    {
        const auto row_or_nullopt = m_relation_repository.find(g, builder);
        if (row_or_nullopt)
            return make_view(std::make_pair(g.get_index(), *row_or_nullopt), *this);
        return std::nullopt;
    }

    template<FactKind T>
    std::pair<View<std::pair<Index<Predicate<T>>, Index<Binding2>>, Repository>, bool> get_or_create(View<Index<Predicate<T>>, Repository> g,
                                                                                                     const IndexList<Object>& builder)
    {
        const auto [row, success] =
            m_relation_repository.get_or_create(g.get_index(), g.get_arity(), std::max<uint8_t>(1, std::bit_width(m_num_objects)), builder);

        return { make_view(std::make_pair(g.get_index(), row), *this), success };
    }

    template<FactKind T>
    auto operator[](std::pair<Index<Predicate<T>>, Index<Binding2>> index) const noexcept
    {
        return m_relation_repository[index];
    }

    template<FactKind T>
    size_t size(Index<Predicate<T>> g) const noexcept
    {
        return m_relation_repository.size(g);
    }

    template<FactKind T>
    const RelationTableRepository& get_canonical_context(std::pair<Index<Predicate<T>>, Index<Binding2>> index) const noexcept
    {
        return m_relation_repository.get_canonical_context(index);
    }

    /**
     * Function relation table
     */

    template<FactKind T>
    std::optional<View<std::pair<Index<Function<T>>, Index<Binding2>>, Repository>>
    find_with_hash(Index<Function<T>> g, const IndexList<Object>& builder, size_t h) const noexcept
    {
        const auto row_or_nullopt = m_relation_repository.find_with_hash(g, builder, h);
        if (row_or_nullopt)
            return make_view(std::make_pair(g.get_index(), *row_or_nullopt), *this);
        return std::nullopt;
    }

    template<FactKind T>
    std::optional<View<std::pair<Index<Function<T>>, Index<Binding2>>, Repository>> find(Index<Function<T>> g, const IndexList<Object>& builder) const noexcept
    {
        const auto row_or_nullopt = m_relation_repository.find(g, builder);
        if (row_or_nullopt)
            return make_view(std::make_pair(g.get_index(), *row_or_nullopt), *this);
        return std::nullopt;
    }

    template<FactKind T>
    std::pair<View<std::pair<Index<Function<T>>, Index<Binding2>>, Repository>, bool> get_or_create(View<Index<Function<T>>, Repository> g,
                                                                                                    const IndexList<Object>& builder)
    {
        const auto [row, success] =
            m_relation_repository.get_or_create(g.get_index(), g.get_arity(), std::max<uint8_t>(1, std::bit_width(m_num_objects)), builder);

        return { make_view(std::make_pair(g.get_index(), row), *this), success };
    }

    template<FactKind T>
    auto operator[](std::pair<Index<Function<T>>, Index<Binding2>> index) const noexcept
    {
        return m_relation_repository[index];
    }

    template<FactKind T>
    size_t size(Index<Function<T>> g) const noexcept
    {
        return m_relation_repository.size(g);
    }

    template<FactKind T>
    const RelationTableRepository& get_canonical_context(std::pair<Index<Function<T>>, Index<Binding2>> index) const noexcept
    {
        return m_relation_repository.get_canonical_context(index);
    }
};

static_assert(RepositoryConcept<Repository>);
static_assert(Context<Repository>);

}

#endif
