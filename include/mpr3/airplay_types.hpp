#pragma once
namespace mpr3 {
struct AirPlayReceiverSessionPrivate;
// Host response model, never cast to a target CFDictionaryRef output.
struct SetupResponse { int status = 0; };
} // namespace mpr3
