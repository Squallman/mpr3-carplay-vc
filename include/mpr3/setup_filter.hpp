#pragma once

#include "mpr3/airplay_types.hpp"
#include <functional>

namespace mpr3 {

// ABI model only. The real target declaration is isolated here and is not
// asserted to be a usable host/CoreFoundation declaration.
using TargetSetupABI = int (*)(AirPlayReceiverSessionPrivate *, CFDictionaryRef,
                               CFDictionaryRef *); // libairplay.so @ 0x58dc0

using OriginalSetupFn = std::function<int(AirPlayReceiverSessionPrivate *,
                                          const MockCFRequest &, MockSetupResponse *)>;

struct SetupFilterResult {
  MockCFRequest stockRequest;
  std::vector<std::shared_ptr<MockCFDictionary>> secondaryDescriptors;
  bool hadSecondary = false;
  bool parseFailed = false;
};

SetupFilterResult splitSecondaryDescriptors(const MockCFRequest &request);

class AirPlaySetupHook {
 public:
  AirPlaySetupHook(OriginalSetupFn original, std::function<bool(const std::vector<std::shared_ptr<MockCFDictionary>> &)> secondaryStart);
  int setup(AirPlayReceiverSessionPrivate *session, const MockCFRequest &request,
            MockSetupResponse *response);
  const SetupFilterResult &lastFilter() const { return lastFilter_; }

 private:
  OriginalSetupFn original_;
  std::function<bool(const std::vector<std::shared_ptr<MockCFDictionary>> &)> secondaryStart_;
  SetupFilterResult lastFilter_;
};

} // namespace mpr3
