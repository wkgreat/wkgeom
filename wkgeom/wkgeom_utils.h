#pragma once

#include <cmath>

namespace wk::wkgeom::utils {

inline bool dbl_equal(const double d1, const double d2) {
  return std::fabs(d1 - d2) < __DBL_EPSILON__;
}

}  // namespace wk::wkgeom::utils