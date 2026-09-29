#include "mpr3/target/entry_integration_contract.hpp"

// Compile-only object. Not linked into core, tests, or a loadable library.
extern "C" int AirPlayReceiverSessionSetup(
    mpr3::AirPlayReceiverSessionPrivate *session,
    mpr3::target::CFDictionaryRef request,
    mpr3::target::CFDictionaryRef *responseOut) {
  using namespace mpr3::target;
  const auto result = passThroughSetup(entryOriginalSetupResolver(),
      &AirPlayReceiverSessionSetup, session, request, responseOut);
  if (const int *status = result.originalStatus()) return *status;
  // Deliberately unresolved integration policy, never an invented AirPlay code.
  entryResolverUnavailable(result.resolution());
}
