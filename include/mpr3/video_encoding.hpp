#pragma once
namespace mpr3 {
enum class ActivationResult {
  NotAttempted, Activated, Unavailable, InvalidDisplayable, Failed,
  MissingEndpoint, MissingDisplayableId
};
class IVideoEncodingClient {
 public:
  virtual ~IVideoEncodingClient() = default;
  // Evidence-backed semantic shape, not the generated target ABI.
  // No service variant or MOST/Ethernet selection.
  virtual ActivationResult setActiveDisplayable(int displayId, int displayableId) = 0;
};
} // namespace mpr3
