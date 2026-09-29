#include "mpr3/setup_filter.hpp"

namespace mpr3 {

int DescriptorRef::type() const {
  if (!value) return -1;
  auto it = value->integers.find("type");
  return it == value->integers.end() ? -1 : static_cast<int>(it->second);
}

SetupFilterResult splitSecondaryDescriptors(const MockCFRequest &request) {
  SetupFilterResult result;
  auto it = request.arrays.find("streams");
  if (it == request.arrays.end() || !it->second) {
    result.parseFailed = true;
    result.stockRequest = request;
    return result;
  }
  auto filtered = std::make_shared<MockCFArray>();
  for (const auto &descriptor : it->second->values) {
    if (!descriptor) { result.parseFailed = true; continue; }
    DescriptorRef ref(descriptor);
    if (ref.type() == 111) {
      result.hadSecondary = true;
      result.secondaryDescriptors.push_back(descriptor); // retain identity
    } else {
      filtered->values.push_back(descriptor); // shallow append, identity preserved
    }
  }
  result.stockRequest = request;
  result.stockRequest.arrays["streams"] = std::move(filtered);
  return result;
}

AirPlaySetupHook::AirPlaySetupHook(OriginalSetupFn original,
                                   std::function<bool(const std::vector<std::shared_ptr<MockCFDictionary>> &)> secondaryStart)
    : original_(std::move(original)), secondaryStart_(std::move(secondaryStart)) {}

int AirPlaySetupHook::setup(AirPlayReceiverSessionPrivate *session,
                            const MockCFRequest &request, MockSetupResponse *response) {
  lastFilter_ = splitSecondaryDescriptors(request);
  if (lastFilter_.parseFailed) return original_(session, request, response);
  if (!lastFilter_.hadSecondary) return original_(session, request, response);
  // Fail-open: secondary startup is advisory and cannot alter stock result.
  if (secondaryStart_) (void)secondaryStart_(lastFilter_.secondaryDescriptors);
  return original_(session, lastFilter_.stockRequest, response);
}

} // namespace mpr3
