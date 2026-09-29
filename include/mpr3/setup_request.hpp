#pragma once
#include <cstdint>
#include <memory>
#include <optional>
#include <vector>

namespace mpr3 {
class ISetupDescriptor {
 public:
  virtual ~ISetupDescriptor() = default;
  virtual std::optional<std::int64_t> type() const = 0;
  // Non-owning identity token, valid while the descriptor is retained.
  virtual const void *identity() const noexcept = 0;
};
using DescriptorRef = std::shared_ptr<const ISetupDescriptor>;
using DescriptorCollection = std::vector<DescriptorRef>;
class ISetupRequest {
 public:
  virtual ~ISetupRequest() = default;
  // Missing/malformed streams return nullopt; null entries/unknown types are
  // invalid. Unknown fields remain opaque in retained descriptors.
  virtual std::optional<DescriptorCollection> streams() const = 0;
  // Preserve opaque request fields and reuse descriptor identities.
  // Return nullptr if a safe replacement request cannot be constructed.
  virtual std::unique_ptr<ISetupRequest> withStreams(DescriptorCollection) const = 0;
};
} // namespace mpr3
