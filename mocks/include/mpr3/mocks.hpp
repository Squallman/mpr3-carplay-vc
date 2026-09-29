#pragma once

// Host test data and adapters only. Exported by mpr3_mocks, not mpr3_core.
#include "mpr3/secondary_capabilities.hpp"
#include "mpr3/secondary_controller.hpp"
#include "mpr3/setup_filter.hpp"
#include <string>
#include <unordered_map>
#include <utility>

namespace mpr3 {

struct MockCFDictionary final : ISetupDescriptor {
  std::unordered_map<std::string, std::int64_t> integers;
  std::unordered_map<std::string, std::string> strings;
  bool throwOnType = false;
  std::optional<std::int64_t> type() const override;
  const void *identity() const noexcept override { return this; }
};
struct MockCFArray { DescriptorCollection values; };
struct MockCFRequest final : ISetupRequest {
  std::unordered_map<std::string, std::shared_ptr<MockCFArray>> arrays;
  std::unordered_map<std::string, std::string> opaqueFields;
  bool throwOnRead = false;
  bool throwOnCopy = false;
  bool failCopy = false;
  std::optional<DescriptorCollection> streams() const override;
  std::unique_ptr<ISetupRequest> withStreams(DescriptorCollection) const override;
};

struct MockAirPlay {
  int result = 0;
  int calls = 0;
  const ISetupRequest *seenRequest = nullptr; // Identity only; may expire after SETUP.
  AirPlayReceiverSessionPrivate *seenSession = nullptr;
  SetupResponse *seenResponse = nullptr;
  DescriptorCollection seenDescriptors;
  std::unordered_map<std::string, std::string> seenOpaqueFields;
  int setup(AirPlayReceiverSessionPrivate *, const ISetupRequest &, SetupResponse *);
  OriginalSetupFn handler();
};
struct MockSecondaryController final : ISecondaryController {
  bool result = true;
  int calls = 0;
  DescriptorCollection received;
  bool start(const DescriptorCollection &) noexcept override;
};
struct MockEventSink final : IEventSink {
  std::vector<Event> events;
  bool throwOnEvent = false;
  void onEvent(const Event &) override;
  std::vector<EventKind> kinds() const;
};

// Shared counters survive owned resource destruction for cleanup assertions.
// cleanupOrder uses fixed-size storage so noexcept teardown cannot allocate.
enum class CleanupStep { DecoderStop, StreamStop, DecoderDestroy, StreamDestroy,
                         ContextDestroy, DisplayableDestroy };
struct MockLifecycle {
  int displayablesCreated = 0, displayablesDestroyed = 0;
  int streamsCreated = 0, streamsDestroyed = 0, streamStarts = 0, streamStops = 0;
  int decodersCreated = 0, decodersDestroyed = 0, decoderStarts = 0, decoderStops = 0;
  int contextsCreated = 0, contextsDestroyed = 0;
  CleanupStep cleanupOrder[128]{};
  std::size_t cleanupCount = 0;
  void record(CleanupStep step) noexcept;
  bool balanced() const noexcept;
};
class MockScreenStream final : public IScreenStream {
 public:
  MockScreenStream(std::shared_ptr<MockLifecycle>, bool startResult, bool throwOnStart);
  ~MockScreenStream() override;
  bool start() override;
  void stop() noexcept override;
 private:
  std::shared_ptr<MockLifecycle> lifecycle_;
  bool startResult_, throwOnStart_;
};
class MockDecoder final : public IDecoder {
 public:
  MockDecoder(std::shared_ptr<MockLifecycle>, bool startResult, bool throwOnStart);
  ~MockDecoder() override;
  bool start() override;
  void stop() noexcept override;
 private:
  std::shared_ptr<MockLifecycle> lifecycle_;
  bool startResult_, throwOnStart_;
};
class MockRenderContext final : public IRenderContext {
 public:
  explicit MockRenderContext(std::shared_ptr<MockLifecycle>);
  ~MockRenderContext() override;
 private:
  std::shared_ptr<MockLifecycle> lifecycle_;
};
class MockDisplayable final : public IDisplayable {
 public:
  MockDisplayable(DisplayableHandle, std::shared_ptr<MockLifecycle>);
  ~MockDisplayable() override;
  const DisplayableHandle &handle() const noexcept override { return handle_; }
 private:
  DisplayableHandle handle_;
  std::shared_ptr<MockLifecycle> lifecycle_;
};
struct MockDisplayableProvider final : IDisplayableProvider {
  explicit MockDisplayableProvider(std::shared_ptr<MockLifecycle> counters)
      : lifecycle(std::move(counters)) {}
  std::shared_ptr<MockLifecycle> lifecycle;
  bool createResult = true, throwOnCreate = false, returnNull = false;
  bool overrideId = false;
  std::optional<int> resolvedId;
  std::optional<std::string> returnedName;
  int calls = 0;
  DisplayableHandle lastConfig;
  DisplayableCreateResult createByConfiguredName(const DisplayableHandle &) override;
};
enum class MockFactoryFailure { None, StreamCreate, DecoderCreate, ContextCreate };
struct MockPipelineFactory final : ISecondaryPipelineFactory {
  explicit MockPipelineFactory(std::shared_ptr<MockLifecycle> counters)
      : lifecycle(std::move(counters)) {}
  std::shared_ptr<MockLifecycle> lifecycle;
  MockFactoryFailure failure = MockFactoryFailure::None;
  bool streamStartResult = true, decoderStartResult = true;
  bool throwOnCreate = false, throwOnStreamStart = false, throwOnDecoderStart = false;
  int calls = 0;
  DescriptorCollection received;
  DisplayableHandle receivedDisplayable;
  RuntimeConfig receivedConfig;
  PipelineCreateResult create(const DescriptorCollection &, const IDisplayable &,
                              const RuntimeConfig &) override;
};
struct MockVideoEncoding final : IVideoEncodingClient {
  ActivationResult result = ActivationResult::Activated;
  bool throwOnActivate = false;
  int calls = 0;
  std::optional<int> lastDisplay, lastDisplayable;
  ActivationResult setActiveDisplayable(int, int) override;
};
struct MockSecondaryDisplayAdvertisement final : ISecondaryDisplayAdvertisement {
  CapabilitySupport value = CapabilitySupport::Unknown;
  CapabilitySupport support() const noexcept override { return value; }
};
struct MockIap2SecondaryCapability final : IIap2SecondaryCapability {
  CapabilitySupport value = CapabilitySupport::Unknown;
  CapabilitySupport support() const noexcept override { return value; }
};

} // namespace mpr3
