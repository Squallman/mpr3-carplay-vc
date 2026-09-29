#pragma once
#include "mpr3/target/cflite_abi.hpp"
#include <optional>

namespace mpr3::target {
// Immutable after construction. No firmware addresses or process-global state.
struct CFLiteApi {
  CFArrayGetTypeIDFn const arrayGetTypeID = nullptr;
  CFDictionaryGetTypeIDFn const dictionaryGetTypeID = nullptr;
  CFDictionaryGetTypedValueFn const dictionaryGetTypedValue = nullptr;
  CFDictionaryGetInt64Fn const dictionaryGetInt64 = nullptr;
  CFArrayGetCountFn const arrayGetCount = nullptr;
  CFArrayGetTypedValueAtIndexFn const arrayGetTypedValueAtIndex = nullptr;
  CFDictionaryCreateMutableCopyFn const dictionaryCreateMutableCopy = nullptr;
  CFArrayCreateMutableFn const arrayCreateMutable = nullptr;
  CFArrayAppendValueFn const arrayAppendValue = nullptr;
  CFDictionarySetValueFn const dictionarySetValue = nullptr;
  CFStringCreateWithCStringFn const stringCreateWithCString = nullptr;
  CFRetainFn const retain = nullptr;
  CFReleaseFn const release = nullptr;
  const OpaqueCFLArrayCallbacks *const typeArrayCallbacks = nullptr;
  bool complete() const noexcept;
};
using UntypedCFLiteFunction = void (*)();
class ICFLiteSymbolLookup {
 public:
  virtual ~ICFLiteSymbolLookup() = default;
  virtual UntypedCFLiteFunction lookupFunction(const char *name) noexcept = 0;
  virtual const void *lookupData(const char *name) noexcept = 0;
};
struct CFLiteApiResolution {
  std::optional<CFLiteApi> api;
  const char *missingSymbol = nullptr; // First missing name in documented lookup order.
};
CFLiteApiResolution resolveCFLiteApi(ICFLiteSymbolLookup &) noexcept;

#ifdef MPR3_ENABLE_POSIX_CFLITE_RESOLVER
class PosixCFLiteSymbolLookup final : public ICFLiteSymbolLookup {
 public:
  UntypedCFLiteFunction lookupFunction(const char *) noexcept override;
  const void *lookupData(const char *) noexcept override;
};
#endif
} // namespace mpr3::target
