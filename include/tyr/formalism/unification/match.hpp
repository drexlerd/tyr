#ifndef TYR_FORMALISM_UNIFICATION_MATCH_HPP_
#define TYR_FORMALISM_UNIFICATION_MATCH_HPP_

#include "tyr/formalism/unification/match_term.hpp"
#include "tyr/formalism/unification/structure_traits.hpp"

namespace tyr::formalism::unification
{

template<typename T, typename Policy = DefaultMatchPolicy>
[[nodiscard]] std::optional<TermMatchState> match(const T& pattern, const T& element, TermMatchState state, const Policy& policy = {})
{
    const bool ok =
        structure_traits<T>::zip_terms(pattern, element, [&](const Data<Term>& lhs, const Data<Term>& rhs) { return match_term(lhs, rhs, state, policy); });

    if (!ok)
        return std::nullopt;

    return state;
}

template<typename T>
[[nodiscard]] std::optional<SubstitutionFunction<Data<Term>>> match(const T& pattern, const T& element, SubstitutionFunction<Data<Term>> sigma)
{
    TermMatchState state { std::move(sigma), SubstitutionFunction<Data<Term>>() };

    auto result = match(pattern, element, std::move(state), DefaultMatchPolicy {});
    if (!result.has_value())
        return std::nullopt;

    return std::move(result->sigma);
}

}

#endif