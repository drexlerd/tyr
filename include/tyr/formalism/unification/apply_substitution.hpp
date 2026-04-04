#ifndef TYR_FORMALISM_UNIFICATION_APPLY_SUBSTITUTION_HPP_
#define TYR_FORMALISM_UNIFICATION_APPLY_SUBSTITUTION_HPP_

#include "tyr/formalism/unification/structure_traits.hpp"
#include "tyr/formalism/unification/structure_traits_impl.hpp"
#include "tyr/formalism/unification/substitution.hpp"
#include "tyr/formalism/unification/term_operations.hpp"

#include <algorithm>
#include <cassert>
#include <vector>

namespace tyr::formalism::unification
{

template<TermSubstitution S>
[[nodiscard]] Data<Term> apply_substitution_once(const Data<Term>& term, const S& rho)
{
    if (!is_parameter(term))
        return term;

    const auto p = get_parameter(term);
    if (!rho.is_bound(p))
        return term;

    return Data<Term>(*rho[p]);
}

template<TermUnifiableStructure T, TermSubstitution S>
[[nodiscard]] T apply_substitution_once(const T& value, const S& rho)
{
    return structure_traits<T>::transform_terms(value, [&](const Data<Term>& term) { return apply_substitution_once(term, rho); });
}

template<TermSubstitution S>
[[nodiscard]] Data<Term> apply_substitution(const Data<Term>& term, const S& rho)
{
    return apply_substitution_once(term, rho);
}

template<TermUnifiableStructure T, TermSubstitution S>
[[nodiscard]] T apply_substitution(const T& value, const S& rho)
{
    return apply_substitution_once(value, rho);
}

template<TermSubstitution S>
[[nodiscard]] Data<Term> apply_substitution_fixpoint(const Data<Term>& term, const S& rho)
{
    auto current = term;
    auto seen = std::vector<ParameterIndex> {};

    while (is_parameter(current))
    {
        const auto p = get_parameter(current);

        // Stop on cycles and return the repeated parameter unchanged.
        if (std::find(seen.begin(), seen.end(), p) != seen.end())
            return current;

        seen.push_back(p);

        if (!rho.is_bound(p))
            return current;

        current = Data<Term>(*rho[p]);
    }

    return current;
}

template<TermUnifiableStructure T, TermSubstitution S>
[[nodiscard]] T apply_substitution_fixpoint(const T& value, const S& rho)
{
    return structure_traits<T>::transform_terms(value, [&](const Data<Term>& term) { return apply_substitution_fixpoint(term, rho); });
}

template<TermSubstitution S1, TermSubstitution S2>
[[nodiscard]] SubstitutionFunction<Data<Term>> compose_substitutions(const S1& outer, const S2& inner)
{
    auto parameters = std::vector<ParameterIndex> {};
    parameters.reserve(inner.parameters().size() + outer.parameters().size());

    const auto append_unique = [&](const auto& source)
    {
        for (const auto parameter : source.parameters())
        {
            if (std::find(parameters.begin(), parameters.end(), parameter) == parameters.end())
                parameters.push_back(parameter);
        }
    };

    append_unique(inner);
    append_unique(outer);

    auto result = SubstitutionFunction<Data<Term>>(std::move(parameters));

    // The resulting substitution applies `inner` first and `outer` second.
    for (const auto parameter : result.parameters())
    {
        auto value = Data<Term>(parameter);
        value = apply_substitution_fixpoint(apply_substitution_fixpoint(value, inner), outer);

        if (value != Data<Term>(parameter))
        {
            [[maybe_unused]] const auto inserted = result.assign(parameter, value);
            assert(inserted);
        }
    }

    return result;
}

}  // namespace tyr::formalism::unification

#endif