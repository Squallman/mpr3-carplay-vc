#pragma once

#include <string>

namespace mpr3 {

struct DisplayableConfig {
  std::string configuredName = "Displayable_Debug_1"; // TEST CANDIDATE ONLY
  int configuredId = -1; // injected metadata; -1 means unresolved/UNKNOWN
  bool testCandidateOnly = true;
};

class IDisplayableProvider {
 public:
  virtual ~IDisplayableProvider() = default;
  virtual bool createByConfiguredName(const DisplayableConfig &) = 0;
  virtual void destroy() = 0;
};

} // namespace mpr3
