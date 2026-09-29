#include "test_support.hpp"
void test_secondary_failure_fail_open(mpr3_test::TestRun &test) {
  using namespace mpr3; using namespace mpr3_test;
  // Every orchestration failure is exercised through the real SETUP hook.
  for (int stockResult : {0, -42}) {
    for (int failure = 0; failure < 14; ++failure) {
      auto config = testConfig();
      if (failure == 10) config.displayable.configuredName.clear();
      if (failure == 11) { config.clusterDisplayId.reset(); config.outputPolicy = OutputPolicy::RequireActivationInputs; }
      Fixture f(config);
      f.stock.result = stockResult;
      switch (failure) {
        case 0: f.displayables.createResult = false; break;
        case 1: f.factory.failure = MockFactoryFailure::StreamCreate; break;
        case 2: f.factory.failure = MockFactoryFailure::DecoderCreate; break;
        case 3: f.factory.failure = MockFactoryFailure::ContextCreate; break;
        case 4: f.factory.streamStartResult = false; break;
        case 5: f.factory.decoderStartResult = false; break;
        case 6: f.video.result = ActivationResult::Unavailable; break;
        case 7: f.video.result = ActivationResult::InvalidDisplayable; break;
        case 8: f.video.result = ActivationResult::Failed; break;
        case 9: f.video.throwOnActivate = true; break;
        case 12: f.displayables.throwOnCreate = true; break;
        case 13: f.factory.throwOnDecoderStart = true; break;
        default: break;
      }
      auto main = descriptor(110, 77), secondary = descriptor(111);
      auto r = request({main, secondary});
      SetupResponse out;
      CHECK(f.setup(r, &out) == stockResult);
      CHECK(out.status == stockResult);
      CHECK(f.stock.calls == 1);
      CHECK(f.stock.seenDescriptors == DescriptorCollection{main});
      CHECK(main->integers.at("unknown_marker") == 77);
      CHECK(main->strings.at("opaque_field") == "preserve-me");
      CHECK((r.streams().value() == DescriptorCollection{main, secondary}));
      CHECK(f.controller->state() == SecondaryState::Failed);
      CHECK(f.counters->balanced());
      const auto kinds = f.events.kinds();
      CHECK(kinds[2] == EventKind::StockSetupForwarded);
      CHECK(kinds.back() == EventKind::SecondaryFailed);
    }
  }
}
