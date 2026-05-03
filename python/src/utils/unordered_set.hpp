/*
    nanobind/stl/optional.h: type caster for std::optional<...>

    Copyright (c) 2022 Yoshiki Matsuda and Wenzel Jakob

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE file.
*/

#pragma once

#include <gtl/phmap.hpp>
#include <nanobind/nanobind.h>
#include <nanobind/stl/detail/nb_set.h>
#include <type_traits>
#include <tyr/common/declarations.hpp>
#include <tyr/common/equal_to.hpp>
#include <tyr/common/hash.hpp>

NAMESPACE_BEGIN(NB_NAMESPACE)
NAMESPACE_BEGIN(detail)

template<typename Key, typename Hash, typename Compare, typename Alloc>
struct type_caster<gtl::flat_hash_set<Key, Hash, Compare, Alloc>> : set_caster<gtl::flat_hash_set<Key, Hash, Compare, Alloc>, Key>
{
};

NAMESPACE_END(detail)
NAMESPACE_END(NB_NAMESPACE)
