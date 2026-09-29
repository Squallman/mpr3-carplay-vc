#pragma once
namespace mpr3 {
enum class SecondaryState {
  Idle, DescriptorReceived, Preparing, Starting, Running, Stopping, Failed
};
enum class SecondaryFailure {
  None, InvalidRequest, InvalidTransition, MultipleDescriptorsRejected,
  MissingConfiguration, DisplayableCreateFailed, PipelineCreateFailed,
  StreamStartFailed, DecoderStartFailed, VideoEncodingUnavailable,
  InvalidDisplayable, OutputActivationFailed, UnexpectedException
};
} // namespace mpr3
