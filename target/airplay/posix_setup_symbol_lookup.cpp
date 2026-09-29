#include "mpr3/target/posix_setup_symbol_lookup.hpp"
#include <dlfcn.h>

namespace mpr3::target {
SetupABI PosixSetupSymbolLookup::lookupOriginalSetup() noexcept {
  // POSIX conversion only; no firmware address/path or dlopen fallback.
  return reinterpret_cast<SetupABI>(dlsym(RTLD_NEXT, setupSymbolName));
}
} // namespace mpr3::target
