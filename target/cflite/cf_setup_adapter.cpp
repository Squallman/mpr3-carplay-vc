#include "mpr3/target/cf_setup_adapter.hpp"
#include "cf_owned_reference.hpp"
#include <utility>

namespace mpr3::target {
CFSetupDescriptor::CFSetupDescriptor(CFLiteSetupContextRef context, CFDictionaryRef dictionary) noexcept
    : context_(std::move(context)), dictionary_(dictionary) {}
DescriptorRef CFSetupDescriptor::retaining(CFLiteSetupContextRef context, CFDictionaryRef dictionary) {
  if (!context || !dictionary) return {};
  const auto &api = context->api();
  const auto retained = api.retain(detail::object(dictionary));
  if (!retained) return {};
  detail::OwnedReference owner(api, retained);
  auto *descriptor = new CFSetupDescriptor(context, dictionary);
  owner.relinquish();
  return DescriptorRef(descriptor); // Deletes descriptor on control-block allocation failure.
}
CFSetupDescriptor::~CFSetupDescriptor() {
  context_->api().release(detail::object(dictionary_));
}
std::optional<std::int64_t> CFSetupDescriptor::type() const {
  CFLInt32 error = 0;
  const auto value = context_->api().dictionaryGetInt64(dictionary_, context_->typeKey(), &error);
  if (error) return std::nullopt;
  return static_cast<std::int64_t>(value);
}
CFSetupRequest::CFSetupRequest(CFLiteSetupContextRef context, CFDictionaryRef dictionary, bool owns) noexcept
    : context_(std::move(context)), dictionary_(dictionary), owns_(owns) {}
std::unique_ptr<CFSetupRequest> CFSetupRequest::borrowed(
    CFLiteSetupContextRef context, CFDictionaryRef dictionary) {
  if (!context || !dictionary) return {};
  return std::unique_ptr<CFSetupRequest>(new CFSetupRequest(std::move(context), dictionary, false));
}
CFSetupRequest::~CFSetupRequest() {
  if (owns_) context_->api().release(detail::object(dictionary_));
}
std::optional<DescriptorCollection> CFSetupRequest::streams() const {
  try {
    const auto &api = context_->api();
    CFLInt32 error = 0;
    const auto value = api.dictionaryGetTypedValue(dictionary_, context_->streamsKey(),
                                                   context_->arrayTypeID(), &error);
    if (error || !value) return std::nullopt;
    const auto array = reinterpret_cast<const OpaqueCFArray *>(value);
    const auto count = api.arrayGetCount(array);
    if (count < 0) return std::nullopt;
    DescriptorCollection descriptors;
    descriptors.reserve(static_cast<std::size_t>(count));
    for (CFLInt32 index = 0; index < count; ++index) {
      error = 0;
      const auto element = api.arrayGetTypedValueAtIndex(array, index,
                                                        context_->dictionaryTypeID(), &error);
      if (error || !element) return std::nullopt;
      auto descriptor = CFSetupDescriptor::retaining(context_,
          reinterpret_cast<CFDictionaryRef>(element));
      if (!descriptor) return std::nullopt;
      descriptors.push_back(std::move(descriptor));
    }
    return descriptors;
  } catch (...) {
    return std::nullopt;
  }
}
std::unique_ptr<ISetupRequest> CFSetupRequest::withStreams(DescriptorCollection descriptors) const {
  try {
    for (const auto &descriptor : descriptors) {
      const auto target = dynamic_cast<const CFSetupDescriptor *>(descriptor.get());
      if (!target || target->contextIdentity() != context_.get()) return {};
    }
    const auto &api = context_->api();
    auto *copy = api.dictionaryCreateMutableCopy(nullptr, 0, dictionary_);
    if (!copy) return {};
    detail::OwnedReference copyOwner(api, detail::object(copy));
    auto *array = api.arrayCreateMutable(nullptr, 0, api.typeArrayCallbacks);
    if (!array) return {};
    detail::OwnedReference arrayOwner(api, detail::object(array));
    for (const auto &descriptor : descriptors) {
      const auto target = static_cast<const CFSetupDescriptor *>(descriptor.get());
      if (api.arrayAppendValue(array, detail::object(target->rawDictionary())) != 0) return {};
    }
    if (api.dictionarySetValue(copy, context_->streamsKey(), detail::object(array)) != 0) return {};
    auto result = std::unique_ptr<CFSetupRequest>(new CFSetupRequest(context_, copy, true));
    copyOwner.relinquish();
    // arrayOwner drops the local reference; copy retains the replacement array.
    return result;
  } catch (...) {
    return {};
  }
}
} // namespace mpr3::target
