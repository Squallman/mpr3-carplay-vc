#pragma once

#include "mpr3/displayable.hpp"

namespace mpr3 {

struct RuntimeConfig {
  DisplayableConfig displayable;
  int clusterDisplayId = -1; // UNKNOWN unless explicitly injected by a test.
  int displayableId = -1;
};

} // namespace mpr3
