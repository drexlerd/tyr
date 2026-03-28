/*
 * Copyright (C) 2025 Dominik Drexler
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#ifndef TYR_COMMON_RAW_VECTOR_SET_HPP_
#define TYR_COMMON_RAW_VECTOR_SET_HPP_

#include "tyr/common/declarations.hpp"
#include "tyr/common/hash.hpp"
#include "tyr/common/raw_vector_pool.hpp"

#include <algorithm>
#include <cassert>
#include <memory>
#include <optional>
#include <vector>

namespace tyr
{

template<std::unsigned_integral Block, size_t FirstSegmentSize = 1024>
class RawVectorSet
{
public:
    RawVectorSet() : m_pool(std::make_shared<RawVectorPool<Block, FirstSegmentSize>>()), m_set(0, IndexableHash(m_pool), IndexableEqualTo(m_pool)) {}

    RawVectorSet(const RawVectorSet&) = delete;
    RawVectorSet& operator=(const RawVectorSet&) = delete;
    RawVectorSet(RawVectorSet&&) = default;
    RawVectorSet& operator=(RawVectorSet&&) = default;

    std::optional<uint_t> find(const std::vector<Block>& value) const
    {
        if (auto it = m_set.find(value); it != m_set.end())
            return *it;
        return std::nullopt;
    }

    uint_t insert(const std::vector<Block>& value)
    {
        if (auto it = m_set.find(value); it != m_set.end())
            return *it;

        const uint_t idx = static_cast<uint_t>(m_pool->size());
        m_pool->insert(value);
        m_set.emplace(idx);
        return idx;
    }

    RawVectorView<Block> operator[](uint_t idx) noexcept { return (*m_pool)[idx]; }

    RawVectorView<const Block> operator[](uint_t idx) const noexcept { return (*m_pool)[idx]; }

    size_t size() const noexcept { return m_pool->size(); }

    void clear() noexcept
    {
        m_pool->clear();
        m_set.clear();
    }

private:
    struct IndexableHash
    {
        using is_transparent = void;

        std::shared_ptr<RawVectorPool<Block, FirstSegmentSize>> pool;

        IndexableHash() noexcept : pool(nullptr) {}
        explicit IndexableHash(std::shared_ptr<RawVectorPool<Block, FirstSegmentSize>> pool) noexcept : pool(std::move(pool)) {}

        static size_t hash(const Block* data, size_t len) noexcept
        {
            size_t seed = len;
            for (size_t i = 0; i < len; ++i)
                tyr::hash_combine(seed, data[i]);
            return seed;
        }

        size_t operator()(uint_t idx) const noexcept
        {
            const auto view = (*pool)[idx];
            return hash(view.data(), view.size());
        }

        size_t operator()(const std::vector<Block>& value) const noexcept { return hash(value.data(), value.size()); }
    };

    struct IndexableEqualTo
    {
        using is_transparent = void;

        std::shared_ptr<RawVectorPool<Block, FirstSegmentSize>> pool;

        IndexableEqualTo() noexcept : pool(nullptr) {}
        explicit IndexableEqualTo(std::shared_ptr<RawVectorPool<Block, FirstSegmentSize>> pool) noexcept : pool(std::move(pool)) {}

        static bool equal_to(const Block* lhs, size_t lhs_size, const Block* rhs, size_t rhs_size) noexcept
        {
            return lhs_size == rhs_size && std::equal(lhs, lhs + lhs_size, rhs);
        }

        bool operator()(uint_t lhs, uint_t rhs) const noexcept
        {
            const auto lhs_view = (*pool)[lhs];
            const auto rhs_view = (*pool)[rhs];
            return equal_to(lhs_view.data(), lhs_view.size(), rhs_view.data(), rhs_view.size());
        }

        bool operator()(const std::vector<Block>& lhs, uint_t rhs) const noexcept
        {
            const auto rhs_view = (*pool)[rhs];
            return equal_to(lhs.data(), lhs.size(), rhs_view.data(), rhs_view.size());
        }

        bool operator()(uint_t lhs, const std::vector<Block>& rhs) const noexcept
        {
            const auto lhs_view = (*pool)[lhs];
            return equal_to(lhs_view.data(), lhs_view.size(), rhs.data(), rhs.size());
        }

        bool operator()(const std::vector<Block>& lhs, const std::vector<Block>& rhs) const noexcept
        {
            return equal_to(lhs.data(), lhs.size(), rhs.data(), rhs.size());
        }
    };

    std::shared_ptr<RawVectorPool<Block, FirstSegmentSize>> m_pool;
    gtl::flat_hash_set<uint_t, IndexableHash, IndexableEqualTo> m_set;
};

}  // namespace tyr

#endif