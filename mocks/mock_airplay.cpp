#include "mpr3/mocks.hpp"
#include <stdexcept>

namespace mpr3 {
int MockAirPlay::setup(AirPlayReceiverSessionPrivate *session, const ISetupRequest &request,
                      SetupResponse *response) {
  ++calls;
  seenRequest = &request;
  seenSession = session;
  seenResponse = response;
  // The original handler observes malformed requests independently of the hook.
  const auto *mock = dynamic_cast<const MockCFRequest *>(&request);
  if (mock) {
    const auto streams = mock->arrays.find("streams");
    seenDescriptors = streams != mock->arrays.end() && streams->second
        ? streams->second->values : DescriptorCollection{};
    seenOpaqueFields = mock->opaqueFields;
  } else {
    seenDescriptors = request.streams().value_or(DescriptorCollection{});
  }
  if (response) response->status = result;
  return result;
}
OriginalSetupFn MockAirPlay::handler() {
  return [this](auto *session, const auto &request, auto *response) {
    return setup(session, request, response);
  };
}
bool MockSecondaryController::start(const DescriptorCollection &descriptors) noexcept {
  ++calls;
  try { received = descriptors; return result; } catch (...) { return false; }
}
void MockEventSink::onEvent(const Event &event) {
  if (throwOnEvent) throw std::runtime_error("mock event failure");
  events.push_back(event);
}
std::vector<EventKind> MockEventSink::kinds() const {
  std::vector<EventKind> result;
  for (const auto &event : events) result.push_back(event.kind);
  return result;
}
} // namespace mpr3
