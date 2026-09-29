#include "test_support.hpp"
void test_original_failure() {
  using namespace mpr3; using namespace mpr3_test;
  std::vector<std::shared_ptr<MockCFDictionary>> seen; AirPlaySetupHook hook(recorder(&seen, -42), [](const auto &){return true;});
  MockSetupResponse out; CHECK(hook.setup(nullptr, request({descriptor(110)}), &out)==-42); CHECK(out.status==-42);
}
