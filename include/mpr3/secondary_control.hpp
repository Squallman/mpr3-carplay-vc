#pragma once
#include "mpr3/setup_request.hpp"
namespace mpr3 {
class ISecondaryController {
 public:
  virtual ~ISecondaryController() = default;
  virtual bool start(const DescriptorCollection &) noexcept = 0;
};
} // namespace mpr3
