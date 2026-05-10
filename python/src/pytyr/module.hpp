#ifndef PYTYR_MODULE_HPP_
#define PYTYR_MODULE_HPP_

#include <nanobind/nanobind.h>

namespace nb = nanobind;

namespace tyr
{

void bind_module_definitions(nb::module_& m);

}  // namespace tyr

#endif
