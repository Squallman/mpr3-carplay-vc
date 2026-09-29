#pragma once
#include "mpr3/airplay_types.hpp"
namespace mpr3::target {
// Evidence-only declaration: Phase 6 airplay-session-setup-abi.md.
// No host request/descriptor implements the CoreFoundation ABI.
struct OpaqueCFDictionary;
using CFDictionaryRef = const OpaqueCFDictionary *;
using SetupABI = int (*)(AirPlayReceiverSessionPrivate *, CFDictionaryRef, CFDictionaryRef *);
SetupABI resolveOriginalSetup();
} // namespace mpr3::target
