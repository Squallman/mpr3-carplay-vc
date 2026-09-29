#pragma once

#include "mpr3/secondary_session.hpp"
#include "mpr3/video_encoding.hpp"
#include <vector>

namespace mpr3 {

struct MockScreenStream final : IScreenStream {
  bool startResult = true; int starts = 0; int stops = 0;
  bool start() override { ++starts; return startResult; }
  void stop() override { ++stops; }
};
struct MockDecoder final : IDecoder {
  bool startResult = true; int starts = 0; int stops = 0;
  bool start() override { ++starts; return startResult; }
  void stop() override { ++stops; }
};
struct MockDisplayable final : IDisplayable {
  bool validResult = true; bool valid() const override { return validResult; }
};
struct MockVideoEncoding final : IVideoEncodingClient {
  Result result = Result::Ok; int calls = 0; int lastDisplay = -1; int lastDisplayable = -1;
  Result setActiveDisplayable(int d, int x) override { ++calls; lastDisplay = d; lastDisplayable = x; return result; }
};

} // namespace mpr3
