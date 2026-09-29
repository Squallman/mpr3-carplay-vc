#include "test_support.hpp"
void test_secondary_failure_fail_open() {
  using namespace mpr3; using namespace mpr3_test;
  std::vector<std::shared_ptr<MockCFDictionary>> seen;
  AirPlaySetupHook hook(recorder(&seen), [](const auto &) { return false; });
  CHECK(hook.setup(nullptr, request({descriptor(110), descriptor(111)}), nullptr)==0);
  CHECK(seen.size()==1 && seen[0]->integers.at("type")==110);

  auto stream = std::make_unique<MockScreenStream>();
  auto decoder = std::make_unique<MockDecoder>(); decoder->startResult = false;
  SecondarySessionOwner decoderFailure(std::move(stream), std::move(decoder), std::make_unique<MockDisplayable>());
  CHECK(!decoderFailure.start());
  CHECK(decoderFailure.state() == SecondaryState::Failed);

  MockVideoEncoding rpc; rpc.result = Result::Unavailable;
  CHECK(rpc.setActiveDisplayable(4, 137) == Result::Unavailable);
  // The stock handler above has already succeeded; RPC failure is secondary-only.
  CHECK(seen.size() == 1 && seen[0]->integers.at("type") == 110);
}
