#include "test_support.hpp"
#include <limits>
#include <type_traits>
using namespace target_test;

void test_bridge_identity(TestRun &test) {
  Fixture f;
  const auto result = f.call();
  CHECK(result.state() == SetupBridgeState::ResolvedAndCalled);
  CHECK(result.resolution() == ResolutionStatus::Resolved);
  CHECK(f.log.session == f.session);
  CHECK(f.log.request == f.request);
  CHECK(f.log.responseOut == &f.responseOut);
  CHECK(f.responseOut == f.log.response);
  CHECK(f.log.calls == 1);
  CHECK(f.log.wrapperCalls == 0);
  CHECK(f.resolver.calls == 1);
  CHECK(f.resolver.lastWrapper == &wrapper);
}
void test_bridge_status(TestRun &test) {
  Fixture f;
  for (int status : {0, -27, 42, std::numeric_limits<int>::min(),
                    std::numeric_limits<int>::max()}) {
    const unsigned before = f.log.calls;
    f.log.status = status;
    const auto result = f.call();
    CHECK(result.originalStatus() != nullptr);
    CHECK(*result.originalStatus() == status);
    CHECK(f.log.calls == before + 1);
  }
  CHECK(f.resolver.calls == 5);
}
void test_bridge_unavailable(TestRun &test) {
  Fixture f;
  for (auto reason : {ResolutionStatus::MissingSymbol, ResolutionStatus::SelfReference}) {
    f.resolver.answer = {reason, nullptr};
    const auto previous = f.responseOut;
    const auto result = f.call();
    CHECK(result.state() == SetupBridgeState::ResolverUnavailable);
    CHECK(result.resolution() == reason);
    CHECK(result.originalStatus() == nullptr);
    CHECK(f.responseOut == previous);
    CHECK(f.log.calls == 0);
  }
  CHECK(f.resolver.calls == 2);
}
void test_bridge_defensive_self(TestRun &test) {
  Fixture f;
  f.resolver.answer = {ResolutionStatus::Resolved, &wrapper};
  const auto result = f.call();
  CHECK(result.resolution() == ResolutionStatus::SelfReference);
  CHECK(result.originalStatus() == nullptr);
  CHECK(f.log.calls == 0);
  CHECK(f.log.wrapperCalls == 0);
  f.resolver.answer = {ResolutionStatus::Resolved, nullptr};
  const auto missing = f.call();
  CHECK(missing.resolution() == ResolutionStatus::MissingSymbol);
  CHECK(missing.originalStatus() == nullptr);
}
void test_bridge_repeated(TestRun &test) {
  Fixture f;
  CHECK(f.call().state() == SetupBridgeState::ResolvedAndCalled);
  f.resolver.answer = {ResolutionStatus::MissingSymbol, nullptr};
  CHECK(f.call().originalStatus() == nullptr);
  f.resolver.answer = {ResolutionStatus::Resolved, &stock};
  char differentSession{}, differentRequest{};
  f.session = reinterpret_cast<mpr3::AirPlayReceiverSessionPrivate *>(&differentSession);
  f.request = reinterpret_cast<CFDictionaryRef>(&differentRequest);
  CFDictionaryRef separateOutput = nullptr;
  const auto result = passThroughSetup(f.resolver, &wrapper, f.session, f.request, &separateOutput);
  CHECK(result.state() == SetupBridgeState::ResolvedAndCalled);
  CHECK(f.log.session == f.session);
  CHECK(f.log.request == f.request);
  CHECK(f.log.responseOut == &separateOutput);
  CHECK(separateOutput == f.log.response);
  CHECK(f.log.calls == 2);
  CHECK(f.resolver.calls == 3);
}
void test_bridge_null_and_boundary(TestRun &test) {
  Fixture f;
  const auto result = passThroughSetup(f.resolver, &wrapper, nullptr, nullptr, nullptr);
  CHECK(result.state() == SetupBridgeState::ResolvedAndCalled);
  CHECK(f.log.session == nullptr);
  CHECK(f.log.request == nullptr);
  CHECK(f.log.responseOut == nullptr);
  CHECK(f.log.calls == 1);
  // Compile-time boundary: no request adapter, controller or service can be injected.
  using BridgeSignature = SetupBridgeResult (*)(IOriginalSetupResolver &, SetupABI,
      mpr3::AirPlayReceiverSessionPrivate *, CFDictionaryRef, CFDictionaryRef *);
  static_assert(std::is_same_v<decltype(&passThroughSetup), BridgeSignature>);
  CHECK(f.log.wrapperCalls == 0);
}
