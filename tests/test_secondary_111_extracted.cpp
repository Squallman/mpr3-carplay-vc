#include "test_support.hpp"
void test_secondary_111_extracted() {
  using namespace mpr3; using namespace mpr3_test;
  auto d110 = descriptor(110), d111 = descriptor(111); std::vector<std::shared_ptr<MockCFDictionary>> seen;
  std::vector<std::shared_ptr<MockCFDictionary>> got;
  AirPlaySetupHook hook(recorder(&seen), [&](const auto &x) { got = x; return true; });
  CHECK(hook.setup(nullptr, request({d110, d111}), nullptr) == 0);
  CHECK(seen.size() == 1 && seen[0].get() == d110.get());
  CHECK(got.size() == 1 && got[0].get() == d111.get());
}
