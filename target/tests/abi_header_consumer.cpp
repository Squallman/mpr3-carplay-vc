// Dependency-free compile-only consumer: no standard library or Apple headers.
#include "mpr3/target/airplay_setup_abi.hpp"
static_assert(sizeof(int) == 4, "AirPlay status must occupy w0");
static_assert(sizeof(void *) == 8, "The inspected target ABI is 64-bit");
mpr3::target::SetupABI targetHeaderConsumer() {
  return &AirPlayReceiverSessionSetup;
}
