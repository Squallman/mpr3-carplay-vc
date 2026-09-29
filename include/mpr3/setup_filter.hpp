#pragma once
#include "mpr3/airplay_types.hpp"
#include "mpr3/events.hpp"
#include "mpr3/secondary_control.hpp"
#include <functional>
namespace mpr3 {
using OriginalSetupFn = std::function<int(AirPlayReceiverSessionPrivate *,
                                        const ISetupRequest &, SetupResponse *)>;
struct SetupFilterResult {
  // Null means forward the original request object unchanged.
  std::unique_ptr<ISetupRequest> stockRequest;
  DescriptorCollection secondaryDescriptors;
  bool parseFailed = false;
};
SetupFilterResult splitSecondaryDescriptors(const ISetupRequest &);
class AirPlaySetupHook {
 public:
  AirPlaySetupHook(OriginalSetupFn, ISecondaryController &, IEventSink * = nullptr);
  int setup(AirPlayReceiverSessionPrivate *, const ISetupRequest &, SetupResponse *);
 private:
  OriginalSetupFn original_;
  ISecondaryController &secondary_;
  IEventSink *events_;
};
} // namespace mpr3
