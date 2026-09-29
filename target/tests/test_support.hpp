#pragma once
#include "mpr3/target/target_setup_bridge.hpp"
#include <cstdlib>
#include <iostream>

namespace target_test {
struct TestRun {
  unsigned assertions = 0;
  void check(bool value, const char *expression, int line) {
    ++assertions;
    if (!value) {
      std::cerr << "target contract check failed at line " << line << ": " << expression << '\n';
      std::exit(1);
    }
  }
};
#define CHECK(x) test.check(static_cast<bool>(x), #x, __LINE__)

using namespace mpr3::target;
class FakeLookup final : public ISetupSymbolLookup {
 public:
  SetupABI candidate = nullptr;
  unsigned calls = 0;
  SetupABI lookupOriginalSetup() noexcept override { ++calls; return candidate; }
};
class FakeResolver final : public IOriginalSetupResolver {
 public:
  SetupResolution answer{ResolutionStatus::MissingSymbol, nullptr};
  SetupABI lastWrapper = nullptr;
  unsigned calls = 0;
  SetupResolution resolve(SetupABI wrapper) noexcept override {
    ++calls;
    lastWrapper = wrapper;
    return answer;
  }
};
struct CallLog {
  unsigned calls = 0;
  unsigned wrapperCalls = 0;
  mpr3::AirPlayReceiverSessionPrivate *session = nullptr;
  CFDictionaryRef request = nullptr;
  CFDictionaryRef *responseOut = nullptr;
  CFDictionaryRef response = nullptr;
  int status = 0;
};
inline thread_local CallLog *activeLog = nullptr; // Test instrumentation only.
inline int stock(mpr3::AirPlayReceiverSessionPrivate *session,
                 CFDictionaryRef request, CFDictionaryRef *responseOut) {
  ++activeLog->calls;
  activeLog->session = session;
  activeLog->request = request;
  activeLog->responseOut = responseOut;
  if (responseOut) *responseOut = activeLog->response;
  return activeLog->status;
}
inline int wrapper(mpr3::AirPlayReceiverSessionPrivate *, CFDictionaryRef, CFDictionaryRef *) {
  ++activeLog->wrapperCalls;
  return -99; // Test data; this function must never be called.
}
struct Fixture {
  // Addresses stand in for opaque objects; tests never dereference those objects.
  char sessionToken{}, requestToken{}, responseToken{}, previousResponseToken{};
  mpr3::AirPlayReceiverSessionPrivate *session =
      reinterpret_cast<mpr3::AirPlayReceiverSessionPrivate *>(&sessionToken);
  CFDictionaryRef request = reinterpret_cast<CFDictionaryRef>(&requestToken);
  CFDictionaryRef responseOut = reinterpret_cast<CFDictionaryRef>(&previousResponseToken);
  CallLog log;
  FakeResolver resolver;
  Fixture() {
    activeLog = &log;
    log.response = reinterpret_cast<CFDictionaryRef>(&responseToken);
    resolver.answer = {ResolutionStatus::Resolved, &stock};
  }
  ~Fixture() { activeLog = nullptr; }
  SetupBridgeResult call() {
    return passThroughSetup(resolver, &wrapper, session, request, &responseOut);
  }
};
} // namespace target_test
