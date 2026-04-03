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

#ifndef TYR_FORMALISM_FDR_CONTEXT_HPP_
#define TYR_FORMALISM_FDR_CONTEXT_HPP_

#include "tyr/buffer/declarations.hpp"
#include "tyr/common/equal_to.hpp"
#include "tyr/common/hash.hpp"
#include "tyr/common/types.hpp"
#include "tyr/formalism/planning/declarations.hpp"
#include "tyr/formalism/planning/repository.hpp"

#include <concepts>
#include <vector>

namespace tyr::formalism::planning
{

class FDRContext
{
public:
    // Construct without mutexes.
    explicit FDRContext(RepositoryPtr context);

    // Construct with ground mutexes.
    FDRContext(const std::vector<GroundAtomViewList<FluentTag>>& mutexes, RepositoryPtr context);

    // Construct with binary ground mutexes.
    FDRContext(const GroundAtomViewList<FluentTag>& all_atoms, RepositoryPtr context);

    // Copy the FDRContext.
    FDRContext(const FDRContext& other, Builder& builder, RepositoryPtr context);

    Data<FDRFact<FluentTag>> get_fact(Index<GroundAtom<FluentTag>> atom);
    Data<FDRFact<FluentTag>> get_fact(GroundAtomView<FluentTag> atom);
    FDRFactView<FluentTag> get_fact_view(GroundAtomView<FluentTag> atom);

    Data<FDRFact<FluentTag>> get_fact(Index<GroundLiteral<FluentTag>> literal);
    Data<FDRFact<FluentTag>> get_fact(GroundLiteralView<FluentTag> literal);
    FDRFactView<FluentTag> get_fact_view(GroundLiteralView<FluentTag> literal);

    FDRVariableListView<FluentTag> get_variables() const;

private:
    RepositoryPtr m_context;
    Data<FDRVariable<FluentTag>> m_builder;
    IndexList<FDRVariable<FluentTag>> m_variables;
    UnorderedMap<Index<GroundAtom<FluentTag>>, Data<FDRFact<FluentTag>>> m_mapping;
};

}

#endif
