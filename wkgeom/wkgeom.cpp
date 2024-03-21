#include "wkgeom.h"

namespace wk::wkgeom {

template <typename U, int D>
std::ostream& operator<<(std::ostream& os, const LineString<U, D>& line) {
  for (const Point<U, D>& p : line.points) {
    os << p << ",";
  }
  return os;
}

}  // namespace wk::wkgeom