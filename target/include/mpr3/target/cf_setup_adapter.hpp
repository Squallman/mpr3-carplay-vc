#pragma once
#include "mpr3/setup_request.hpp"
#include "mpr3/target/airplay_setup_abi.hpp"
#include "mpr3/target/cflite_api.hpp"

namespace mpr3::target {
class CFLiteSetupContext;
using CFLiteSetupContextRef = std::shared_ptr<const CFLiteSetupContext>;
enum class CFLiteContextFailure {
  None, IncompleteApi, InvalidTypeSelectors, StreamsKeyCreation, TypeKeyCreation, HostAllocation
};
struct CFLiteContextCreation {
  CFLiteSetupContextRef context;
  CFLiteContextFailure failure = CFLiteContextFailure::None;
};
class CFLiteSetupContext final {
 public:
  static CFLiteContextCreation create(const CFLiteApi &) noexcept;
  ~CFLiteSetupContext();
  CFLiteSetupContext(const CFLiteSetupContext &) = delete;
  CFLiteSetupContext &operator=(const CFLiteSetupContext &) = delete;
  const CFLiteApi &api() const noexcept { return api_; }
  const OpaqueCFString *streamsKey() const noexcept { return streamsKey_; }
  const OpaqueCFString *typeKey() const noexcept { return typeKey_; }
  CFLUInt32 arrayTypeID() const noexcept { return arrayTypeID_; }
  CFLUInt32 dictionaryTypeID() const noexcept { return dictionaryTypeID_; }
 private:
  CFLiteSetupContext(const CFLiteApi &, const OpaqueCFString *, const OpaqueCFString *,
                     CFLUInt32, CFLUInt32) noexcept;
  const CFLiteApi api_;
  const OpaqueCFString *const streamsKey_;
  const OpaqueCFString *const typeKey_;
  const CFLUInt32 arrayTypeID_;
  const CFLUInt32 dictionaryTypeID_;
};
class CFSetupDescriptor final : public ISetupDescriptor {
 public:
  static DescriptorRef retaining(CFLiteSetupContextRef, CFDictionaryRef);
  ~CFSetupDescriptor() override;
  CFSetupDescriptor(const CFSetupDescriptor &) = delete;
  CFSetupDescriptor &operator=(const CFSetupDescriptor &) = delete;
  std::optional<std::int64_t> type() const override;
  const void *identity() const noexcept override { return dictionary_; }
  CFDictionaryRef rawDictionary() const noexcept { return dictionary_; }
  const CFLiteSetupContext *contextIdentity() const noexcept { return context_.get(); }
 private:
  CFSetupDescriptor(CFLiteSetupContextRef, CFDictionaryRef) noexcept;
  CFLiteSetupContextRef context_;
  CFDictionaryRef dictionary_;
};
class CFSetupRequest final : public ISetupRequest {
 public:
  static std::unique_ptr<CFSetupRequest> borrowed(CFLiteSetupContextRef, CFDictionaryRef);
  ~CFSetupRequest() override;
  CFSetupRequest(const CFSetupRequest &) = delete;
  CFSetupRequest &operator=(const CFSetupRequest &) = delete;
  std::optional<DescriptorCollection> streams() const override;
  std::unique_ptr<ISetupRequest> withStreams(DescriptorCollection) const override;
  CFDictionaryRef rawDictionary() const noexcept { return dictionary_; }
  const CFLiteSetupContext *contextIdentity() const noexcept { return context_.get(); }
 private:
  CFSetupRequest(CFLiteSetupContextRef, CFDictionaryRef, bool owns) noexcept;
  CFLiteSetupContextRef context_;
  CFDictionaryRef dictionary_;
  bool owns_;
};
enum class TargetRequestSelection { Original, Filtered };
struct TargetFilterResult {
  bool parseFailed = false;
  CFDictionaryRef originalRequest = nullptr; // Caller-owned.
  std::unique_ptr<CFSetupRequest> filteredRequest;
  DescriptorCollection secondaryDescriptors;
  TargetRequestSelection selection() const noexcept {
    return filteredRequest ? TargetRequestSelection::Filtered : TargetRequestSelection::Original;
  }
  CFDictionaryRef selectedRawRequest() const noexcept {
    return filteredRequest ? filteredRequest->rawDictionary() : originalRequest;
  }
};
// Offline integration boundary. The exported C SETUP entrypoint does not call this.
TargetFilterResult filterTargetSetupRequest(CFLiteSetupContextRef, CFDictionaryRef) noexcept;
} // namespace mpr3::target
