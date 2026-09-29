# MPR3 CarPlay Virtual Cockpit

Research and offline prototype for Audi MIB3 Premium / MPR3 CarPlay secondary
navigation video in the Virtual Cockpit. The target is Linux/AArch64 MPR3, not
the QNX/MHI2Q implementation.

## Goal

Preserve stock CarPlay while investigating an independent secondary navigation
screen path from iPhone to an Audi cluster display.

## Current status

An offline architectural prototype exists. It uses mocked CoreFoundation,
ScreenStream, decoder, displayable, and VideoEncoding services. There is no
vehicle deployment, firmware patch, install package, or claim that P3695
supports working AltScreen today.

## Proven high-level findings

- Stock main CarPlay uses stream type 110.
- Stock P3695 rejects SETUP stream type 111.
- `AirPlayReceiverSessionSetup` has strong static evidence for a three-argument ABI and an internal PLT/JUMP_SLOT seam.
- Screen contexts and ScreenStream factories are structurally instance-oriented.
- `videoencoderservice` directly consumes an IPTE displayable.
- `setActiveDisplayable(displayID, displayable)` exists.
- Displayable creation uses configured LayerConfig names.
- Native navigation uses the display-init/EGL infrastructure.

## Unresolved

The iOS type-111 capability trigger, P3695 parameter-17 behavior, complete
secondary descriptor, AppArmor/loader authorization, active MOST/Ethernet
endpoint, safe production displayable, and target concurrent lifecycle remain
unresolved.

## Prototype

The implementation is at repository root in `include/`, `hook/`, `sidecar/`,
`mocks/`, and `tests/`. It is intentionally host-only and fail-open: secondary
failure must not break stock stream 110.

## Research

See [`research/analysis/README.md`](research/analysis/README.md) for the Phase
1–6 evidence index. The large local firmware workspace is
`.local-research/mpr3/P3695/`; it is intentionally ignored and is not required
to understand the tracked architecture.

## Build

```sh
./scripts/build-local.sh
```

Or, when CMake is available:

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

Direct Apple Clang host compilation was previously validated; no target
AArch64 binary is executed.
