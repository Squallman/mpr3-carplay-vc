#include "mpr3/runtime_config.hpp"
namespace mpr3 {
ConfigValidation RuntimeConfig::validate(std::optional<int> acquiredId) const noexcept {
  const bool hasName = !displayable.configuredName.empty();
  return {hasName, hasName && clusterDisplayId.has_value() &&
                    (acquiredId.has_value() || displayable.resolvedId.has_value())};
}
} // namespace mpr3
