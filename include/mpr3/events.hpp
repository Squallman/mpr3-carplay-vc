#pragma once
#include "mpr3/secondary_status.hpp"
#include <cstddef>
namespace mpr3 {
enum class EventKind {
  SetupReceived, SetupFilterFailed, SecondaryDescriptorsExtracted,
  StockSetupForwarded, SecondaryPreparing, DisplayableCreated, PipelineStarted,
  OutputActivated, OutputActivationSkipped, SecondaryRunning, SecondaryFailed,
  SecondaryStopped, SecondaryAttemptRejected
};
struct Event {
  EventKind kind;
  SecondaryFailure failure = SecondaryFailure::None;
  std::size_t descriptorCount = 0;
};
class IEventSink {
 public:
  virtual ~IEventSink() = default;
  virtual void onEvent(const Event &) = 0;
};
// Observability cannot interrupt stock forwarding or cleanup.
inline void emit(IEventSink *sink, Event event) noexcept {
  if (!sink) return;
  try { sink->onEvent(event); } catch (...) {}
}
} // namespace mpr3
