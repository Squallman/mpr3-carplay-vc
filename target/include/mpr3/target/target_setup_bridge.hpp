#pragma once
#include "mpr3/target/original_setup_resolver.hpp"

namespace mpr3::target {
enum class SetupBridgeState { ResolvedAndCalled, ResolverUnavailable };
class SetupBridgeResult {
 public:
  static SetupBridgeResult called(int status) noexcept {
    return {SetupBridgeState::ResolvedAndCalled, ResolutionStatus::Resolved, status};
  }
  static SetupBridgeResult unavailable(ResolutionStatus reason) noexcept {
    return {SetupBridgeState::ResolverUnavailable, reason, 0};
  }
  SetupBridgeState state() const noexcept { return state_; }
  ResolutionStatus resolution() const noexcept { return resolution_; }
  // Null means no stock status exists. Never translate failure into AirPlay status.
  const int *originalStatus() const noexcept {
    return state_ == SetupBridgeState::ResolvedAndCalled ? &status_ : nullptr;
  }
 private:
  SetupBridgeResult(SetupBridgeState state, ResolutionStatus resolution, int status) noexcept
      : state_(state), resolution_(resolution), status_(status) {}
  SetupBridgeState state_;
  ResolutionStatus resolution_;
  int status_;
};

SetupBridgeResult passThroughSetup(IOriginalSetupResolver &resolver, SetupABI wrapper,
    AirPlayReceiverSessionPrivate *session, CFDictionaryRef request,
    CFDictionaryRef *responseOut);
} // namespace mpr3::target
