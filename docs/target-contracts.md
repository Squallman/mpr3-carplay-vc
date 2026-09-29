# Stage 3: target contracts and compile-only boundary

This is OFFLINE work. Prototype v2 remains the core architecture. The current
target layer does not parse dictionaries, intercept 111, advertise a display or
start secondary resources. Every request, including stock type 110, is forwarded
unchanged when resolution succeeds.

## Canonical ABI

[`airplay_setup_abi.hpp`](../target/include/mpr3/target/airplay_setup_abi.hpp)
defines the function type, pointer and exported declaration together:

```cpp
using SetupFunction = int(AirPlayReceiverSessionPrivate *session,
                          CFDictionaryRef request, CFDictionaryRef *responseOut);
using SetupABI = SetupFunction *;
extern "C" mpr3::target::SetupFunction AirPlayReceiverSessionSetup;
```

The session is the project's opaque declaration in `mpr3`; `CFDictionaryRef`
points to a const opaque project dictionary in `mpr3::target`. Neither declares
object layout or claims Apple header compatibility. `ISetupRequest` and host
`SetupResponse` remain separate. The legacy header is an include-only shim.

**STRONG EVIDENCE:** x0=session, x1=request, x2=responseOut; w0=integer status.
`libairplay.so`, `AirPlayReceiverSessionSetup @ 0x58dc0` are evidence metadata,
never address-based lookup logic. Sources:
[Phase 6 ABI](../research/analysis/phase6/airplay-session-setup-abi.md) and
[Phase 5 ELF seam](../research/analysis/phase5/elf-interposition-matrix.txt).

## Original resolution and reentrancy

`IOriginalSetupResolver::resolve(wrapper)` returns typed `SetupResolution`:
`Resolved`, `MissingSymbol` or `SelfReference`. `OriginalSetupResolver` borrows
an `ISetupSymbolLookup`, performs exactly one lookup per call and validates
null/self results. Failures carry no function pointer. The bridge revalidates
a claimed success before calling it. Tests supply both lookup and resolver fakes.
Passing the actual wrapper address is mandatory; the entry passes its own
address directly.

There is no cache, lazy initialization, mutable global resolver, fallback
address, firmware path or dlopen. Repeated calls resolve independently, including
recovery after absence. Injected dependencies must outlive consumers. Uncached
resolution avoids a shared initialization protocol. Concurrent safety still
depends on injected lookup, loader and stock function. Repeated serial calls
and exact self rejection are tested; **target thread-safety remains UNKNOWN**.
Indirect cycles involving other wrappers and loader-induced reentry require
runtime proof; self rejection does not establish all interposition chains safe.

`PosixSetupSymbolLookup` is available only with explicit
`MPR3_ENABLE_POSIX_TARGET_RESOLVER=ON`. It isolates the POSIX conversion from
`dlsym(RTLD_NEXT, setupSymbolName)` to `SetupABI`. Core never calls dlsym or links
libdl. The former core-bound `MPR3_ENABLE_TARGET_LOADER` option is replaced and
rejected when enabled. **PLAUSIBLE:** RTLD_NEXT strategy; **UNKNOWN:** target
scope, symbol versioning and policy. Sources:
[loader contract](../research/analysis/phase6/preload-loader-contract.md),
[launch evidence](../research/analysis/phase6/dio-manager-launch-contract.md) and
[policy matrix](../research/analysis/phase6/runtime-policy-matrix.md).

## Pass-through and unlinked entry

`passThroughSetup` calls the resolved original exactly once with identical
session, request and responseOut pointers. `ResolvedAndCalled` exposes the exact
integer status through `originalStatus()`. The bridge never reads dictionaries,
changes the output slot, retains/releases objects or interacts with a controller.
Only stock may write the output. The original's host exception behavior is
preserved; no target exception policy is invented.

Failure returns `ResolverUnavailable`, its resolution reason and a null
`originalStatus()` accessor. It calls no original and leaves the output untouched.
Private integer storage is inaccessible as a stock status on failure. Result
enums are offline design choices, not recovered AirPlay status values.

`mpr3_target_entry_object` exports the C symbol and delegates to the bridge.
It requires an intentionally undefined resolver provider and, on unavailability,
an intentionally undefined non-returning integration-policy function.
**This is a missing integration contract, not an implemented termination or
failure policy.** No AirPlay error code is guessed. Runtime integration remains
blocked until failure handling can preserve normal CarPlay. The entry object
is excluded from core and test runtime and is never linked into a shared hook.

## Build and verification

`./scripts/build-local.sh` retains the core/mocks/tests host behavior with no
target include, lookup or libdl requirements. Both new options default OFF.
`./scripts/build-target-contracts.sh` opts into additional host tests and unlinked
entry/header objects. CMake additionally produces a STATIC contract library,
independent of core even if `BUILD_SHARED_LIBS` is ON. Target tests link core
only to verify unchanged default configuration. No shared-library or install
target exists.

`./scripts/check-target-abi.sh` uses installed cross tools to compile the
dependency-free entry and isolated header consumer for `aarch64-linux-gnu`.
It verifies ELF64/ET_REL/EM_AARCH64 and a unique GLOBAL FUNC DEFAULT unmangled
symbol. No sysroot or extracted library is required. `file` and available
nm/readelf report the object; a portable Python ELF reader validates it even
when ELF-aware symbol tools are absent. Tool capability absence is a clean SKIP;
source/symbol failures fail the check. Its dedicated ignored directory permits
only named objects/logs and rejects unexpected linked or packaging artifacts.
Objects are never executed.

Validation on the Stage 3 host (Apple Clang 21): existing host suite **34 groups /
687 assertions**, additional target suite **13 groups / 105 assertions**, both
also passing AddressSanitizer/UndefinedBehaviorSanitizer. Opt-in POSIX code
compiles on the host; tests still use fakes. CMake is unavailable here, so the
direct host build paths were exercised; the CMake path has not been run.
Clang cross-compilation succeeded without a sysroot. `file` reports
`ELF 64-bit LSB relocatable, ARM aarch64, version 1 (SYSV), not stripped`;
host `nm` reports `0000000000000000 T AirPlayReceiverSessionSetup`.
Portable ELF inspection confirms GLOBAL FUNC DEFAULT, with runtime resolver,
bridge and failure-policy references intentionally unresolved in the entry object.

Tests cover resolution/null/self rejection, exact pointers/status/call counts,
repeat calls, null argument neutrality, capability metadata, default IDs/name
and restoration gates. A separate symbol dependency check rejects controller,
CF, display and COMM references from pass-through objects/contracts, providing
a structural test of zero SecondaryController interaction.

## Capability metadata and evidence blockers

Evidence and implementation availability are separate axes. The classifications
are **PROVEN**, **STRONG EVIDENCE**, **PLAUSIBLE**, **UNKNOWN**, **DISPROVEN**.
NOT IMPLEMENTED is an availability label, not an evidence classification.
Metadata is never used in the pass-through hot path.

| Contract/adapter | Exact contract evidence | Runtime | Availability |
|---|---|---|---|
| AirPlay SETUP ABI | STRONG EVIDENCE | UNKNOWN | Contract and unlinked entry |
| RTLD_NEXT resolver | PLAUSIBLE | UNKNOWN | Explicit POSIX build only |
| CoreFoundation adapter | UNKNOWN | UNKNOWN | NOT IMPLEMENTED |
| ScreenStream adapter | UNKNOWN | UNKNOWN | NOT IMPLEMENTED |
| display-init adapter | UNKNOWN | UNKNOWN | NOT IMPLEMENTED |
| VideoEncoding COMM adapter | UNKNOWN | UNKNOWN | NOT IMPLEMENTED |
| advertisement adapter | UNKNOWN | UNKNOWN | NOT IMPLEMENTED |
| iAP2 capability adapter | UNKNOWN | UNKNOWN | NOT IMPLEMENTED |

UNKNOWN exact adapter ABIs do not erase evidence for underlying service seams.
`currentOutputEvidence` has every gate unset. Even hypothetical proof of ABI,
endpoint, ownership and authorization cannot satisfy `canClaimTargetReadyOutput`
without previous selection, restoration, native-navigation race policy and
restoration-before-destruction evidence. This is introspection only; it neither
detects runtime readiness nor activates/restores output.

Delegate slot, full secondary descriptor, P3695 parameter-17/ThemeAssets, iOS
type-111 trigger, CF key/helper ABIs, target service ABIs, endpoint, occupancy,
authorization, concurrent lifecycle and restoration remain **UNKNOWN**.
Comparative MHI2Q/MHI3 findings are never promoted to MPR3 constants.
