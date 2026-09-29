#include "mpr3/target/target_setup_bridge.hpp"

namespace mpr3::target {
SetupBridgeResult passThroughSetup(IOriginalSetupResolver &resolver, SetupABI wrapper,
    AirPlayReceiverSessionPrivate *session, CFDictionaryRef request,
    CFDictionaryRef *responseOut) {
  auto resolution = resolver.resolve(wrapper);
  if (resolution.status == ResolutionStatus::Resolved) {
    // Defend even against a resolver implementation returning a malformed success.
    resolution = validateOriginalSetup(resolution.original, wrapper);
  }
  if (resolution.status != ResolutionStatus::Resolved)
    return SetupBridgeResult::unavailable(resolution.status);
  return SetupBridgeResult::called(resolution.original(session, request, responseOut));
}
} // namespace mpr3::target
