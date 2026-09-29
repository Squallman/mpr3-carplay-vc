#include "fake_cflite.hpp"
#include "test_support.hpp"
#include <array>
#include <cstring>

using namespace cflite_test;
using target_test::TestRun;
namespace {
constexpr std::array<const char *, 14> names{{
  "CFArrayGetTypeID", "CFDictionaryGetTypeID", "CFDictionaryGetTypedValue", "CFDictionaryGetInt64",
  "CFArrayGetCount", "CFArrayGetTypedValueAtIndex", "CFDictionaryCreateMutableCopy",
  "CFArrayCreateMutable", "CFArrayAppendValue", "CFDictionarySetValue", "CFStringCreateWithCString",
  "CFRetain", "CFRelease", "kCFLArrayCallBacksCFLTypes"
}};
class Lookup final : public ICFLiteSymbolLookup {
 public:
  explicit Lookup(const CFLiteApi &api) : functions{{
    reinterpret_cast<UntypedCFLiteFunction>(api.arrayGetTypeID),
    reinterpret_cast<UntypedCFLiteFunction>(api.dictionaryGetTypeID),
    reinterpret_cast<UntypedCFLiteFunction>(api.dictionaryGetTypedValue),
    reinterpret_cast<UntypedCFLiteFunction>(api.dictionaryGetInt64),
    reinterpret_cast<UntypedCFLiteFunction>(api.arrayGetCount),
    reinterpret_cast<UntypedCFLiteFunction>(api.arrayGetTypedValueAtIndex),
    reinterpret_cast<UntypedCFLiteFunction>(api.dictionaryCreateMutableCopy),
    reinterpret_cast<UntypedCFLiteFunction>(api.arrayCreateMutable),
    reinterpret_cast<UntypedCFLiteFunction>(api.arrayAppendValue),
    reinterpret_cast<UntypedCFLiteFunction>(api.dictionarySetValue),
    reinterpret_cast<UntypedCFLiteFunction>(api.stringCreateWithCString),
    reinterpret_cast<UntypedCFLiteFunction>(api.retain),
    reinterpret_cast<UntypedCFLiteFunction>(api.release)
  }}, callbacks(api.typeArrayCallbacks) {}
  int missing = -1;
  std::array<const char *, 14> requested{};
  unsigned calls = 0;
  bool kindsCorrect = true;
  UntypedCFLiteFunction lookupFunction(const char *name) noexcept override {
    const auto index = calls++;
    if (index >= requested.size()) std::abort();
    requested[index] = name;
    kindsCorrect &= index < functions.size();
    if (index >= functions.size() || std::strcmp(name, names[index]) != 0) return nullptr;
    return static_cast<int>(index) == missing ? nullptr : functions[index];
  }
  const void *lookupData(const char *name) noexcept override {
    const auto index = calls++;
    if (index >= requested.size()) std::abort();
    requested[index] = name;
    kindsCorrect &= index == 13;
    return index == 13 && std::strcmp(name, names[index]) == 0 && missing != 13 ? callbacks : nullptr;
  }
 private:
  std::array<UntypedCFLiteFunction, 13> functions;
  const OpaqueCFLArrayCallbacks *callbacks;
};
}
void test_cf_symbol_resolution(TestRun &test) {
  FakeCFLite runtime;
  const auto api = runtime.api();
  CHECK(api.complete());
  CHECK(!CFLiteApi{}.complete());
  for (int missing = -1; missing < 14; ++missing) {
    Lookup lookup(api);
    lookup.missing = missing;
    const auto result = resolveCFLiteApi(lookup);
    CHECK(lookup.calls == 14);
    CHECK(lookup.kindsCorrect);
    for (unsigned i = 0; i < names.size(); ++i) CHECK(std::strcmp(lookup.requested[i], names[i]) == 0);
    if (missing < 0) {
      CHECK(result.api);
      CHECK(result.api->complete());
      CHECK(!result.missingSymbol);
      CHECK(result.api->arrayGetTypeID == api.arrayGetTypeID);
      CHECK(result.api->dictionaryGetTypeID == api.dictionaryGetTypeID);
      CHECK(result.api->dictionaryGetTypedValue == api.dictionaryGetTypedValue);
      CHECK(result.api->dictionaryGetInt64 == api.dictionaryGetInt64);
      CHECK(result.api->arrayGetCount == api.arrayGetCount);
      CHECK(result.api->arrayGetTypedValueAtIndex == api.arrayGetTypedValueAtIndex);
      CHECK(result.api->dictionaryCreateMutableCopy == api.dictionaryCreateMutableCopy);
      CHECK(result.api->arrayCreateMutable == api.arrayCreateMutable);
      CHECK(result.api->arrayAppendValue == api.arrayAppendValue);
      CHECK(result.api->dictionarySetValue == api.dictionarySetValue);
      CHECK(result.api->stringCreateWithCString == api.stringCreateWithCString);
      CHECK(result.api->retain == api.retain);
      CHECK(result.api->release == api.release);
      CHECK(result.api->typeArrayCallbacks == api.typeArrayCallbacks);
    } else {
      CHECK(!result.api); // No partial success or address fallback.
      CHECK(result.missingSymbol);
      CHECK(std::strcmp(result.missingSymbol, names[static_cast<unsigned>(missing)]) == 0);
    }
  }
}
