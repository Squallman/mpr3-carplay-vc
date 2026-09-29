#include "test_support.hpp"
void test_original_failure(mpr3_test::TestRun &test) {
  using namespace mpr3; using namespace mpr3_test;
  for (int result : {0, -42, 123}) {
    for (bool secondarySucceeds : {true, false}) {
      MockAirPlay stock; stock.result = result;
      MockSecondaryController secondary; secondary.result = secondarySucceeds;
      AirPlaySetupHook hook(stock.handler(), secondary);
      SetupResponse out;
      auto r = request({descriptor(110), descriptor(111)});
      CHECK(hook.setup(nullptr, r, &out) == result);
      CHECK(out.status == result);
      CHECK(stock.seenResponse == &out && stock.seenSession == nullptr);
      CHECK(stock.calls == 1 && secondary.calls == 1);
    }
  }
}
