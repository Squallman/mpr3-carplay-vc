#include "mpr3/secondary_controller.hpp"
#include <utility>
namespace mpr3 {
namespace {
struct OperationGuard {
  bool &active;
  explicit OperationGuard(bool &flag) noexcept : active(flag) { active = true; }
  ~OperationGuard() { active = false; }
};
} // namespace
SecondaryController::SecondaryController(RuntimeConfig config,
    IDisplayableProvider &displayables, ISecondaryPipelineFactory &factory,
    IVideoEncodingClient &videoEncoding, IEventSink *events)
    : config_(std::move(config)), displayables_(displayables), factory_(factory),
      videoEncoding_(videoEncoding), events_(events) {}
SecondaryController::~SecondaryController() { stop(); }
void SecondaryController::cleanup() noexcept {
  pipeline_.reset(); // Stop/destroy before releasing the displayable.
  displayable_.reset();
  descriptors_.clear();
}
bool SecondaryController::fail(SecondaryFailure reason) noexcept {
  cleanup();
  failure_ = reason;
  state_ = SecondaryState::Failed;
  emit(events_, {EventKind::SecondaryFailed, reason});
  return false;
}
bool SecondaryController::start(const DescriptorCollection &descriptors) noexcept {
  if (inOperation_ || (state_ != SecondaryState::Idle && state_ != SecondaryState::Failed)) {
    emit(events_, {EventKind::SecondaryAttemptRejected, SecondaryFailure::InvalidTransition});
    return false;
  }
  OperationGuard operation(inOperation_);
  failure_ = SecondaryFailure::None;
  activation_ = ActivationResult::NotAttempted;
  receivedCount_ = descriptors.size();
  state_ = SecondaryState::DescriptorReceived;
  try {
    if (descriptors.empty()) return fail(SecondaryFailure::InvalidRequest);
    for (const auto &descriptor : descriptors) {
      if (!descriptor || descriptor->type() != 111) return fail(SecondaryFailure::InvalidRequest);
    }
    if (descriptors.size() > 1 && config_.multiSecondaryPolicy == MultiSecondaryPolicy::Reject)
      return fail(SecondaryFailure::MultipleDescriptorsRejected);
    descriptors_ = descriptors; // Retain the entire collection in original order.
    state_ = SecondaryState::Preparing;
    emit(events_, {EventKind::SecondaryPreparing});
    if (!config_.validate().validForOffline) return fail(SecondaryFailure::MissingConfiguration);
    auto created = displayables_.createByConfiguredName(config_.displayable);
    displayable_ = std::move(created.displayable);
    if (created.status != DisplayableCreateStatus::Created || !displayable_)
      return fail(SecondaryFailure::DisplayableCreateFailed);
    const auto &handle = displayable_->handle();
    if (handle.configuredName != config_.displayable.configuredName ||
        (handle.resolvedId && config_.displayable.resolvedId &&
         handle.resolvedId != config_.displayable.resolvedId))
      return fail(SecondaryFailure::InvalidDisplayable);
    emit(events_, {EventKind::DisplayableCreated});
    const auto id = handle.resolvedId ? handle.resolvedId : config_.displayable.resolvedId;
    if (!config_.clusterDisplayId) activation_ = ActivationResult::MissingEndpoint;
    else if (!id) activation_ = ActivationResult::MissingDisplayableId;
    if (!config_.validate(id).activationInputsKnown &&
        config_.outputPolicy == OutputPolicy::RequireActivationInputs)
      return fail(SecondaryFailure::MissingConfiguration);
    // Single-screen host pipeline: explicit FirstOnly prototype policy.
    DescriptorCollection selected{descriptors_.front()};
    auto createdPipeline = factory_.create(selected, *displayable_, config_);
    pipeline_ = std::move(createdPipeline.pipeline);
    if (createdPipeline.failure != SecondaryFailure::None || !pipeline_)
      return fail(SecondaryFailure::PipelineCreateFailed);
    state_ = SecondaryState::Starting;
    const auto startFailure = pipeline_->start();
    if (startFailure != SecondaryFailure::None) return fail(startFailure);
    emit(events_, {EventKind::PipelineStarted});
    if (config_.validate(id).activationInputsKnown) {
      activation_ = ActivationResult::Failed; // Also diagnoses a throwing client.
      activation_ = videoEncoding_.setActiveDisplayable(*config_.clusterDisplayId, *id);
      if (activation_ == ActivationResult::Unavailable)
        return fail(SecondaryFailure::VideoEncodingUnavailable);
      if (activation_ == ActivationResult::InvalidDisplayable)
        return fail(SecondaryFailure::InvalidDisplayable);
      if (activation_ != ActivationResult::Activated)
        return fail(SecondaryFailure::OutputActivationFailed);
      emit(events_, {EventKind::OutputActivated});
    } else {
      emit(events_, {EventKind::OutputActivationSkipped});
    }
    state_ = SecondaryState::Running;
    emit(events_, {EventKind::SecondaryRunning});
    return true;
  } catch (...) {
    return fail(SecondaryFailure::UnexpectedException);
  }
}
void SecondaryController::stop() noexcept {
  if (inOperation_ || state_ == SecondaryState::Idle) return;
  OperationGuard operation(inOperation_);
  state_ = SecondaryState::Stopping;
  cleanup();
  state_ = SecondaryState::Idle;
  emit(events_, {EventKind::SecondaryStopped});
}
} // namespace mpr3
