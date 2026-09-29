#include "test_support.hpp"
void test_stock_110_preserved(mpr3_test::TestRun &test) {
  using namespace mpr3; using namespace mpr3_test;
  auto d110 = descriptor(110, 7);
  auto r = request({d110});
  auto originalArray = r.arrays.at("streams");
  MockAirPlay stock;
  MockSecondaryController secondary;
  AirPlaySetupHook hook(stock.handler(), secondary);
  CHECK(hook.setup(nullptr, r, nullptr) == 0);
  CHECK(stock.seenRequest == &r);
  CHECK(stock.seenDescriptors == DescriptorCollection{d110});
  CHECK(stock.seenDescriptors[0]->identity() == d110.get());
  CHECK(d110->integers.at("unknown_marker") == 7);
  CHECK(d110->strings.at("opaque_field") == "preserve-me");
  CHECK(r.arrays.at("streams") == originalArray);
  CHECK(secondary.calls == 0);
}
