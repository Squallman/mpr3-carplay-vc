#pragma once
#include "mpr3/target/original_setup_resolver.hpp"

#if !defined(MPR3_ENABLE_POSIX_TARGET_RESOLVER) || !MPR3_ENABLE_POSIX_TARGET_RESOLVER
#error "POSIX lookup requires explicit MPR3_ENABLE_POSIX_TARGET_RESOLVER=1"
#endif
namespace mpr3::target {
// PLAUSIBLE; target loader scope, versioning and policy remain UNKNOWN.
class PosixSetupSymbolLookup final : public ISetupSymbolLookup {
 public:
  SetupABI lookupOriginalSetup() noexcept override;
};
} // namespace mpr3::target
