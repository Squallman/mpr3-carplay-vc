#pragma once
namespace mpr3 {
enum class CapabilitySupport { Unknown, Supported, Unsupported };
// Semantic extension points only; no descriptor fields, delegate slot, wire
// parameter, or causal relationship to iOS type 111 is specified here.
class ISecondaryDisplayAdvertisement {
 public:
  virtual ~ISecondaryDisplayAdvertisement() = default;
  virtual CapabilitySupport support() const noexcept = 0;
};
class IIap2SecondaryCapability {
 public:
  virtual ~IIap2SecondaryCapability() = default;
  virtual CapabilitySupport support() const noexcept = 0;
};
} // namespace mpr3
