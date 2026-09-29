#include "mpr3/target/cf_setup_adapter.hpp"
#include "cf_owned_reference.hpp"

namespace mpr3::target {
CFLiteSetupContext::CFLiteSetupContext(const CFLiteApi &api, const OpaqueCFString *streams,
    const OpaqueCFString *type, CFLUInt32 arrayID, CFLUInt32 dictionaryID) noexcept
    : api_(api), streamsKey_(streams), typeKey_(type), arrayTypeID_(arrayID),
      dictionaryTypeID_(dictionaryID) {}
CFLiteSetupContext::~CFLiteSetupContext() {
  api_.release(detail::object(typeKey_));
  api_.release(detail::object(streamsKey_));
}
CFLiteContextCreation CFLiteSetupContext::create(const CFLiteApi &api) noexcept {
  if (!api.complete()) return {{}, CFLiteContextFailure::IncompleteApi};
  try {
    const auto arrayID = api.arrayGetTypeID();
    const auto dictionaryID = api.dictionaryGetTypeID();
    if (!arrayID || !dictionaryID || arrayID == dictionaryID)
      return {{}, CFLiteContextFailure::InvalidTypeSelectors};
    const auto streams = api.stringCreateWithCString(nullptr, "streams", recoveredCStringEncodingSelector);
    if (!streams) return {{}, CFLiteContextFailure::StreamsKeyCreation};
    detail::OwnedReference streamsOwner(api, detail::object(streams));
    const auto type = api.stringCreateWithCString(nullptr, "type", recoveredCStringEncodingSelector);
    if (!type) return {{}, CFLiteContextFailure::TypeKeyCreation};
    detail::OwnedReference typeOwner(api, detail::object(type));
    auto *context = new CFLiteSetupContext(api, streams, type, arrayID, dictionaryID);
    streamsOwner.relinquish();
    typeOwner.relinquish();
    // shared_ptr deletes context if allocating its control block throws.
    return {CFLiteSetupContextRef(context), CFLiteContextFailure::None};
  } catch (...) {
    return {{}, CFLiteContextFailure::HostAllocation};
  }
}
} // namespace mpr3::target
