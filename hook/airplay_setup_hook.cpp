#include "mpr3/setup_filter.hpp"
#include <stdexcept>
#include <utility>
namespace mpr3 {
SetupFilterResult splitSecondaryDescriptors(const ISetupRequest &request) {
  try {
    const auto streams = request.streams();
    if (!streams) return {nullptr, {}, true};
    SetupFilterResult result;
    DescriptorCollection stock;
    for (const auto &descriptor : *streams) {
      if (!descriptor) return {nullptr, {}, true};
      const auto type = descriptor->type();
      if (!type) return {nullptr, {}, true};
      if (*type == 111) result.secondaryDescriptors.push_back(descriptor);
      else stock.push_back(descriptor);
    }
    if (!result.secondaryDescriptors.empty()) {
      result.stockRequest = request.withStreams(std::move(stock));
      if (!result.stockRequest) return {nullptr, {}, true};
    }
    return result;
  } catch (...) {
    return {nullptr, {}, true};
  }
}
AirPlaySetupHook::AirPlaySetupHook(OriginalSetupFn original,
    ISecondaryController &secondary, IEventSink *events)
    : original_(std::move(original)), secondary_(secondary), events_(events) {
  if (!original_) throw std::invalid_argument("SETUP requires an original stock handler");
}
int AirPlaySetupHook::setup(AirPlayReceiverSessionPrivate *session,
    const ISetupRequest &request, SetupResponse *response) {
  emit(events_, {EventKind::SetupReceived});
  auto filtered = splitSecondaryDescriptors(request);
  if (filtered.parseFailed) emit(events_, {EventKind::SetupFilterFailed});
  if (!filtered.secondaryDescriptors.empty())
    emit(events_, {EventKind::SecondaryDescriptorsExtracted, SecondaryFailure::None,
                   filtered.secondaryDescriptors.size()});
  const auto &stockRequest = filtered.stockRequest ? *filtered.stockRequest : request;
  // Stock SETUP completes first, including response and exact return status.
  const int stockResult = original_(session, stockRequest, response);
  emit(events_, {EventKind::StockSetupForwarded});
  if (!filtered.secondaryDescriptors.empty())
    (void)secondary_.start(filtered.secondaryDescriptors);
  return stockResult;
}
} // namespace mpr3
