#include "test_support.hpp"

void test_full_start(mpr3_test::TestRun &test) {
  using namespace mpr3; using namespace mpr3_test;
  Fixture f;
  auto secondary = descriptor(111, 23);
  CHECK(f.controller->start({secondary}));
  CHECK(f.controller->state() == SecondaryState::Running);
  CHECK(f.controller->failure() == SecondaryFailure::None);
  CHECK(f.controller->activationResult() == ActivationResult::Activated);
  CHECK(f.controller->receivedDescriptorCount() == 1);
  CHECK(f.displayables.calls == 1 && f.factory.calls == 1 && f.video.calls == 1);
  CHECK(f.displayables.lastConfig.configuredName == "Displayable_Debug_1");
  CHECK(f.factory.received == DescriptorCollection{secondary});
  CHECK(f.factory.receivedDisplayable.resolvedId == 137);
  CHECK(f.factory.receivedConfig.clusterDisplayId == 9001);
  CHECK(f.video.lastDisplay == 9001 && f.video.lastDisplayable == 137);
  CHECK(f.counters->streamStarts == 1 && f.counters->decoderStarts == 1);
  CHECK(f.counters->displayablesDestroyed == 0);
  f.controller->stop();
  CHECK(f.counters->balanced());
}

void test_displayable_failure(mpr3_test::TestRun &test) {
  using namespace mpr3; using namespace mpr3_test;
  for (bool nullHandle : {false, true}) {
    Fixture f;
    f.displayables.createResult = nullHandle;
    f.displayables.returnNull = nullHandle;
    CHECK(!f.start());
    CHECK(f.controller->failure() == SecondaryFailure::DisplayableCreateFailed);
    CHECK(f.controller->state() == SecondaryState::Failed);
    CHECK(f.factory.calls == 0 && f.video.calls == 0);
    CHECK(f.counters->balanced());
  }
}

void test_factory_failure(mpr3_test::TestRun &test) {
  using namespace mpr3; using namespace mpr3_test;
  for (auto stage : {MockFactoryFailure::StreamCreate, MockFactoryFailure::DecoderCreate,
                     MockFactoryFailure::ContextCreate}) {
    Fixture f;
    f.factory.failure = stage;
    CHECK(!f.start());
    CHECK(f.controller->failure() == SecondaryFailure::PipelineCreateFailed);
    CHECK(f.controller->state() == SecondaryState::Failed);
    CHECK(f.counters->balanced());
    CHECK(f.counters->displayablesCreated == 1 && f.counters->displayablesDestroyed == 1);
    CHECK(f.counters->streamStarts == 0 && f.counters->decoderStarts == 0);
    CHECK(f.video.calls == 0);
    CHECK(f.counters->streamsCreated == (stage == MockFactoryFailure::StreamCreate ? 0 : 1));
    CHECK(f.counters->decodersCreated == (stage == MockFactoryFailure::ContextCreate ? 1 : 0));
  }
}

void test_stream_start_failure(mpr3_test::TestRun &test) {
  using namespace mpr3; using namespace mpr3_test;
  Fixture f; f.factory.streamStartResult = false;
  CHECK(!f.start());
  CHECK(f.controller->failure() == SecondaryFailure::StreamStartFailed);
  CHECK(f.counters->streamStarts == 1 && f.counters->streamStops == 1);
  CHECK(f.counters->decoderStarts == 0 && f.counters->decoderStops == 0);
  CHECK(f.counters->balanced());
  CHECK(f.video.calls == 0);
}

void test_decoder_start_failure(mpr3_test::TestRun &test) {
  using namespace mpr3; using namespace mpr3_test;
  Fixture f; f.factory.decoderStartResult = false;
  CHECK(!f.start());
  CHECK(f.controller->failure() == SecondaryFailure::DecoderStartFailed);
  CHECK(f.counters->streamStarts == 1 && f.counters->streamStops == 1);
  CHECK(f.counters->decoderStarts == 1 && f.counters->decoderStops == 1);
  CHECK(f.counters->balanced());
  CHECK(f.video.calls == 0);
}

void test_video_unavailable(mpr3_test::TestRun &test) {
  using namespace mpr3; using namespace mpr3_test;
  Fixture f; f.video.result = ActivationResult::Unavailable;
  CHECK(!f.start());
  CHECK(f.controller->failure() == SecondaryFailure::VideoEncodingUnavailable);
  CHECK(f.controller->activationResult() == ActivationResult::Unavailable);
  CHECK(f.counters->balanced());
  CHECK(f.counters->streamStops == 1 && f.counters->decoderStops == 1);
}

void test_activation_failure(mpr3_test::TestRun &test) {
  using namespace mpr3; using namespace mpr3_test;
  for (auto result : {ActivationResult::Failed, ActivationResult::InvalidDisplayable,
                     ActivationResult::NotAttempted}) {
    Fixture f; f.video.result = result;
    CHECK(!f.start());
    CHECK(f.controller->failure() == (result == ActivationResult::InvalidDisplayable
        ? SecondaryFailure::InvalidDisplayable : SecondaryFailure::OutputActivationFailed));
    CHECK(f.controller->activationResult() == result);
    CHECK(f.counters->balanced());
    CHECK(f.counters->streamStops == 1 && f.counters->decoderStops == 1);
  }
}

void test_missing_endpoint(mpr3_test::TestRun &test) {
  using namespace mpr3; using namespace mpr3_test;
  auto config = testConfig(); config.clusterDisplayId.reset();
  Fixture f(config);
  CHECK(f.start());
  CHECK(f.controller->state() == SecondaryState::Running);
  CHECK(f.controller->activationResult() == ActivationResult::MissingEndpoint);
  CHECK(f.controller->failure() == SecondaryFailure::None);
  CHECK(f.video.calls == 0);
  CHECK(f.counters->streamStarts == 1 && f.counters->decoderStarts == 1);
  f.controller->stop();
  CHECK(f.counters->balanced());
}

void test_missing_displayable_id(mpr3_test::TestRun &test) {
  using namespace mpr3; using namespace mpr3_test;
  auto config = testConfig(); config.displayable.resolvedId.reset();
  Fixture f(config);
  CHECK(f.start());
  CHECK(f.controller->state() == SecondaryState::Running);
  CHECK(f.controller->activationResult() == ActivationResult::MissingDisplayableId);
  CHECK(f.controller->failure() == SecondaryFailure::None);
  CHECK(f.video.calls == 0);
  f.controller->stop();
  CHECK(f.counters->balanced());
}

void test_config_validation(mpr3_test::TestRun &test) {
  using namespace mpr3; using namespace mpr3_test;
  RuntimeConfig empty;
  CHECK(empty.displayable.configuredName.empty());
  CHECK(!empty.displayable.resolvedId && !empty.clusterDisplayId);
  CHECK(!empty.validate().validForOffline && !empty.validate().activationInputsKnown);
  Fixture noDisplayable(empty);
  CHECK(!noDisplayable.start());
  CHECK(noDisplayable.controller->failure() == SecondaryFailure::MissingConfiguration);
  CHECK(noDisplayable.displayables.calls == 0);
  auto config = testConfig();
  CHECK(config.validate().validForOffline && config.validate().activationInputsKnown);
  config.displayable.resolvedId.reset();
  CHECK(config.validate().validForOffline && !config.validate().activationInputsKnown);
  CHECK(config.validate(42).activationInputsKnown);
  config.clusterDisplayId.reset();
  CHECK(!config.validate(42).activationInputsKnown);
  config.outputPolicy = OutputPolicy::RequireActivationInputs;
  Fixture strict(config);
  CHECK(!strict.start());
  CHECK(strict.controller->failure() == SecondaryFailure::MissingConfiguration);
  CHECK(strict.controller->activationResult() == ActivationResult::MissingEndpoint);
  CHECK(strict.counters->balanced() && strict.factory.calls == 0);
  config.clusterDisplayId = 9001;
  Fixture strictMissingId(config);
  CHECK(!strictMissingId.start());
  CHECK(strictMissingId.controller->activationResult() == ActivationResult::MissingDisplayableId);
  CHECK(strictMissingId.counters->balanced() && strictMissingId.video.calls == 0);
  // Numeric values are opaque: no invented enum/range interpretation.
  config.displayable.resolvedId = 0;
  config.clusterDisplayId = -9007;
  Fixture arbitrary(config);
  CHECK(arbitrary.start());
  CHECK(arbitrary.video.lastDisplay == -9007 && arbitrary.video.lastDisplayable == 0);
}

void test_resolved_handle(mpr3_test::TestRun &test) {
  using namespace mpr3; using namespace mpr3_test;
  auto config = testConfig(); config.displayable.resolvedId.reset();
  Fixture f(config);
  f.displayables.overrideId = true; f.displayables.resolvedId = 234;
  CHECK(f.start());
  CHECK(f.video.lastDisplayable == 234);
  CHECK(f.factory.receivedDisplayable.resolvedId == 234);
  for (bool wrongName : {false, true}) {
    Fixture bad;
    if (wrongName) bad.displayables.returnedName = "another-test-name";
    else { bad.displayables.overrideId = true; bad.displayables.resolvedId = 234; }
    CHECK(!bad.start());
    CHECK(bad.controller->failure() == SecondaryFailure::InvalidDisplayable);
    CHECK(bad.counters->balanced() && bad.factory.calls == 0 && bad.video.calls == 0);
  }
}

void test_multi_policy(mpr3_test::TestRun &test) {
  using namespace mpr3; using namespace mpr3_test;
  auto a = descriptor(111, 1), b = descriptor(111, 2);
  Fixture reject;
  CHECK(!reject.controller->start({a, b}));
  CHECK(reject.controller->receivedDescriptorCount() == 2);
  CHECK(reject.controller->failure() == SecondaryFailure::MultipleDescriptorsRejected);
  CHECK(reject.displayables.calls == 0);
  auto config = testConfig(); config.multiSecondaryPolicy = MultiSecondaryPolicy::FirstOnly;
  Fixture first(config);
  CHECK(first.controller->start({a, b}));
  CHECK(first.controller->receivedDescriptorCount() == 2);
  CHECK(first.factory.received == DescriptorCollection{a});
  // SETUP always sends all 111 descriptors to the controller; policy is separate.
  Fixture hooked;
  auto main = descriptor(110);
  CHECK(hooked.setup(request({a, main, b})) == 0);
  CHECK(hooked.controller->receivedDescriptorCount() == 2);
  CHECK(hooked.stock.seenDescriptors == DescriptorCollection{main});
  CHECK(hooked.controller->failure() == SecondaryFailure::MultipleDescriptorsRejected);
}

void test_controller_invalid_request(mpr3_test::TestRun &test) {
  using namespace mpr3; using namespace mpr3_test;
  Fixture f;
  for (const auto &descriptors : {DescriptorCollection{}, DescriptorCollection{nullptr},
                                DescriptorCollection{descriptor(110)}}) {
    CHECK(!f.controller->start(descriptors));
    CHECK(f.controller->failure() == SecondaryFailure::InvalidRequest);
    CHECK(f.counters->balanced() && f.displayables.calls == 0);
  }
  CHECK(f.start());
}

void test_secondary_exceptions(mpr3_test::TestRun &test) {
  using namespace mpr3; using namespace mpr3_test;
  for (int stage = 0; stage < 5; ++stage) {
    Fixture f;
    switch (stage) {
      case 0: f.displayables.throwOnCreate = true; break;
      case 1: f.factory.throwOnCreate = true; break;
      case 2: f.factory.throwOnStreamStart = true; break;
      case 3: f.factory.throwOnDecoderStart = true; break;
      case 4: f.video.throwOnActivate = true; break;
    }
    CHECK(!f.start());
    CHECK(f.controller->failure() == SecondaryFailure::UnexpectedException);
    CHECK(f.counters->balanced());
    CHECK(f.counters->streamStops == (stage >= 2 ? 1 : 0));
    CHECK(f.counters->decoderStops == (stage >= 3 ? 1 : 0));
    if (stage == 4) CHECK(f.controller->activationResult() == ActivationResult::Failed);
  }
}

void test_descriptor_lifetime(mpr3_test::TestRun &test) {
  using namespace mpr3; using namespace mpr3_test;
  auto config = testConfig(); config.multiSecondaryPolicy = MultiSecondaryPolicy::FirstOnly;
  Fixture f(config);
  std::weak_ptr<const ISetupDescriptor> first, second;
  {
    auto a = descriptor(111), b = descriptor(111);
    first = a; second = b;
    CHECK(f.controller->start({a, b}));
    f.factory.received.clear(); // Remove the mock factory's diagnostic retention.
  }
  CHECK(!first.expired() && !second.expired());
  f.controller->stop();
  CHECK(first.expired() && second.expired());
}

void test_capability_placeholders(mpr3_test::TestRun &test) {
  using namespace mpr3;
  MockSecondaryDisplayAdvertisement advertisement;
  MockIap2SecondaryCapability iap2;
  CHECK(advertisement.support() == CapabilitySupport::Unknown);
  CHECK(iap2.support() == CapabilitySupport::Unknown);
  advertisement.value = CapabilitySupport::Unsupported;
  CHECK(advertisement.support() == CapabilitySupport::Unsupported);
}

void test_pipeline_raii(mpr3_test::TestRun &test) {
  using namespace mpr3; using namespace mpr3_test;
  auto counters = std::make_shared<MockLifecycle>();
  {
    SecondarySessionOwner owner(std::make_unique<MockScreenStream>(counters, true, false),
        std::make_unique<MockDecoder>(counters, true, false),
        std::make_unique<MockRenderContext>(counters));
    CHECK(owner.start() == SecondaryFailure::None);
    CHECK(owner.start() == SecondaryFailure::InvalidTransition);
    CHECK(counters->streamStops == 0);
  }
  CHECK(counters->balanced());
  CHECK(counters->streamStops == 1 && counters->decoderStops == 1);
  {
    SecondarySessionOwner incomplete(std::make_unique<MockScreenStream>(counters, true, false),
        nullptr, std::make_unique<MockRenderContext>(counters));
    CHECK(incomplete.start() == SecondaryFailure::PipelineCreateFailed);
    incomplete.stop(); incomplete.stop();
  }
  CHECK(counters->balanced());
}

void test_alternative_request_adapter(mpr3_test::TestRun &test) {
  using namespace mpr3;
  // Exercise the core with an adapter that has no MockCF objects at all.
  struct Payload { std::int64_t type; std::string opaque; };
  struct DescriptorView final : ISetupDescriptor {
    std::shared_ptr<const Payload> payload;
    explicit DescriptorView(std::shared_ptr<const Payload> p) : payload(std::move(p)) {}
    std::optional<std::int64_t> type() const override { return payload->type; }
    const void *identity() const noexcept override { return payload.get(); }
  };
  struct RequestView final : ISetupRequest {
    DescriptorCollection values;
    std::shared_ptr<const std::string> opaque;
    std::optional<DescriptorCollection> streams() const override { return values; }
    std::unique_ptr<ISetupRequest> withStreams(DescriptorCollection next) const override {
      auto copy = std::make_unique<RequestView>(*this);
      copy->values = std::move(next);
      return copy;
    }
  } requestView;
  auto mainPayload = std::make_shared<Payload>(Payload{110, "opaque-main-fields"});
  auto main = std::make_shared<DescriptorView>(mainPayload);
  auto secondary = std::make_shared<DescriptorView>(std::make_shared<Payload>(Payload{111, "opaque"}));
  requestView.values = {main, secondary};
  requestView.opaque = std::make_shared<const std::string>("opaque-request");
  auto filtered = splitSecondaryDescriptors(requestView);
  CHECK(!filtered.parseFailed && filtered.stockRequest);
  CHECK(filtered.stockRequest->streams().value() == DescriptorCollection{main});
  CHECK(filtered.stockRequest->streams()->front()->identity() == mainPayload.get());
  CHECK(filtered.secondaryDescriptors == DescriptorCollection{secondary});
  const auto *copy = dynamic_cast<const RequestView *>(filtered.stockRequest.get());
  CHECK(copy && copy->opaque == requestView.opaque);
  CHECK(mainPayload->opaque == "opaque-main-fields");
}
