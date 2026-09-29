#include "mpr3/mocks.hpp"
#include <stdexcept>

namespace mpr3 {
ActivationResult MockVideoEncoding::setActiveDisplayable(int display, int displayable) {
  ++calls;
  lastDisplay = display;
  lastDisplayable = displayable;
  if (throwOnActivate) throw std::runtime_error("mock activation failure");
  return result;
}
} // namespace mpr3
