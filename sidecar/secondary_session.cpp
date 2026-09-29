#include "mpr3/secondary_session.hpp"

namespace mpr3 {

SecondarySessionOwner::SecondarySessionOwner(std::unique_ptr<IScreenStream> stream,
                                             std::unique_ptr<IDecoder> decoder,
                                             std::unique_ptr<IDisplayable> displayable)
    : stream_(std::move(stream)), decoder_(std::move(decoder)), displayable_(std::move(displayable)) {}

bool SecondarySessionOwner::start() {
  if (state_ != SecondaryState::Idle || !stream_ || !decoder_ || !displayable_ || !displayable_->valid()) {
    state_ = SecondaryState::Failed; return false;
  }
  state_ = SecondaryState::DescriptorReceived;
  state_ = SecondaryState::Starting;
  if (!stream_->start() || !decoder_->start()) {
    stream_->stop(); decoder_->stop(); state_ = SecondaryState::Failed; return false;
  }
  state_ = SecondaryState::Running; return true;
}

void SecondarySessionOwner::stop() {
  if (state_ == SecondaryState::Idle || state_ == SecondaryState::Failed) return;
  state_ = SecondaryState::Stopping;
  if (decoder_) decoder_->stop();
  if (stream_) stream_->stop();
  state_ = SecondaryState::Idle;
}

} // namespace mpr3
