#pragma once
#include "mpr3/mocks.hpp"
#include <cstdlib>
#include <iostream>
#include <memory>

namespace mpr3_test {
struct TestRun {
  std::size_t assertions = 0;
  void check(bool value, const char *expr, const char *file, int line) {
    ++assertions;
    if (!value) {
      std::cerr << file << ':' << line << ": failed: " << expr << '\n';
      std::exit(1);
    }
  }
};
#define CHECK(x) test.check(static_cast<bool>(x), #x, __FILE__, __LINE__)

inline std::shared_ptr<mpr3::MockCFDictionary> descriptor(std::int64_t type, int marker = 0) {
  auto d = std::make_shared<mpr3::MockCFDictionary>();
  d->integers["type"] = type;
  d->integers["unknown_marker"] = marker;
  d->strings["opaque_field"] = "preserve-me";
  return d;
}
inline mpr3::MockCFRequest request(std::initializer_list<mpr3::DescriptorRef> ds) {
  mpr3::MockCFRequest r;
  auto a = std::make_shared<mpr3::MockCFArray>();
  a->values = ds;
  r.arrays["streams"] = std::move(a);
  r.opaqueFields["opaque_request_field"] = "untouched";
  return r;
}
inline mpr3::RuntimeConfig testConfig() {
  mpr3::RuntimeConfig config;
  // HOST TEST DATA ONLY. No target safety or endpoint semantics are implied.
  config.displayable = {"Displayable_Debug_1", 137};
  config.clusterDisplayId = 9001;
  return config;
}
struct Fixture {
  std::shared_ptr<mpr3::MockLifecycle> counters = std::make_shared<mpr3::MockLifecycle>();
  mpr3::MockEventSink events;
  mpr3::MockDisplayableProvider displayables{counters};
  mpr3::MockPipelineFactory factory{counters};
  mpr3::MockVideoEncoding video;
  mpr3::MockAirPlay stock;
  std::unique_ptr<mpr3::SecondaryController> controller;
  explicit Fixture(mpr3::RuntimeConfig config = testConfig())
      : controller(std::make_unique<mpr3::SecondaryController>(
            std::move(config), displayables, factory, video, &events)) {}
  bool start() { return controller->start({descriptor(111)}); }
  int setup(const mpr3::ISetupRequest &r, mpr3::SetupResponse *out = nullptr) {
    mpr3::AirPlaySetupHook hook(stock.handler(), *controller, &events);
    return hook.setup(nullptr, r, out);
  }
};
} // namespace mpr3_test
