#pragma once
#include "mpr3/displayable.hpp"
namespace mpr3 {
// OFFLINE prototype choices, not recovered P3695 semantics.
enum class MultiSecondaryPolicy { Reject, FirstOnly };
enum class OutputPolicy { AllowOfflinePipeline, RequireActivationInputs };
struct ConfigValidation {
  bool validForOffline = false;
  bool activationInputsKnown = false;
};
struct RuntimeConfig {
  DisplayableHandle displayable; // Empty name and unknown ID by default.
  std::optional<int> clusterDisplayId;
  MultiSecondaryPolicy multiSecondaryPolicy = MultiSecondaryPolicy::Reject;
  OutputPolicy outputPolicy = OutputPolicy::AllowOfflinePipeline;
  // Input sufficiency is not proof of occupancy, authorization, or safety.
  ConfigValidation validate(std::optional<int> acquiredId = std::nullopt) const noexcept;
};
} // namespace mpr3
