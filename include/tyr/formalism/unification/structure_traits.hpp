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

#ifndef TYR_FORMALISM_UNIFICATION_STRUCTURE_TRAITS_HPP_
#define TYR_FORMALISM_UNIFICATION_STRUCTURE_TRAITS_HPP_

#include "tyr/formalism/term_data.hpp"

#include <concepts>
#include <utility>

namespace tyr::formalism::unification
{

template<typename T>
struct structure_traits;

template<>
struct structure_traits<Data<Term>>
{
    template<typename F>
    static bool zip_terms(const Data<Term>& lhs, const Data<Term>& rhs, F&& f)
    {
        return f(lhs, rhs);
    }

    template<typename F>
    static Data<Term> transform_terms(const Data<Term>& value, F&& f)
    {
        return f(value);
    }
};

template<typename T>
concept TermUnifiableStructure = requires(const T& lhs, const T& rhs) {
    {
        structure_traits<T>::zip_terms(lhs, rhs, [](const Data<Term>&, const Data<Term>&) { return true; })
    } -> std::same_as<bool>;
    {
        structure_traits<T>::transform_terms(lhs, [](const Data<Term>& term) { return term; })
    } -> std::same_as<T>;
};

}  // namespace tyr::formalism::unification

#endif