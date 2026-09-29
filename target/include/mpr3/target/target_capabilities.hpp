#pragma once

namespace mpr3::target {
// Evidence classifications and implementation availability are separate axes.
enum class TargetContractStatus { Proven, StrongEvidence, Plausible, Unknown, Disproven };
enum class AdapterAvailability { ContractOnly, OptInPosix, ImplementedOffline, NotImplemented };
struct TargetCapability {
  const char *name;
  TargetContractStatus evidence;
  TargetContractStatus runtime;
  AdapterAvailability availability;
};
inline constexpr TargetCapability targetCapabilities[] = {
  {"AirPlay setup ABI", TargetContractStatus::StrongEvidence, TargetContractStatus::Unknown,
      AdapterAvailability::ContractOnly},
  {"RTLD_NEXT resolver", TargetContractStatus::Plausible, TargetContractStatus::Unknown,
      AdapterAvailability::OptInPosix},
  {"CoreFoundation adapter", TargetContractStatus::StrongEvidence, TargetContractStatus::Unknown,
      AdapterAvailability::ImplementedOffline},
  {"ScreenStream adapter", TargetContractStatus::Unknown, TargetContractStatus::Unknown,
      AdapterAvailability::NotImplemented},
  {"display-init adapter", TargetContractStatus::Unknown, TargetContractStatus::Unknown,
      AdapterAvailability::NotImplemented},
  {"VideoEncoding COMM adapter", TargetContractStatus::Unknown, TargetContractStatus::Unknown,
      AdapterAvailability::NotImplemented},
  {"advertisement adapter", TargetContractStatus::Unknown, TargetContractStatus::Unknown,
      AdapterAvailability::NotImplemented},
  {"iAP2 capability adapter", TargetContractStatus::Unknown, TargetContractStatus::Unknown,
      AdapterAvailability::NotImplemented},
};
// Required evidence gates, not output activation or deactivation implementations.
struct OutputReadinessEvidence {
  bool adapterAbiProven = false;
  bool endpointProven = false;
  bool displayableOwnershipProven = false;
  bool authorizationProven = false;
  bool lifecycleProven = false;
  bool previousSelectionKnown = false;
  bool restorationProven = false;
  bool nativeNavigationRacePolicyProven = false;
  bool restoreBeforeDestroyProven = false;
};
constexpr bool canClaimTargetReadyOutput(const OutputReadinessEvidence &e) noexcept {
  return e.adapterAbiProven && e.endpointProven && e.displayableOwnershipProven &&
      e.authorizationProven && e.lifecycleProven && e.previousSelectionKnown &&
      e.restorationProven && e.nativeNavigationRacePolicyProven && e.restoreBeforeDestroyProven;
}
inline constexpr OutputReadinessEvidence currentOutputEvidence{};
static_assert(!canClaimTargetReadyOutput(currentOutputEvidence), "Target output remains blocked");
} // namespace mpr3::target
