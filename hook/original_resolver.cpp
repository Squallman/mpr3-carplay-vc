#include "mpr3/setup_filter.hpp"

#if MPR3_ENABLE_TARGET_LOADER
#include <dlfcn.h>
#endif

namespace mpr3 {

TargetSetupABI resolveOriginalSetup() {
#if MPR3_ENABLE_TARGET_LOADER
  // Optional host-compatible model of the Phase 6 RTLD_NEXT seam. Never used
  // by default tests and never executed against target firmware here.
  return reinterpret_cast<TargetSetupABI>(dlsym(RTLD_NEXT, "AirPlayReceiverSessionSetup"));
#else
  return nullptr;
#endif
}

} // namespace mpr3
