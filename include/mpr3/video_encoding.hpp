#pragma once

namespace mpr3 {

enum class Result { Ok, Unavailable, InvalidDisplayable, Failed };

class IVideoEncodingClient {
 public:
  virtual ~IVideoEncodingClient() = default;
  // PROVEN method shape; endpoint numeric value is intentionally caller supplied.
  virtual Result setActiveDisplayable(int displayId, int displayableId) = 0;
};

} // namespace mpr3
