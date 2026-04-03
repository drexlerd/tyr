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

#ifndef TYR_COMMON_SHARED_OBJECT_POOL_HPP_
#define TYR_COMMON_SHARED_OBJECT_POOL_HPP_

#include <atomic>
#include <cassert>
#include <memory>
#include <mutex>
#include <utility>
#include <vector>

namespace tyr
{

/**
 * A thread-safe shared object pool
 */

template<typename T, bool ThreadSafe = false>
class SharedObjectPool;

template<typename T, bool ThreadSafe = false>
struct SharedObjectPoolEntry;

template<typename T>
struct SharedObjectPoolEntry<T, true>
{
    std::atomic_size_t refcount;
    T object;

    SharedObjectPoolEntry() : refcount(0), object() {}

    template<typename... Args>
    explicit SharedObjectPoolEntry(Args&&... args) : refcount(0), object(std::forward<Args>(args)...)
    {
    }

    template<typename... Args>
    void initialize(Args&&... args)
    {
        object.initialize(std::forward<Args>(args)...);
    }
};

template<typename T>
struct SharedObjectPoolEntry<T, false>
{
    size_t refcount;
    T object;

    SharedObjectPoolEntry() : refcount(0), object() {}

    template<typename... Args>
    explicit SharedObjectPoolEntry(Args&&... args) : refcount(0), object(std::forward<Args>(args)...)
    {
    }

    template<typename... Args>
    void initialize(Args&&... args)
    {
        object.initialize(std::forward<Args>(args)...);
    }
};

template<typename T, bool ThreadSafe = false>
class SharedObjectPoolPtr;

template<typename T>
class SharedObjectPoolPtr<T, true>
{
private:
    using Entry = SharedObjectPoolEntry<T, true>;

    SharedObjectPool<T, true>* m_pool;
    Entry* m_entry;

private:
    void deallocate()
    {
        assert(m_pool && m_entry);

        m_pool->free(m_entry);
        m_pool = nullptr;
        m_entry = nullptr;
    }

    Entry* release() noexcept
    {
        Entry* temp = m_entry;
        m_entry = nullptr;
        m_pool = nullptr;
        return temp;
    }

    void inc_ref_count() noexcept
    {
        assert(m_entry);

        m_entry->refcount.fetch_add(1, std::memory_order_relaxed);
    }

    void dec_ref_count()
    {
        assert(m_entry);

        auto old = m_entry->refcount.fetch_sub(1, std::memory_order_acq_rel);

        assert(old > 0);
        if (old == 1)
        {
            deallocate();  // returns to pool with mutex
        }
    }

public:
    SharedObjectPoolPtr() noexcept : SharedObjectPoolPtr(nullptr, nullptr) {}

    SharedObjectPoolPtr(SharedObjectPool<T, true>* pool, Entry* object) noexcept : m_pool(pool), m_entry(object)
    {
        if (m_pool && m_entry)
            inc_ref_count();
    }

    SharedObjectPoolPtr(const SharedObjectPoolPtr& other) noexcept : SharedObjectPoolPtr()
    {
        m_pool = other.m_pool;
        m_entry = other.m_entry;

        if (m_pool && m_entry)
            inc_ref_count();
    }

    SharedObjectPoolPtr& operator=(const SharedObjectPoolPtr& other)
    {
        if (this != &other)
        {
            if (m_pool && m_entry)
                dec_ref_count();

            m_pool = other.m_pool;
            m_entry = other.m_entry;

            if (m_pool && m_entry)
                inc_ref_count();
        }
        return *this;
    }

    // Movable
    SharedObjectPoolPtr(SharedObjectPoolPtr&& other) noexcept : m_pool(other.m_pool), m_entry(other.m_entry)
    {
        other.m_pool = nullptr;
        other.m_entry = nullptr;
    }

    SharedObjectPoolPtr& operator=(SharedObjectPoolPtr&& other) noexcept
    {
        if (this != &other)
        {
            if (m_pool && m_entry)
                dec_ref_count();

            m_pool = other.m_pool;
            m_entry = other.m_entry;

            other.m_pool = nullptr;
            other.m_entry = nullptr;
        }
        return *this;
    }

    ~SharedObjectPoolPtr()
    {
        if (m_pool && m_entry)
            dec_ref_count();
    }

    SharedObjectPoolPtr clone() const
        requires std::is_copy_assignable_v<T>
    {
        if (m_pool && m_entry)
        {
            SharedObjectPoolPtr pointer = m_pool->get_or_allocate();
            *pointer = this->operator*();  // copy-assign T
            return pointer;
        }
        else
        {
            return SharedObjectPoolPtr();
        }
    }

    T& operator*() const noexcept
    {
        assert(m_entry);
        return m_entry->object;
    }

    T* operator->() const noexcept
    {
        assert(m_entry);
        return &m_entry->object;
    }

    size_t ref_count() const noexcept
    {
        assert(m_entry);
        return m_entry->refcount.load(std::memory_order_acquire);
    }

    explicit operator bool() const noexcept { return m_entry != nullptr; }
};

template<typename T>
class SharedObjectPoolPtr<T, false>
{
private:
    using Entry = SharedObjectPoolEntry<T, false>;

    SharedObjectPool<T, false>* m_pool;
    Entry* m_entry;

private:
    void deallocate()
    {
        assert(m_pool && m_entry);

        m_pool->free(m_entry);
        m_pool = nullptr;
        m_entry = nullptr;
    }

    Entry* release() noexcept
    {
        Entry* temp = m_entry;
        m_entry = nullptr;
        m_pool = nullptr;
        return temp;
    }

    void inc_ref_count() noexcept
    {
        assert(m_entry);

        ++m_entry->refcount;
    }

    void dec_ref_count()
    {
        assert(m_entry);
        assert(m_entry->refcount > 0);

        if (--m_entry->refcount == 0)
        {
            deallocate();  // returns to pool with mutex
        }
    }

public:
    SharedObjectPoolPtr() noexcept : SharedObjectPoolPtr(nullptr, nullptr) {}

    SharedObjectPoolPtr(SharedObjectPool<T, false>* pool, Entry* object) noexcept : m_pool(pool), m_entry(object)
    {
        if (m_pool && m_entry)
            inc_ref_count();
    }

    SharedObjectPoolPtr(const SharedObjectPoolPtr& other) noexcept : SharedObjectPoolPtr()
    {
        m_pool = other.m_pool;
        m_entry = other.m_entry;

        if (m_pool && m_entry)
            inc_ref_count();
    }

    SharedObjectPoolPtr& operator=(const SharedObjectPoolPtr& other)
    {
        if (this != &other)
        {
            if (m_pool && m_entry)
                dec_ref_count();

            m_pool = other.m_pool;
            m_entry = other.m_entry;

            if (m_pool && m_entry)
                inc_ref_count();
        }
        return *this;
    }

    // Movable
    SharedObjectPoolPtr(SharedObjectPoolPtr&& other) noexcept : m_pool(other.m_pool), m_entry(other.m_entry)
    {
        other.m_pool = nullptr;
        other.m_entry = nullptr;
    }

    SharedObjectPoolPtr& operator=(SharedObjectPoolPtr&& other) noexcept
    {
        if (this != &other)
        {
            if (m_pool && m_entry)
                dec_ref_count();

            m_pool = other.m_pool;
            m_entry = other.m_entry;

            other.m_pool = nullptr;
            other.m_entry = nullptr;
        }
        return *this;
    }

    ~SharedObjectPoolPtr()
    {
        if (m_pool && m_entry)
            dec_ref_count();
    }

    SharedObjectPoolPtr clone() const
        requires std::is_copy_assignable_v<T>
    {
        if (m_pool && m_entry)
        {
            SharedObjectPoolPtr pointer = m_pool->get_or_allocate();
            *pointer = this->operator*();  // copy-assign T
            return pointer;
        }
        else
        {
            return SharedObjectPoolPtr();
        }
    }

    T& operator*() const noexcept
    {
        assert(m_entry);
        return m_entry->object;
    }

    T* operator->() const noexcept
    {
        assert(m_entry);
        return &m_entry->object;
    }

    size_t ref_count() const noexcept
    {
        assert(m_entry);
        return m_entry->refcount;
    }

    explicit operator bool() const noexcept { return m_entry != nullptr; }
};

template<typename T>
class SharedObjectPool<T, true>
{
private:
    using Entry = SharedObjectPoolEntry<T, true>;

    std::vector<std::unique_ptr<Entry>> m_storage;
    std::vector<Entry*> m_stack;
    mutable std::mutex m_mutex;

    void allocate()
    {
        m_storage.push_back(std::make_unique<Entry>());
        m_stack.push_back(m_storage.back().get());
    }

    void free(Entry* element)
    {
        std::lock_guard<std::mutex> lg(m_mutex);

        m_stack.push_back(element);
    }

    friend class SharedObjectPoolPtr<T, true>;

public:
    // Non-copyable to prevent dangling memory pool pointers.
    SharedObjectPool() noexcept = default;
    SharedObjectPool(const SharedObjectPool& other) = delete;
    SharedObjectPool& operator=(const SharedObjectPool& other) = delete;
    SharedObjectPool(SharedObjectPool&& other) noexcept = default;
    SharedObjectPool& operator=(SharedObjectPool&& other) noexcept = default;

    [[nodiscard]] SharedObjectPoolPtr<T, true> get_or_allocate()
    {
        std::lock_guard<std::mutex> lg(m_mutex);

        if (m_stack.empty())
            allocate();
        Entry* element = m_stack.back();
        m_stack.pop_back();
        return SharedObjectPoolPtr<T, true>(this, element);
    }

    template<typename... Args>
    [[nodiscard]] SharedObjectPoolPtr<T, true> get_or_allocate(Args&&... args)
    {
        std::lock_guard<std::mutex> lg(m_mutex);

        if (m_stack.empty())
            allocate();
        Entry* element = m_stack.back();
        m_stack.pop_back();
        element->initialize(std::forward<Args>(args)...);
        return SharedObjectPoolPtr<T, true>(this, element);
    }

    [[nodiscard]] size_t get_size() const noexcept
    {
        std::lock_guard<std::mutex> lg(m_mutex);

        return m_storage.size();
    }

    [[nodiscard]] size_t get_num_free() const noexcept
    {
        std::lock_guard<std::mutex> lg(m_mutex);

        return m_stack.size();
    }
};

template<typename T>
class SharedObjectPool<T, false>
{
private:
    using Entry = SharedObjectPoolEntry<T, false>;

    std::vector<std::unique_ptr<Entry>> m_storage;
    std::vector<Entry*> m_stack;

    void allocate()
    {
        m_storage.push_back(std::make_unique<Entry>());
        m_stack.push_back(m_storage.back().get());
    }

    void free(Entry* element) { m_stack.push_back(element); }

    friend class SharedObjectPoolPtr<T, false>;

public:
    // Non-copyable to prevent dangling memory pool pointers.
    SharedObjectPool() noexcept = default;
    SharedObjectPool(const SharedObjectPool& other) = delete;
    SharedObjectPool& operator=(const SharedObjectPool& other) = delete;
    SharedObjectPool(SharedObjectPool&& other) noexcept = default;
    SharedObjectPool& operator=(SharedObjectPool&& other) noexcept = default;

    [[nodiscard]] SharedObjectPoolPtr<T, false> get_or_allocate()
    {
        if (m_stack.empty())
            allocate();
        Entry* element = m_stack.back();
        m_stack.pop_back();
        return SharedObjectPoolPtr<T, false>(this, element);
    }

    template<typename... Args>
    [[nodiscard]] SharedObjectPoolPtr<T, false> get_or_allocate(Args&&... args)
    {
        if (m_stack.empty())
            allocate();
        Entry* element = m_stack.back();
        m_stack.pop_back();
        element->initialize(std::forward<Args>(args)...);
        return SharedObjectPoolPtr<T, false>(this, element);
    }

    [[nodiscard]] size_t get_size() const noexcept { return m_storage.size(); }

    [[nodiscard]] size_t get_num_free() const noexcept { return m_stack.size(); }
};

}

#endif