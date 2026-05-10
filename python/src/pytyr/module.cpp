#include "module.hpp"

#include "common/module.hpp"
#include "formalism/module.hpp"
#include "planning/module.hpp"

namespace tyr
{

void bind_module_definitions(nb::module_& m)
{
    auto common_module = m.def_submodule("common");
    common::bind_module_definitions(common_module);

    auto formalism_module = m.def_submodule("formalism");
    formalism::bind_module_definitions(formalism_module);

    auto planning_module = m.def_submodule("planning");
    planning::bind_module_definitions(planning_module);
}

}  // namespace tyr
