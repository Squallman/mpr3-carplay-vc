#include "test_support.hpp"
void test_multiple_descriptors() {
  using namespace mpr3; using namespace mpr3_test;
  auto a=descriptor(100), b=descriptor(111), c=descriptor(101), d=descriptor(110), e=descriptor(111);
  std::vector<std::shared_ptr<MockCFDictionary>> seen, got;
  AirPlaySetupHook hook(recorder(&seen), [&](const auto &x) { got=x; return true; });
  CHECK(hook.setup(nullptr, request({a,b,c,d,e}), nullptr) == 0);
  CHECK(seen.size()==3 && seen[0].get()==a.get() && seen[1].get()==c.get() && seen[2].get()==d.get());
  CHECK(got.size()==2 && got[0].get()==b.get() && got[1].get()==e.get());
}
