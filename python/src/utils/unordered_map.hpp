/*
    nanobind/stl/unordered_map.h: type caster for std::unordered_map<...>

    Copyright (c) 2022 Matej Ferencevic and Wenzel Jakob

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE file.
*/

#pragma once

#include <gtl/phmap.hpp>
#include <nanobind/nanobind.h>
#include <nanobind/stl/detail/nb_dict.h>
#include <type_traits>
#include <tyr/common/declarations.hpp>
#include <tyr/common/equal_to.hpp>
#include <tyr/common/hash.hpp>

NAMESPACE_BEGIN(NB_NAMESPACE)
NAMESPACE_BEGIN(detail)

template<typename Key, typename T, typename Hash, typename Compare, typename Alloc>
struct type_caster<gtl::flat_hash_map<Key, T, Hash, Compare, Alloc>> : dict_caster<gtl::flat_hash_map<Key, T, Hash, Compare, Alloc>, Key, T>
{
};

NAMESPACE_END(detail)
NAMESPACE_END(NB_NAMESPACE)
