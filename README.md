# MPR3 CarPlay Virtual Cockpit

Research and offline prototype for Audi MIB3 Premium / MPR3 CarPlay secondary
navigation video in the Virtual Cockpit. The target is Linux/AArch64 MPR3, not
the QNX/MHI2Q implementation.

## Goal

Preserve stock CarPlay while investigating an independent secondary navigation
screen path from iPhone to an Audi cluster display.

## Current status

Prototype v2 models the complete proposed control flow on the host: SETUP
filtering, stock forwarding, a secondary controller, configured displayable
acquisition, pipeline construction/startup, optional VideoEncoding activation,
and deterministic cleanup. Its normal host suite uses mock request adapters
and device services.
There is no vehicle deployment, firmware patch, install package, or claim that
P3695 supports working AltScreen today.

Stage 3 adds an opt-in target contract layer: a canonical opaque AirPlay ABI,
testable original resolver, exact pass-through bridge, capability/blocker metadata
and an exported compile-only entry object. A separate P3695 CFLite SETUP adapter
now implements the recovered request ABI and ownership contract, validated
offline with a dedicated fake runtime. It is not connected to that entrypoint,
which remains pass-through; device-service adapters remain unimplemented.
See [CFLite adapter](docs/cf-setup-adapter.md), [target contracts](docs/target-contracts.md) and
[target boundary](target/README.md).

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

The iOS type-111 capability trigger, client relevance of P3695 child17, complete
secondary descriptor, AppArmor/loader authorization, active MOST/Ethernet
endpoint, safe production displayable, and target concurrent lifecycle remain
unresolved.

## Prototype

The implementation is at repository root in `include/`, `hook/`, `sidecar/`,
`mocks/`, and `tests/`. `mpr3_core` contains abstract request filtering and
secondary orchestration; `mpr3_mocks` contains host implementations;
`mpr3_tests` links both. Core hook interfaces contain no MockCF types and are
not CoreFoundation ABI-compatible.

Stock SETUP completes first and its exact result/response are preserved.
Non-111 descriptors retain their original identity and opaque fields. A
secondary failure cleans only secondary resources. Runtime configuration has
no default displayable name or numeric target IDs. Missing activation IDs can
permit an offline pipeline, with output explicitly marked skipped.

See [`docs/offline-prototype-v2.md`](docs/offline-prototype-v2.md) for lifecycle,
policies, contracts, test coverage, and target adapter boundaries.

## Research

See [`research/analysis/README.md`](research/analysis/README.md) for the Phase
1–8 evidence index. The large local firmware workspace is
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

If CMake is unavailable, the script uses `${CXX:-clang++}` directly with C++17
and `-Wall -Wextra -Wpedantic -Werror`. Both build paths run the same test suite
and print its test-group/assertion totals. No target AArch64 binary is executed.

Optional offline target checks (contracts and both POSIX resolver options default OFF):

```sh
./scripts/build-target-contracts.sh
./scripts/check-target-abi.sh
```

The first also runs dedicated CFLite ownership/filtering tests. The second
compiles/inspects dependency-free SETUP and CFLite AArch64/Linux objects with
installed tools only. No loadable hook or deployment workflow is produced.
