#include <nanobind/nanobind.h>

#include <tyr/common/config.hpp>

namespace nb = nanobind;

NB_MODULE(_downstream_tyr, m)
{
    m.def("float_t_size", [] { return sizeof(tyr::float_t); });
    m.def("multiply", [](int lhs, int rhs) { return lhs * rhs; });
}
