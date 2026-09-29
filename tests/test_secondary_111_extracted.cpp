#include "test_support.hpp"
void test_secondary_111_extracted(mpr3_test::TestRun &test) {
  using namespace mpr3; using namespace mpr3_test;
  auto main = descriptor(110), secondaryDescriptor = descriptor(111);
  auto r = request({main, secondaryDescriptor});
  auto originalArray = r.arrays.at("streams");
  auto opaqueArray = std::make_shared<MockCFArray>();
  r.arrays["unknown_array"] = opaqueArray;
  auto filtered = splitSecondaryDescriptors(r);
  CHECK(!filtered.parseFailed);
  CHECK(filtered.stockRequest);
  CHECK(filtered.stockRequest->streams().value() == DescriptorCollection{main});
  CHECK(filtered.secondaryDescriptors == DescriptorCollection{secondaryDescriptor});
  const auto *copy = dynamic_cast<const MockCFRequest *>(filtered.stockRequest.get());
  CHECK(copy && copy->opaqueFields == r.opaqueFields);
  CHECK(copy->arrays.at("unknown_array") == opaqueArray);
  CHECK(r.arrays.at("streams") == originalArray);
  CHECK((r.streams().value() == DescriptorCollection{main, secondaryDescriptor}));
  MockAirPlay stock;
  MockSecondaryController secondary;
  AirPlaySetupHook hook(stock.handler(), secondary);
  CHECK(hook.setup(nullptr, r, nullptr) == 0);
  CHECK(stock.calls == 1 && secondary.calls == 1);
  CHECK(stock.seenDescriptors == DescriptorCollection{main});
  CHECK(secondary.received == DescriptorCollection{secondaryDescriptor});
  CHECK(stock.seenOpaqueFields == r.opaqueFields);
  // A request containing only 111 produces an empty stock streams collection.
  CHECK(hook.setup(nullptr, request({secondaryDescriptor}), nullptr) == 0);
  CHECK(stock.seenDescriptors.empty());
}
