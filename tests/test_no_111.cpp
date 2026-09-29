#include "test_support.hpp"
void test_no_111(mpr3_test::TestRun &test) {
  using namespace mpr3; using namespace mpr3_test;
  MockAirPlay stock;
  MockSecondaryController secondary;
  MockEventSink events;
  AirPlaySetupHook hook(stock.handler(), secondary, &events);
  for (auto r : {request({descriptor(110)}), request({descriptor(99), descriptor(112)}), request({})}) {
    CHECK(hook.setup(nullptr, r, nullptr) == 0);
    CHECK(stock.seenRequest == &r);
    CHECK(secondary.calls == 0);
    auto filtered = splitSecondaryDescriptors(r);
    CHECK(!filtered.stockRequest && !filtered.parseFailed);
  }
  CHECK(stock.calls == 3);
  CHECK((events.kinds() == std::vector<EventKind>{
      EventKind::SetupReceived, EventKind::StockSetupForwarded,
      EventKind::SetupReceived, EventKind::StockSetupForwarded,
      EventKind::SetupReceived, EventKind::StockSetupForwarded}));
}
void test_invalid_requests(mpr3_test::TestRun &test) {
  using namespace mpr3; using namespace mpr3_test;
  MockAirPlay stock;
  MockSecondaryController secondary;
  MockEventSink events;
  AirPlaySetupHook hook(stock.handler(), secondary, &events);
  auto invalid = std::make_shared<MockCFDictionary>();
  auto throwing = descriptor(110); throwing->throwOnType = true;
  MockCFRequest missing;
  MockCFRequest nullStreams; nullStreams.arrays["streams"] = nullptr;
  auto readFailure = request({descriptor(111)}); readFailure.throwOnRead = true;
  auto copyFailure = request({descriptor(110), descriptor(111)}); copyFailure.failCopy = true;
  auto copyException = request({descriptor(110), descriptor(111)}); copyException.throwOnCopy = true;
  for (const auto &r : {missing, nullStreams, request({descriptor(111), nullptr}),
       request({descriptor(111), invalid}), request({throwing}), readFailure, copyFailure, copyException}) {
    auto filter = splitSecondaryDescriptors(r);
    CHECK(filter.parseFailed && !filter.stockRequest && filter.secondaryDescriptors.empty());
    CHECK(hook.setup(nullptr, r, nullptr) == 0);
    CHECK(stock.seenRequest == &r);
    CHECK(secondary.calls == 0);
  }
  CHECK(stock.calls == 8);
  CHECK(events.events.size() == 24);
  for (std::size_t i = 0; i < events.events.size(); i += 3) {
    CHECK(events.events[i].kind == EventKind::SetupReceived);
    CHECK(events.events[i + 1].kind == EventKind::SetupFilterFailed);
    CHECK(events.events[i + 2].kind == EventKind::StockSetupForwarded);
  }
}
