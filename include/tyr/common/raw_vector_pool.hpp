#ifndef TYR_COMMON_RAW_VECTOR_POOL_HPP_
#define TYR_COMMON_RAW_VECTOR_POOL_HPP_

#include "tyr/common/bit.hpp"

#include <bit>
#include <cassert>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>
#include <vector>

namespace tyr
{

template<std::unsigned_integral Block>
class RawVectorView
{
public:
    RawVectorView() noexcept : m_data(nullptr) {}
    explicit RawVectorView(Block* data) noexcept : m_data(data) {}

    size_t size() const noexcept
    {
        assert(m_data);
        return static_cast<size_t>(m_data[0]);
    }

    Block* data() noexcept
    {
        assert(m_data);
        return m_data + 1;
    }

    const Block* data() const noexcept
    {
        assert(m_data);
        return m_data + 1;
    }

    Block& operator[](size_t i) noexcept
    {
        assert(m_data);
        assert(i < size());
        return m_data[1 + i];
    }

    const Block& operator[](size_t i) const noexcept
    {
        assert(m_data);
        assert(i < size());
        return m_data[1 + i];
    }

    Block* raw_data() noexcept { return m_data; }
    const Block* raw_data() const noexcept { return m_data; }

private:
    Block* m_data;
};

template<std::unsigned_integral Block>
class RawVectorView<const Block>
{
public:
    RawVectorView() noexcept : m_data(nullptr) {}
    explicit RawVectorView(const Block* data) noexcept : m_data(data) {}

    size_t size() const noexcept
    {
        assert(m_data);
        return static_cast<size_t>(m_data[0]);
    }

    const Block* data() const noexcept
    {
        assert(m_data);
        return m_data + 1;
    }

    const Block& operator[](size_t i) const noexcept
    {
        assert(m_data);
        assert(i < size());
        return m_data[1 + i];
    }

    const Block* raw_data() const noexcept { return m_data; }

private:
    const Block* m_data;
};

template<std::unsigned_integral Block, size_t VectorsPerSegment = 1024>
class RawVectorPool
{
    static_assert(bit::is_power_of_two(VectorsPerSegment));

private:
    static constexpr size_t next_power_of_two_at_least(size_t n) noexcept
    {
        if (n <= 1)
            return 1;
        return size_t(1) << std::bit_width(n - 1);
    }

    static constexpr size_t slot_width(size_t payload_capacity) noexcept { return 1 + payload_capacity; }

    bool current_segment_has_space() const noexcept { return !m_segment_storage.empty() && m_current_used_slots < VectorsPerSegment; }

    bool current_segment_fits(size_t payload_size) const noexcept { return !m_segment_storage.empty() && m_current_payload_capacity >= payload_size; }

    void append_segment(size_t payload_capacity)
    {
        m_segment_storage.emplace_back(slot_width(payload_capacity) * VectorsPerSegment);
        m_current_payload_capacity = payload_capacity;
        m_current_used_slots = 0;
    }

    void ensure_current_segment(size_t payload_size)
    {
        if (current_segment_fits(payload_size) && current_segment_has_space())
            return;

        size_t new_capacity;
        if (m_segment_storage.empty())
        {
            new_capacity = next_power_of_two_at_least(payload_size);
        }
        else
        {
            new_capacity = m_current_payload_capacity;
            while (new_capacity < payload_size)
                new_capacity *= 2;

            if (new_capacity == m_current_payload_capacity)
                new_capacity *= 2;
        }

        append_segment(new_capacity);
    }

public:
    RawVectorPool() : m_segment_storage(), m_index(), m_current_payload_capacity(0), m_current_used_slots(0) {}

    RawVectorPool(const RawVectorPool&) = delete;
    RawVectorPool& operator=(const RawVectorPool&) = delete;
    RawVectorPool(RawVectorPool&&) = default;
    RawVectorPool& operator=(RawVectorPool&&) = default;

    uint_t insert(const std::vector<Block>& value) { return insert(value.data(), value.size()); }

    uint_t insert(const Block* data, size_t size)
    {
        assert(size <= std::numeric_limits<Block>::max());

        ensure_current_segment(size);

        auto& seg = m_segment_storage.back();
        Block* slot = seg.data() + m_current_used_slots * slot_width(m_current_payload_capacity);
        ++m_current_used_slots;

        slot[0] = static_cast<Block>(size);
        if (size > 0)
            std::memcpy(slot + 1, data, size * sizeof(Block));

        m_index.push_back(slot);
        return static_cast<uint_t>(m_index.size() - 1);
    }

    RawVectorView<Block> operator[](uint_t index) noexcept
    {
        assert(index < m_index.size());
        return RawVectorView<Block>(m_index[index]);
    }

    RawVectorView<const Block> operator[](uint_t index) const noexcept
    {
        assert(index < m_index.size());
        return RawVectorView<const Block>(m_index[index]);
    }

    size_t size() const noexcept { return m_index.size(); }

    void clear() noexcept
    {
        m_index.clear();
        m_current_used_slots = 0;
    }

private:
    std::vector<std::vector<Block>> m_segment_storage;
    std::vector<Block*> m_index;

    size_t m_current_payload_capacity;
    size_t m_current_used_slots;
};

}  // namespace tyr

#endif