#pragma once
#include "mpr3/airplay_types.hpp"

namespace mpr3::target {
// Opaque project types, not a claim to reproduce Apple headers or object layout.
struct OpaqueCFDictionary;
using CFDictionaryRef = const OpaqueCFDictionary *;
// STRONG EVIDENCE: x0=session, x1=request, x2=responseOut; w0=int status.
using SetupFunction = int(AirPlayReceiverSessionPrivate *session,
                          CFDictionaryRef request, CFDictionaryRef *responseOut);
using SetupABI = SetupFunction *;
inline constexpr char setupSymbolName[] = "AirPlayReceiverSessionSetup";
// Evidence metadata only. Neither value participates in lookup or a call.
inline constexpr char setupEvidenceLibrary[] = "libairplay.so";
inline constexpr char setupEvidenceLocation[] = "0x58dc0";
} // namespace mpr3::target

// One canonical exported declaration, using the same recovered function type.
extern "C" mpr3::target::SetupFunction AirPlayReceiverSessionSetup;
