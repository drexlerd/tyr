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

#include "tyr/planning/state_storage/hash_set/numeric.hpp"

#include "tyr/planning/ground_task.hpp"
#include "tyr/planning/ground_task/state_storage/hash_set/context.hpp"
#include "tyr/planning/lifted_task.hpp"
#include "tyr/planning/lifted_task/state_storage/hash_set/context.hpp"

namespace tyr::planning
{
template<typename Task>
NumericStorageBackend<Task, HashSet>::NumericStorageBackend(StateStorageContext<Task, HashSet>& ctx) : m_float_vec_set(ctx.float_vec_set)
{
}

template<typename Task>
typename NumericStorageBackend<Task, HashSet>::Packed
NumericStorageBackend<Task, HashSet>::insert(const typename NumericStorageBackend<Task, HashSet>::Unpacked& unpacked)
{
    return NumericStorageBackend<Task, HashSet>::Packed { m_float_vec_set.insert(unpacked.values) };
}

template<typename Task>
void NumericStorageBackend<Task, HashSet>::unpack(const typename NumericStorageBackend<Task, HashSet>::Packed& packed,
                                                  typename NumericStorageBackend<Task, HashSet>::Unpacked& unpacked)
{
    const auto view = m_float_vec_set[packed.index];

    unpacked.values.resize(view.size());
    for (uint_t i = 0; i < view.size(); ++i)
        unpacked.values[i] = view[i];
}

template class NumericStorageBackend<LiftedTask, HashSet>;
template class NumericStorageBackend<GroundTask, HashSet>;

}
