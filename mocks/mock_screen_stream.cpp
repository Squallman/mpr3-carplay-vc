#include "mpr3/mocks.hpp"
#include <stdexcept>

namespace mpr3 {
void MockLifecycle::record(CleanupStep step) noexcept {
  if (cleanupCount < 128) cleanupOrder[cleanupCount++] = step;
}
bool MockLifecycle::balanced() const noexcept {
  return displayablesCreated == displayablesDestroyed && streamsCreated == streamsDestroyed &&
         decodersCreated == decodersDestroyed && contextsCreated == contextsDestroyed;
}
MockScreenStream::MockScreenStream(std::shared_ptr<MockLifecycle> lifecycle,
    bool startResult, bool throwOnStart)
    : lifecycle_(std::move(lifecycle)), startResult_(startResult), throwOnStart_(throwOnStart) {
  ++lifecycle_->streamsCreated;
}
MockScreenStream::~MockScreenStream() {
  ++lifecycle_->streamsDestroyed;
  lifecycle_->record(CleanupStep::StreamDestroy);
}
bool MockScreenStream::start() {
  ++lifecycle_->streamStarts;
  if (throwOnStart_) throw std::runtime_error("mock stream start failure");
  return startResult_;
}
void MockScreenStream::stop() noexcept {
  ++lifecycle_->streamStops;
  lifecycle_->record(CleanupStep::StreamStop);
}
MockDecoder::MockDecoder(std::shared_ptr<MockLifecycle> lifecycle,
    bool startResult, bool throwOnStart)
    : lifecycle_(std::move(lifecycle)), startResult_(startResult), throwOnStart_(throwOnStart) {
  ++lifecycle_->decodersCreated;
}
MockDecoder::~MockDecoder() {
  ++lifecycle_->decodersDestroyed;
  lifecycle_->record(CleanupStep::DecoderDestroy);
}
bool MockDecoder::start() {
  ++lifecycle_->decoderStarts;
  if (throwOnStart_) throw std::runtime_error("mock decoder start failure");
  return startResult_;
}
void MockDecoder::stop() noexcept {
  ++lifecycle_->decoderStops;
  lifecycle_->record(CleanupStep::DecoderStop);
}
MockRenderContext::MockRenderContext(std::shared_ptr<MockLifecycle> lifecycle)
    : lifecycle_(std::move(lifecycle)) { ++lifecycle_->contextsCreated; }
MockRenderContext::~MockRenderContext() {
  ++lifecycle_->contextsDestroyed;
  lifecycle_->record(CleanupStep::ContextDestroy);
}
} // namespace mpr3
