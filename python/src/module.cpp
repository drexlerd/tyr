#include "pytyr/module.hpp"

#include <nanobind/nanobind.h>

namespace tyr
{

NB_MODULE(_pytyr, m) { bind_module_definitions(m); }

}  // namespace tyr
