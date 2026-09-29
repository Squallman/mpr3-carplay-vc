#include "test_support.hpp"
void test_stock_110_preserved() {
  using namespace mpr3; using namespace mpr3_test;
  auto d110 = descriptor(110, 7); std::vector<std::shared_ptr<MockCFDictionary>> seen;
  AirPlaySetupHook hook(recorder(&seen), [](const auto &) { return true; });
  CHECK(hook.setup(nullptr, request({d110}), nullptr) == 0);
  CHECK(seen.size() == 1 && seen[0].get() == d110.get());
  CHECK(seen[0]->integers.at("unknown_marker") == 7);
}
