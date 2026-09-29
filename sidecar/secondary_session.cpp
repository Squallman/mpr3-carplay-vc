#include "mpr3/secondary_session.hpp"
#include <utility>
namespace mpr3 {
SecondarySessionOwner::SecondarySessionOwner(std::unique_ptr<IScreenStream> stream,
    std::unique_ptr<IDecoder> decoder, std::unique_ptr<IRenderContext> context)
    : stream_(std::move(stream)), decoder_(std::move(decoder)), context_(std::move(context)) {}
SecondarySessionOwner::~SecondarySessionOwner() { stop(); }
SecondaryFailure SecondarySessionOwner::start() {
  if (streamAttempted_ || decoderAttempted_) return SecondaryFailure::InvalidTransition;
  if (!stream_ || !decoder_ || !context_) return SecondaryFailure::PipelineCreateFailed;
  streamAttempted_ = true; // Failing/throwing start may allocate resources.
  if (!stream_->start()) return SecondaryFailure::StreamStartFailed;
  decoderAttempted_ = true;
  if (!decoder_->start()) return SecondaryFailure::DecoderStartFailed;
  return SecondaryFailure::None;
}
void SecondarySessionOwner::stop() noexcept {
  if (decoderAttempted_) { decoder_->stop(); decoderAttempted_ = false; }
  if (streamAttempted_) { stream_->stop(); streamAttempted_ = false; }
  decoder_.reset();
  stream_.reset();
  context_.reset();
}
} // namespace mpr3
