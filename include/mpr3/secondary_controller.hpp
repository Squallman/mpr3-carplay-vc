#pragma once
#include "mpr3/events.hpp"
#include "mpr3/secondary_control.hpp"
#include "mpr3/secondary_pipeline.hpp"
#include "mpr3/video_encoding.hpp"
namespace mpr3 {
class SecondaryController final : public ISecondaryController {
 public:
  // Injected services and event sink must outlive the controller.
  SecondaryController(RuntimeConfig, IDisplayableProvider &,
      ISecondaryPipelineFactory &, IVideoEncodingClient &, IEventSink * = nullptr);
  ~SecondaryController();
  SecondaryController(const SecondaryController &) = delete;
  SecondaryController &operator=(const SecondaryController &) = delete;
  bool start(const DescriptorCollection &) noexcept override;
  void stop() noexcept;
  SecondaryState state() const noexcept { return state_; }
  SecondaryFailure failure() const noexcept { return failure_; }
  ActivationResult activationResult() const noexcept { return activation_; }
  std::size_t receivedDescriptorCount() const noexcept { return receivedCount_; }
 private:
  bool fail(SecondaryFailure) noexcept;
  void cleanup() noexcept;
  RuntimeConfig config_;
  IDisplayableProvider &displayables_;
  ISecondaryPipelineFactory &factory_;
  IVideoEncodingClient &videoEncoding_;
  IEventSink *events_;
  SecondaryState state_ = SecondaryState::Idle;
  SecondaryFailure failure_ = SecondaryFailure::None;
  ActivationResult activation_ = ActivationResult::NotAttempted;
  std::size_t receivedCount_ = 0;
  DescriptorCollection descriptors_;
  std::unique_ptr<IDisplayable> displayable_;
  std::unique_ptr<SecondarySessionOwner> pipeline_;
  bool inOperation_ = false;
};
} // namespace mpr3
