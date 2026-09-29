#include "mpr3/mocks.hpp"
#include <stdexcept>

namespace mpr3 {
std::optional<std::int64_t> MockCFDictionary::type() const {
  if (throwOnType) throw std::runtime_error("mock descriptor read failure");
  const auto found = integers.find("type");
  return found == integers.end() ? std::nullopt : std::optional<std::int64_t>{found->second};
}
std::optional<DescriptorCollection> MockCFRequest::streams() const {
  if (throwOnRead) throw std::runtime_error("mock request read failure");
  const auto found = arrays.find("streams");
  if (found == arrays.end() || !found->second) return std::nullopt;
  return found->second->values;
}
std::unique_ptr<ISetupRequest> MockCFRequest::withStreams(DescriptorCollection descriptors) const {
  if (throwOnCopy) throw std::runtime_error("mock request copy failure");
  if (failCopy) return nullptr;
  auto copy = std::make_unique<MockCFRequest>(*this);
  auto streams = std::make_shared<MockCFArray>();
  streams->values = std::move(descriptors);
  copy->arrays["streams"] = std::move(streams);
  return copy;
}
} // namespace mpr3
