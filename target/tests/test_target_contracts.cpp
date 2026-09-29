#include "test_support.hpp"
#include "mpr3/runtime_config.hpp"
#include "mpr3/target/target_capabilities.hpp"
#include "mpr3/target_setup_abi.hpp"
#include <cstring>
#include <type_traits>
using namespace target_test;

void test_capabilities(TestRun &test) {
  CHECK(targetCapabilities[0].evidence == TargetContractStatus::StrongEvidence);
  CHECK(targetCapabilities[1].evidence == TargetContractStatus::Plausible);
  unsigned unresolved = 0;
  for (const auto &capability : targetCapabilities) {
    CHECK(capability.runtime == TargetContractStatus::Unknown);
    if (capability.availability == AdapterAvailability::NotImplemented) {
      CHECK(capability.evidence == TargetContractStatus::Unknown);
      ++unresolved;
    }
  }
  CHECK(unresolved == 6);
  CHECK(!canClaimTargetReadyOutput(currentOutputEvidence));
}
void test_restoration_blocker(TestRun &test) {
  OutputReadinessEvidence e{true, true, true, true, true, true, true, true, true};
  CHECK(canClaimTargetReadyOutput(e)); // Hypothetical proven inputs; no activation occurs.
  for (bool OutputReadinessEvidence::*gate : {
      &OutputReadinessEvidence::previousSelectionKnown,
      &OutputReadinessEvidence::restorationProven,
      &OutputReadinessEvidence::nativeNavigationRacePolicyProven,
      &OutputReadinessEvidence::restoreBeforeDestroyProven}) {
    e.*gate = false;
    CHECK(!canClaimTargetReadyOutput(e));
    e.*gate = true;
  }
  CHECK(!canClaimTargetReadyOutput(currentOutputEvidence));
}
void test_no_target_defaults(TestRun &test) {
  mpr3::RuntimeConfig config;
  CHECK(config.displayable.configuredName.empty());
  CHECK(!config.displayable.resolvedId.has_value());
  CHECK(!config.clusterDisplayId.has_value());
  CHECK(!config.validate().validForOffline);
  CHECK(!config.validate().activationInputsKnown);
  static_assert(std::is_same_v<decltype(&AirPlayReceiverSessionSetup), SetupABI>);
  static_assert(sizeof(int) == 4, "Recovered status occupies w0");
  CHECK(std::strcmp(setupSymbolName, "AirPlayReceiverSessionSetup") == 0);
}

#define GROUPS(X) X(resolver_exact) X(resolver_missing) X(resolver_self) X(resolver_repeated) \
  X(bridge_identity) X(bridge_status) X(bridge_unavailable) X(bridge_defensive_self) \
  X(bridge_repeated) X(bridge_null_and_boundary) X(capabilities) X(restoration_blocker) X(no_target_defaults)
#define DECLARE(name) void test_##name(TestRun &);
GROUPS(DECLARE)
#undef DECLARE
int main() {
  TestRun test;
  unsigned groups = 0;
#define RUN(name) test_##name(test); ++groups;
  GROUPS(RUN)
#undef RUN
  std::cout << "mpr3 target contracts: " << groups << " test groups / "
            << test.assertions << " assertions passed\n";
}
