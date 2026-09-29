#include "mpr3/target/cf_setup_adapter.hpp"
#include "mpr3/setup_filter.hpp"
#include <utility>

namespace mpr3::target {
TargetFilterResult filterTargetSetupRequest(CFLiteSetupContextRef context, CFDictionaryRef request) noexcept {
  TargetFilterResult result;
  result.originalRequest = request;
  try {
    auto borrowed = CFSetupRequest::borrowed(context, request);
    if (!borrowed) { result.parseFailed = true; return result; }
    auto split = splitSecondaryDescriptors(*borrowed);
    result.parseFailed = split.parseFailed;
    if (split.parseFailed) return result;
    if (split.stockRequest) {
      auto *target = dynamic_cast<CFSetupRequest *>(split.stockRequest.get());
      if (!target || target->contextIdentity() != context.get()) {
        result.parseFailed = true;
        return result;
      }
      split.stockRequest.release();
      result.filteredRequest.reset(target);
    }
    result.secondaryDescriptors = std::move(split.secondaryDescriptors);
  } catch (...) {
    result.parseFailed = true;
    result.filteredRequest.reset();
    result.secondaryDescriptors.clear();
  }
  return result;
}
} // namespace mpr3::target
