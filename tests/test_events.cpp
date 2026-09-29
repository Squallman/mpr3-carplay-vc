#include "test_support.hpp"

void test_success_event_order(mpr3_test::TestRun &test) {
  using namespace mpr3; using namespace mpr3_test;
  Fixture f;
  CHECK(f.setup(request({descriptor(110), descriptor(111)})) == 0);
  CHECK((f.events.kinds() == std::vector<EventKind>{EventKind::SetupReceived,
      EventKind::SecondaryDescriptorsExtracted, EventKind::StockSetupForwarded,
      EventKind::SecondaryPreparing, EventKind::DisplayableCreated,
      EventKind::PipelineStarted, EventKind::OutputActivated, EventKind::SecondaryRunning}));
  CHECK(f.events.events[1].descriptorCount == 1);
  f.controller->stop();
  CHECK(f.events.events.back().kind == EventKind::SecondaryStopped);
}

void test_failure_event_order(mpr3_test::TestRun &test) {
  using namespace mpr3; using namespace mpr3_test;
  Fixture f; f.factory.decoderStartResult = false;
  CHECK(f.setup(request({descriptor(110), descriptor(111)})) == 0);
  CHECK((f.events.kinds() == std::vector<EventKind>{EventKind::SetupReceived,
      EventKind::SecondaryDescriptorsExtracted, EventKind::StockSetupForwarded,
      EventKind::SecondaryPreparing, EventKind::DisplayableCreated, EventKind::SecondaryFailed}));
  CHECK(f.events.events.back().failure == SecondaryFailure::DecoderStartFailed);
  CHECK(f.counters->balanced());
  auto config = testConfig(); config.clusterDisplayId.reset();
  Fixture offline(config);
  CHECK(offline.setup(request({descriptor(110), descriptor(111)})) == 0);
  CHECK((offline.events.kinds() == std::vector<EventKind>{EventKind::SetupReceived,
      EventKind::SecondaryDescriptorsExtracted, EventKind::StockSetupForwarded,
      EventKind::SecondaryPreparing, EventKind::DisplayableCreated,
      EventKind::PipelineStarted, EventKind::OutputActivationSkipped, EventKind::SecondaryRunning}));
}

void test_observer_failure(mpr3_test::TestRun &test) {
  using namespace mpr3; using namespace mpr3_test;
  Fixture f; f.events.throwOnEvent = true;
  CHECK(f.setup(request({descriptor(110), descriptor(111)})) == 0);
  CHECK(f.stock.calls == 1 && f.controller->state() == SecondaryState::Running);
  f.controller->stop();
  CHECK(f.counters->balanced());
  CHECK(f.events.events.empty());
}

void test_state_observation(mpr3_test::TestRun &test) {
  using namespace mpr3; using namespace mpr3_test;
  struct Observer final : IEventSink {
    SecondaryController *controller = nullptr; // Borrowed, never owning.
    MockLifecycle *counters = nullptr;
    std::vector<SecondaryState> states;
    bool cleanedAtFailure = false;
    void onEvent(const Event &event) override {
      states.push_back(controller->state());
      if (event.kind == EventKind::SecondaryFailed) cleanedAtFailure = counters->balanced();
    }
  } observer;
  Fixture f;
  SecondaryController controller(testConfig(), f.displayables, f.factory, f.video, &observer);
  observer.controller = &controller; observer.counters = f.counters.get();
  CHECK(controller.start({descriptor(111)}));
  CHECK((observer.states == std::vector<SecondaryState>{SecondaryState::Preparing,
      SecondaryState::Preparing, SecondaryState::Starting, SecondaryState::Starting,
      SecondaryState::Running}));
  controller.stop();
  CHECK(observer.states.back() == SecondaryState::Idle);
  f.video.result = ActivationResult::Failed;
  CHECK(!controller.start({descriptor(111)}));
  CHECK(observer.cleanedAtFailure);
  CHECK(observer.states.back() == SecondaryState::Failed);
}

void test_stock_completed_first(mpr3_test::TestRun &test) {
  using namespace mpr3; using namespace mpr3_test;
  Fixture f;
  bool stockCompleted = false;
  struct Observer final : IEventSink {
    const bool &stockCompleted;
    bool secondaryObservedAfterStock = true;
    explicit Observer(const bool &completed) : stockCompleted(completed) {}
    void onEvent(const Event &event) override {
      if (event.kind == EventKind::SecondaryPreparing)
        secondaryObservedAfterStock = stockCompleted;
    }
  } observer(stockCompleted);
  SecondaryController controller(testConfig(), f.displayables, f.factory, f.video, &observer);
  AirPlaySetupHook hook([&](auto *, const ISetupRequest &, SetupResponse *response) {
    stockCompleted = true;
    if (response) response->status = -42;
    return -42;
  }, controller, &observer);
  SetupResponse response;
  CHECK(hook.setup(nullptr, request({descriptor(110), descriptor(111)}), &response) == -42);
  CHECK(response.status == -42 && observer.secondaryObservedAfterStock);
  CHECK(controller.state() == SecondaryState::Running);
}

void test_observer_reentry(mpr3_test::TestRun &test) {
  using namespace mpr3; using namespace mpr3_test;
  struct Observer final : IEventSink {
    SecondaryController *controller = nullptr;
    bool rejected = false;
    void onEvent(const Event &event) override {
      if (event.kind == EventKind::SecondaryPreparing) {
        controller->stop(); // In-progress operations cannot be interrupted.
        rejected = !controller->start({descriptor(111)});
      }
    }
  } observer;
  Fixture f;
  SecondaryController controller(testConfig(), f.displayables, f.factory, f.video, &observer);
  observer.controller = &controller;
  CHECK(controller.start({descriptor(111)}));
  CHECK(observer.rejected && controller.state() == SecondaryState::Running);
  controller.stop();
  CHECK(f.counters->balanced());
}
