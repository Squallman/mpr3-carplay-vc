#include "test_support.hpp"
void test_no_111() {
  using namespace mpr3; using namespace mpr3_test;
  auto d110=descriptor(110); std::vector<std::shared_ptr<MockCFDictionary>> seen; bool started=false;
  AirPlaySetupHook hook(recorder(&seen), [&](const auto &) { started=true; return true; });
  CHECK(hook.setup(nullptr, request({d110}), nullptr)==0); CHECK(!started);
  CHECK(seen.size()==1 && seen[0].get()==d110.get());
}
