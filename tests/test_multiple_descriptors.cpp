#include "test_support.hpp"
void test_multiple_descriptors(mpr3_test::TestRun &test) {
  using namespace mpr3; using namespace mpr3_test;
  auto a = descriptor(100), b = descriptor(111), c = descriptor(101);
  auto d = descriptor(110), e = descriptor(111), low = descriptor(99);
  auto high = descriptor(112), wide = descriptor((std::int64_t{1} << 32) + 111);
  auto r = request({a, b, c, d, e, low, high, wide});
  MockAirPlay stock;
  MockSecondaryController secondary;
  AirPlaySetupHook hook(stock.handler(), secondary);
  CHECK(hook.setup(nullptr, r, nullptr) == 0);
  CHECK((stock.seenDescriptors == DescriptorCollection{a, c, d, low, high, wide}));
  CHECK((secondary.received == DescriptorCollection{b, e}));
  CHECK((r.streams().value() == DescriptorCollection{a, b, c, d, e, low, high, wide}));
  auto full = request({a, c, d, b});
  CHECK(hook.setup(nullptr, full, nullptr) == 0);
  CHECK((stock.seenDescriptors == DescriptorCollection{a, c, d}));
}
