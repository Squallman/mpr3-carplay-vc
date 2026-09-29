# Offline target boundary

Stage 3 encodes contracts and pass-through behavior. It produces host tests,
an optional static contract library and compile-only objects.
**NOT DEPLOYMENT READY.** No shared hook, runtime bootstrap, installation rule,
device launcher or vehicle connection mechanism is provided.

The canonical header is
[`airplay_setup_abi.hpp`](include/mpr3/target/airplay_setup_abi.hpp).
The legacy `include/mpr3/target_setup_abi.hpp` is an include-only compatibility
shim requiring `target/include` on the consumer's include path. Opaque project
CF types do not claim to reproduce Apple headers. Host requests are never cast
to CF objects.

```sh
./scripts/build-target-contracts.sh
./scripts/check-target-abi.sh
```

With CMake:

```sh
cmake -S . -B build-target-contracts -DMPR3_BUILD_TARGET_CONTRACTS=ON
cmake --build build-target-contracts
ctest --test-dir build-target-contracts --output-on-failure -V
```

`MPR3_BUILD_TARGET_CONTRACTS` and `MPR3_ENABLE_POSIX_TARGET_RESOLVER` default OFF.
POSIX code requires enabling both explicitly. The host fallback accepts
`MPR3_ENABLE_POSIX_TARGET_RESOLVER=1 ./scripts/build-target-contracts.sh`.
Tests use fakes even when POSIX code is compiled; no firmware loader is tested.
There is deliberately no experimental preload-library option or shared output.

The entry object has undefined integration functions for resolver injection
and failure policy. No evidence-backed AirPlay status for absent resolution is
known. These declarations prevent linking a usable hook without additional
work; they do not implement process termination or any error policy. Future
runtime fail-open behavior remains blocked until original forwarding and safe
resolver-failure handling are proven.

See [contracts](../docs/target-contracts.md),
[CF boundary](../docs/corefoundation-target-contract.md), and
[later adapters](../docs/target-adapter-contracts.md).
