#pragma once
#include "mpr3/target/airplay_setup_abi.hpp"

namespace mpr3::target {
enum class ResolutionStatus { Resolved, MissingSymbol, SelfReference };
struct SetupResolution {
  ResolutionStatus status;
  SetupABI original;
};
SetupResolution validateOriginalSetup(SetupABI candidate, SetupABI wrapper) noexcept;

class IOriginalSetupResolver {
 public:
  virtual ~IOriginalSetupResolver() = default;
  virtual SetupResolution resolve(SetupABI wrapper) noexcept = 0;
};
// Typed lookup isolates POSIX conversion and makes resolution independently testable.
class ISetupSymbolLookup {
 public:
  virtual ~ISetupSymbolLookup() = default;
  virtual SetupABI lookupOriginalSetup() noexcept = 0;
};
class OriginalSetupResolver final : public IOriginalSetupResolver {
 public:
  explicit OriginalSetupResolver(ISetupSymbolLookup &lookup) noexcept : lookup_(lookup) {}
  SetupResolution resolve(SetupABI wrapper) noexcept override;
 private:
  ISetupSymbolLookup &lookup_; // Borrowed; must outlive this resolver.
};
} // namespace mpr3::target
