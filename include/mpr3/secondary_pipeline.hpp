#pragma once
#include "mpr3/runtime_config.hpp"
#include "mpr3/secondary_session.hpp"
#include "mpr3/setup_request.hpp"
namespace mpr3 {
struct PipelineCreateResult {
  std::unique_ptr<SecondarySessionOwner> pipeline;
  SecondaryFailure failure = SecondaryFailure::PipelineCreateFailed;
};
class ISecondaryPipelineFactory {
 public:
  virtual ~ISecondaryPipelineFactory() = default;
  // Use RAII for partial construction; descriptors retain opaque identities.
  virtual PipelineCreateResult create(const DescriptorCollection &,
      const IDisplayable &, const RuntimeConfig &) = 0;
};
} // namespace mpr3
