#include "mpr3/target/cflite_api.hpp"

namespace mpr3::target {
bool CFLiteApi::complete() const noexcept {
  return arrayGetTypeID && dictionaryGetTypeID && dictionaryGetTypedValue &&
      dictionaryGetInt64 && arrayGetCount && arrayGetTypedValueAtIndex &&
      dictionaryCreateMutableCopy && arrayCreateMutable && arrayAppendValue &&
      dictionarySetValue && stringCreateWithCString && retain && release && typeArrayCallbacks;
}
CFLiteApiResolution resolveCFLiteApi(ICFLiteSymbolLookup &lookup) noexcept {
  const char *missing = nullptr;
  auto function = [&](const char *name) {
    const auto result = lookup.lookupFunction(name);
    if (!result && !missing) missing = name;
    return result;
  };
  // Braced initializer evaluation is ordered. Always request all 14 exports once.
  const CFLiteApi api{
      reinterpret_cast<CFArrayGetTypeIDFn>(function("CFArrayGetTypeID")),
      reinterpret_cast<CFDictionaryGetTypeIDFn>(function("CFDictionaryGetTypeID")),
      reinterpret_cast<CFDictionaryGetTypedValueFn>(function("CFDictionaryGetTypedValue")),
      reinterpret_cast<CFDictionaryGetInt64Fn>(function("CFDictionaryGetInt64")),
      reinterpret_cast<CFArrayGetCountFn>(function("CFArrayGetCount")),
      reinterpret_cast<CFArrayGetTypedValueAtIndexFn>(function("CFArrayGetTypedValueAtIndex")),
      reinterpret_cast<CFDictionaryCreateMutableCopyFn>(function("CFDictionaryCreateMutableCopy")),
      reinterpret_cast<CFArrayCreateMutableFn>(function("CFArrayCreateMutable")),
      reinterpret_cast<CFArrayAppendValueFn>(function("CFArrayAppendValue")),
      reinterpret_cast<CFDictionarySetValueFn>(function("CFDictionarySetValue")),
      reinterpret_cast<CFStringCreateWithCStringFn>(function("CFStringCreateWithCString")),
      reinterpret_cast<CFRetainFn>(function("CFRetain")),
      reinterpret_cast<CFReleaseFn>(function("CFRelease")),
      static_cast<const OpaqueCFLArrayCallbacks *>(lookup.lookupData("kCFLArrayCallBacksCFLTypes"))};
  if (!api.typeArrayCallbacks && !missing) missing = "kCFLArrayCallBacksCFLTypes";
  if (missing) return {std::nullopt, missing};
  return {api, nullptr};
}
} // namespace mpr3::target
