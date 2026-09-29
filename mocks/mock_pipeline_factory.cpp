#include "mpr3/mocks.hpp"
#include <stdexcept>

namespace mpr3 {
PipelineCreateResult MockPipelineFactory::create(const DescriptorCollection &descriptors,
    const IDisplayable &displayable, const RuntimeConfig &config) {
  ++calls;
  received = descriptors;
  receivedDisplayable = displayable.handle();
  receivedConfig = config;
  if (throwOnCreate) throw std::runtime_error("mock pipeline create failure");
  if (failure == MockFactoryFailure::StreamCreate) return {};
  auto stream = std::make_unique<MockScreenStream>(lifecycle, streamStartResult, throwOnStreamStart);
  if (failure == MockFactoryFailure::DecoderCreate) return {};
  auto decoder = std::make_unique<MockDecoder>(lifecycle, decoderStartResult, throwOnDecoderStart);
  if (failure == MockFactoryFailure::ContextCreate) return {};
  auto context = std::make_unique<MockRenderContext>(lifecycle);
  return {std::make_unique<SecondarySessionOwner>(std::move(stream), std::move(decoder),
                                                std::move(context)), SecondaryFailure::None};
}
} // namespace mpr3
