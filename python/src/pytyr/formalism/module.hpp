#ifndef TYR_PYTHON_FORMALISM_MODULE_HPP_
#define TYR_PYTHON_FORMALISM_MODULE_HPP_

#include <nanobind/nanobind.h>

namespace nb = nanobind;
using namespace nb::literals;

namespace tyr::formalism
{

void bind_module_definitions(nb::module_& m);

}  // namespace tyr::formalism

#endif
