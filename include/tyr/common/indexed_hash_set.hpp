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
 *<
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef TYR_COMMON_INDEXED_HASH_SET_HPP_
#define TYR_COMMON_INDEXED_HASH_SET_HPP_

#include "tyr/common/equal_to.hpp"
#include "tyr/common/hash.hpp"
#include "tyr/common/index_mixins.hpp"
#include "tyr/common/segmented_vector.hpp"
#include "tyr/common/types.hpp"

#include <concepts>
#include <gtl/phmap.hpp>
#include <memory>
#include <optional>
#include <vector>

namespace tyr
{

template<typename T, typename H = Hash<Data<T>>, typename E = EqualTo<Data<T>>, size_t FirstSegmentSize = 32>
class IndexedHashSet
{
    static_assert(bit::is_power_of_two(FirstSegmentSize));
    static_assert(std::is_trivially_copyable_v<Data<T>>);
    static_assert(std::is_default_constructible_v<Data<T>>);

private:
    struct IndexableHash;
    struct IndexableEqualTo;

    using VectorType = SegmentedVector<Data<T>, FirstSegmentSize>;

public:
    IndexedHashSet() : m_storage(std::make_unique<VectorType>()), m_set(0, IndexableHash(*m_storage), IndexableEqualTo(*m_storage)) {}
    IndexedHashSet(const IndexedHashSet& other) = delete;
    IndexedHashSet& operator=(const IndexedHashSet& other) = delete;
    IndexedHashSet(IndexedHashSet&& other) = default;
    IndexedHashSet& operator=(IndexedHashSet&& other) = default;

    std::optional<Index<T>> find(const Data<T>& value) const
    {
        if (auto it = m_set.find(value); it != m_set.end())
            return *it;

        return std::nullopt;
    }

    Index<T> insert(const Data<T>& value)
    {
        if (auto it = m_set.find(value); it != m_set.end())
            return *it;

        Index<T> idx(static_cast<uint_t>(m_storage->size()));
        m_storage->push_back(value);
        m_set.emplace(idx);
        return idx;
    }

    const Data<T>& operator[](Index<T> idx) const noexcept { return (*m_storage)[uint_t(idx)]; }

    std::size_t size() const noexcept { return m_storage->size(); }

private:
    class IndexableHash
    {
    private:
        const VectorType* m_storage;
        H m_hash;

    public:
        using is_transparent = void;

        IndexableHash() noexcept : m_storage(nullptr) {}
        explicit IndexableHash(const VectorType& storage) noexcept : m_storage(&storage) {}

        size_t operator()(Index<T> el) const noexcept { return m_hash((*m_storage)[uint_t(el)]); }
        size_t operator()(const Data<T>& el) const noexcept { return m_hash(el); }
    };

    class IndexableEqualTo
    {
    private:
        const VectorType* m_storage;
        E m_equal_to;

    public:
        using is_transparent = void;

        IndexableEqualTo() noexcept : m_storage(nullptr), m_equal_to() {}
        explicit IndexableEqualTo(const VectorType& storage) noexcept : m_storage(&storage), m_equal_to() {}

        bool operator()(Index<T> lhs, Index<T> rhs) const noexcept { return m_equal_to((*m_storage)[uint_t(lhs)], (*m_storage)[uint_t(rhs)]); }
        bool operator()(const Data<T>& lhs, Index<T> rhs) const noexcept { return m_equal_to(lhs, (*m_storage)[uint_t(rhs)]); }
        bool operator()(Index<T> lhs, const Data<T>& rhs) const noexcept { return m_equal_to((*m_storage)[uint_t(lhs)], rhs); }
        bool operator()(const Data<T>& lhs, const Data<T>& rhs) const noexcept { return m_equal_to(lhs, rhs); }
    };

    std::unique_ptr<VectorType> m_storage;
    gtl::flat_hash_set<Index<T>, IndexableHash, IndexableEqualTo> m_set;
};
}

#endif