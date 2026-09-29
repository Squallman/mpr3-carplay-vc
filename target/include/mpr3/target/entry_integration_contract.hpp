#pragma once
#include "mpr3/target/target_setup_bridge.hpp"

namespace mpr3::target {
// Intentionally undefined: future runtime integration must supply these.
// In particular there is no evidence-backed AirPlay failure status yet.
IOriginalSetupResolver &entryOriginalSetupResolver() noexcept;
[[noreturn]] void entryResolverUnavailable(ResolutionStatus reason) noexcept;
} // namespace mpr3::target
