#include "test_support.hpp"
void test_state_lifecycle(mpr3_test::TestRun &test) {
  using namespace mpr3; using namespace mpr3_test;
  Fixture f;
  CHECK(f.controller->state() == SecondaryState::Idle);
  f.controller->stop();
  CHECK(f.events.events.empty());
  CHECK(f.start());
  CHECK(f.controller->state() == SecondaryState::Running);
  CHECK(!f.start()); // Reject while active; retain the existing resources.
  CHECK(f.controller->state() == SecondaryState::Running);
  CHECK(f.events.events.back().failure == SecondaryFailure::InvalidTransition);
  CHECK(f.counters->displayablesDestroyed == 0 && f.counters->streamStops == 0);
  f.controller->stop();
  CHECK(f.controller->state() == SecondaryState::Idle);
  CHECK(f.counters->balanced());
  CHECK(f.counters->streamStarts == 1 && f.counters->streamStops == 1);
  CHECK(f.counters->decoderStarts == 1 && f.counters->decoderStops == 1);
  auto eventCount = f.events.events.size();
  f.controller->stop();
  CHECK(f.events.events.size() == eventCount);
  CHECK(f.counters->streamStops == 1 && f.counters->decoderStops == 1);
  CHECK(f.start());
  f.controller->stop();
  CHECK(f.counters->balanced());
  CHECK(f.counters->streamStarts == 2 && f.counters->streamStops == 2);
}
void test_destructor_cleanup(mpr3_test::TestRun &test) {
  using namespace mpr3; using namespace mpr3_test;
  Fixture f;
  CHECK(f.start());
  f.controller.reset();
  CHECK(f.counters->balanced());
  CHECK(f.counters->cleanupCount == 6);
  const CleanupStep expected[] = {CleanupStep::DecoderStop, CleanupStep::StreamStop,
      CleanupStep::DecoderDestroy, CleanupStep::StreamDestroy,
      CleanupStep::ContextDestroy, CleanupStep::DisplayableDestroy};
  for (std::size_t i = 0; i < 6; ++i) CHECK(f.counters->cleanupOrder[i] == expected[i]);
  CHECK(f.events.events.back().kind == EventKind::SecondaryStopped);
}
void test_retry_after_failure(mpr3_test::TestRun &test) {
  using namespace mpr3; using namespace mpr3_test;
  Fixture f;
  f.factory.decoderStartResult = false;
  CHECK(!f.start());
  CHECK(f.counters->balanced());
  CHECK(f.counters->streamStops == 1 && f.counters->decoderStops == 1);
  f.factory.decoderStartResult = true;
  CHECK(f.start()); // Retry directly from Failed, after automatic cleanup.
  CHECK(f.controller->failure() == SecondaryFailure::None);
  CHECK(f.controller->state() == SecondaryState::Running);
  f.controller->stop();
  CHECK(f.counters->balanced());
  CHECK(f.counters->streamStarts == 2 && f.counters->streamStops == 2);
  f.video.result = ActivationResult::Failed;
  CHECK(!f.start());
  f.controller->stop();
  const auto count = f.events.events.size();
  f.controller->stop();
  CHECK(count == f.events.events.size());
  CHECK(f.controller->state() == SecondaryState::Idle);
}
