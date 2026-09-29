#include "mpr3/target/original_setup_resolver.hpp"

namespace mpr3::target {
SetupResolution validateOriginalSetup(SetupABI candidate, SetupABI wrapper) noexcept {
  if (!candidate) return {ResolutionStatus::MissingSymbol, nullptr};
  if (candidate == wrapper) return {ResolutionStatus::SelfReference, nullptr};
  return {ResolutionStatus::Resolved, candidate};
}
SetupResolution OriginalSetupResolver::resolve(SetupABI wrapper) noexcept {
  // Exactly one lookup per invocation; no cache or mutable process-wide state.
  return validateOriginalSetup(lookup_.lookupOriginalSetup(), wrapper);
}
} // namespace mpr3::target
