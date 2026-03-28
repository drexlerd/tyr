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

#include "tyr/planning/lifted_task/state_storage/hash_set/atom.hpp"

#include "tyr/planning/lifted_task.hpp"
#include "tyr/planning/lifted_task/state_storage/hash_set/context.hpp"

namespace tyr::planning
{

AtomStorageBackend<LiftedTask, HashSet>::AtomStorageBackend(StateStorageContext<LiftedTask, HashSet>& ctx) : m_uint_vec_set(ctx.uint_vec_set), m_buffer() {}

typename AtomStorageBackend<LiftedTask, HashSet>::Packed
AtomStorageBackend<LiftedTask, HashSet>::insert(const typename AtomStorageBackend<LiftedTask, HashSet>::Unpacked& unpacked)
{
    m_buffer.clear();
    const auto& bits = unpacked.indices;
    for (auto i = bits.find_first(); i != boost::dynamic_bitset<>::npos; i = bits.find_next(i))
        m_buffer.push_back(i);

    return AtomStorageBackend<LiftedTask, HashSet>::Packed { m_uint_vec_set.insert(m_buffer) };
}

void AtomStorageBackend<LiftedTask, HashSet>::unpack(const typename AtomStorageBackend<LiftedTask, HashSet>::Packed& packed,
                                                     typename AtomStorageBackend<LiftedTask, HashSet>::Unpacked& unpacked)
{
    const auto view = m_uint_vec_set[packed.index];

    unpacked.indices.clear();
    for (uint_t i = 0; i < view.size(); ++i)
        tyr::set(view[i], true, unpacked.indices);
}

}
