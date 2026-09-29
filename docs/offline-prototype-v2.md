# Offline prototype v2

## Purpose and scope

Model the proposed MPR3 control flow in small C++17 host components while
preserving stock SETUP behavior. This is an offline implementation architecture,
not a firmware patch or a working vehicle implementation. No stock ELF or P3695
artifact is modified. No target defaults, deployment tools, or real
CoreFoundation/ScreenStream/display-init/COMM bindings are introduced.

## Modules and ownership

| Module | Responsibilities |
|---|---|
| `mpr3_core` | Request filtering, hook, secondary controller, pipeline owner, configuration validation, abstract service/capability/event contracts |
| `mpr3_mocks` | MockCF request/descriptor adapters, original AirPlay handler, stream, decoder, render context, displayable provider/lease, staged pipeline factory, VideoEncoding, capability status probes |
| `mpr3_tests` | Identity/opaque-field checks, fail-open results, lifecycle/failure injection, ordering and resource accounting |

The CMake dependency graph is `tests -> mocks -> core`, with tests also directly
linking core. Core never includes `mocks.hpp` or compiles a `mocks/` source.
The mock header lives under `mocks/include/`, exported only by `mpr3_mocks`.
`MPR3_ENABLE_TARGET_LOADER` defaults OFF; host builds need no target library.
The existing optional `dlsym(RTLD_NEXT)` resolver is an isolated host model,
not target loader validation.

Ownership is explicit: descriptors are `shared_ptr<const ISetupDescriptor>`;
filtered requests, pipelines, and displayable leases are `unique_ptr` objects.
Injected providers/factories/clients/event sinks are borrowed and must outlive
their consumers. There is no global service state or raw owning pointer.

## Abstract SETUP contract

`ISetupDescriptor::type()` returns an optional 64-bit integer. It cannot
accidentally narrow an unfamiliar large value into 111. `identity()` is a
non-owning token for the underlying retained object; a future adapter can have
an identity different from its C++ wrapper address. Unknown descriptor fields
are deliberately unavailable to core filtering.

`ISetupRequest::streams()` provides retained descriptor references, and
`withStreams()` creates a replacement request preserving opaque request fields.
An adapter must shallowly reuse the descriptors, rather than reconstructing
stock dictionaries. Missing/malformed streams, null descriptors, or descriptors
without a readable type invalidate the whole filter transaction. An exception
or null result while copying also invalidates it: forward the original request
with no partially extracted secondary collection. Missing unknown *fields* do
not invalidate a descriptor with a readable type.

The flow is:

1. Emit `SetupReceived` and filter only descriptors whose type is exactly 111.
2. No 111: forward the original request object. With 111: forward the shallow
   request copy and original non-111 descriptor identities in original order.
   Invalid input/copy: forward the original request unchanged.
3. Invoke the original handler once with the supplied session and response.
4. Emit `StockSetupForwarded` after it returns.
5. Notify `ISecondaryController` with the entire ordered 111 collection, if
   extraction succeeded. Secondary receives neither stock response nor stock
   descriptor collection.
6. Return the exact original integer result, including nonzero failures.

Secondary startup is independent of whether the stock result is zero; its
outcome never replaces that result. Exceptions originating in the stock handler
retain the stock handler's host exception behavior; they are not secondary
errors and do not cause a second stock invocation.

The evidence-backed target declaration is documented separately:

```c
int AirPlayReceiverSessionSetup(
    AirPlayReceiverSessionPrivate *,
    CFDictionaryRef,
    CFDictionaryRef *);
```

`target_setup_abi.hpp` uses opaque target-only types for this shape. The host
`ISetupRequest` and `SetupResponse` are not ABI-compatible with CoreFoundation;
no mock pointer aliases are used as target CF references. Exact target retain
counts and error-path ownership remain UNKNOWN.

## Secondary lifecycle

```text
Idle / cleaned Failed
  -> DescriptorReceived
  -> Preparing: configuration, displayable lease, pipeline factory
  -> Starting: stream, decoder, optional activation
  -> Running
  -> Stopping: pipeline stop/destruction, displayable destruction, references release
  -> Idle

Any accepted attempt failure -> the same cleanup -> Failed
```

The controller exposes state, typed failure, activation result, and received
descriptor count. Failure diagnostics persist across stop and reset at the next
accepted attempt. A running start attempt is rejected with an
`SecondaryAttemptRejected/InvalidTransition` event without replacing the active
state, resources, or original failure diagnostics.

The factory receives selected opaque descriptors, the acquired displayable,
and runtime configuration. It owns RAII cleanup during partial construction.
The mock factory can fail at stream, decoder, or render-context creation. The
pipeline owner starts the stream and then decoder. Attempted starts are stopped
even if they return failure or throw; never-started components are destroyed
without a stop call. Adapters must release construction resources in their
destructors and support `noexcept` cleanup after partial start.

Cleanup stops decoder then stream, destroys decoder/stream/render context, then
destroys the displayable lease and releases retained descriptors. Failure is
reported after cleanup. `stop()` is idempotent and destruction stops an active
pipeline. A new independent attempt can start from Idle or cleaned Failed.

The model is synchronous and expects serialized calls; it does not implement
target thread/transport scheduling. Queries from observers are supported.
Reentrant starts are rejected and stops during an in-progress operation are
ignored, preventing observers from interrupting partial ownership transitions.
Observers must not recursively react to their own rejection events or destroy
the component while an operation is in progress.

## Configuration, displayable and output

`RuntimeConfig` contains a configured displayable name, optional numeric
displayable ID, optional cluster display ID, and explicit multi-secondary/output
policies. Its default has no configured displayable or numeric values. There
are no `-1` unknown sentinels or inferred target enum/range values.

`validate()` distinguishes `validForOffline` (nonempty configured name) from
`activationInputsKnown` (name plus both IDs). A provider may resolve an unknown
ID when acquiring the displayable. If supplied metadata and an acquired ID
disagree, or the provider returns another configured name, startup fails with
`InvalidDisplayable`. An explicitly configured known ID can supply the value
when the provider's returned handle has no ID.

| Inputs / outcome | Offline behavior |
|---|---|
| Missing displayable name | Fail with `MissingConfiguration` before creation |
| Name known, endpoint missing | Start pipeline; skip activation with `MissingEndpoint` |
| Name/endpoint known, displayable ID unresolved | Start pipeline; skip activation with `MissingDisplayableId` |
| Both IDs known | Invoke injected `setActiveDisplayable(displayID, displayableID)` |
| Unavailable client | Clean secondary resources; `VideoEncodingUnavailable` |
| Invalid displayable result | Clean secondary resources; `InvalidDisplayable` |
| Other unsuccessful activation | Clean secondary resources; `OutputActivationFailed` |
| Adapter throws | Clean secondary resources; `UnexpectedException` |

`AllowOfflinePipeline` is the default output policy. `RequireActivationInputs`
rejects missing activation inputs after displayable acquisition, before pipeline
construction. Even an `Activated` mock result proves only the modeled call:
numeric sufficiency does not establish target readiness, safe occupancy,
authorization, the active endpoint family, or successful physical output.

`Displayable_Debug_1`, displayable ID 137, and endpoint 9001 occur only as
explicit HOST TEST DATA. Other arbitrary test IDs are also exercised. No
production name, safe ID, DisplayID enum, MOST/Ethernet selection, or secondary
descriptor field is encoded in core.

Output rollback/deactivation is not modeled: the evidence establishes selection
but not a safe secondary restore protocol or previous-selection ownership.
Before implementing a target adapter, acquire evidence for restoring/detaching
output before destroying a selected displayable. Current cleanup guarantees are
host resource guarantees and do not prove target concurrent lifecycle safety.

## Multiple-secondary policy

Filtering always retains every 111 descriptor in original order. The controller
receives all of them. Its current factory models one active secondary screen:
`MultiSecondaryPolicy::Reject` rejects multiple descriptors before resource
creation; `FirstOnly` retains the whole ordered collection for the lifecycle
but supplies only the first descriptor to the pipeline factory. Both are
configurable PROTOTYPE policies, not claims about P3695 semantics.

## Advertisement and iAP2 placeholders

`ISecondaryDisplayAdvertisement` and `IIap2SecondaryCapability` are independent
status-probe extension points, with mock implementations defaulting to Unknown.
They are not startup prerequisites and neither core hook nor controller depends
on their implementation. Future negotiation/bootstrap adapters can use these
interfaces without embedding wire behavior in SETUP orchestration. Actual
advertisement payload construction and capability emission remain unsupported;
the status probes deliberately specify no payload schema or wire operation.

No complete second AirPlay descriptor, exact delegate slot, P3695 parameter-17
behavior, or causal iOS type-111 trigger has been proven. Historical MHI3/MHI2Q
findings are not transferred to MPR3.

## Observability and tests

`IEventSink` accepts small typed events containing a kind, optional failure,
and descriptor count; it receives no raw binary or descriptor fields.
Observer exceptions are contained. Successful SETUP startup emits:

```text
SetupReceived -> SecondaryDescriptorsExtracted -> StockSetupForwarded
-> SecondaryPreparing -> DisplayableCreated -> PipelineStarted
-> OutputActivated (or OutputActivationSkipped) -> SecondaryRunning
-> SecondaryStopped on stop/destruction
```

Failures emit `SecondaryFailed` after partial resources are cleaned. Filter
errors emit `SetupFilterFailed` before forwarding stock. Active/reentrant start
rejection emits `SecondaryAttemptRejected`. A stock-forwarding event precedes
every secondary outcome for every stock call that returns.

The suite retains the original seven behavior groups and expands to 34 groups.
It covers stock/request/descriptor identity and opaque fields, unfamiliar types
below/above 111 and wide integers, multiple 111 ordering, missing/malformed
input and copying, exact stock success/failure responses, all creation/start/
activation failure stages, unknown IDs, configuration validation, idempotence,
destructor and partial cleanup order, retries, retained descriptor lifetime,
alternative non-MockCF adapters, events/states, observer exceptions, and
reentrant attempts. The executable prints the exact assertion total each run.

## Build and future target boundary

```sh
./scripts/build-local.sh
# When CMake is installed:
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

Both paths use C++17 and warnings as errors on supported Clang/GNU compilers.
The script preserves a direct host Clang fallback when CMake is unavailable.
Build outputs are ignored. No target binary is executed.

Host validation completed with Apple Clang: 34 groups / 687 assertions passed,
including an AddressSanitizer/UndefinedBehaviorSanitizer build. Core also builds
independently without the mock include path and contains no Mock symbols.
CMake/CTest execution was unavailable on this host; its library separation is
defined in `CMakeLists.txt`, and the fallback exercised the full suite.

Useful future adapters, documented rather than empty skeleton files:

| Boundary | Required evidence before implementation |
|---|---|
| CoreFoundation `ISetupRequest` / descriptor adapter | Retain/release and safe shallow-copy/error ownership; preserve original dictionaries |
| Original AirPlay resolver/wrapper | Actual loader scope/versioning and policy; retain the documented three-argument ABI |
| ScreenStream/decoder/render factory | Exact lifecycle/construction contracts, independent instances, concurrency and render target |
| display-init provider / lease | Exact create/destroy ABI, LayerConfig resolution, occupancy and safe ownership |
| COMM/EMS VideoEncoding client | Proxy/service variant, policy, active endpoint, call outcomes and output restoration |
| Advertisement / iAP2 bootstrap | Complete secondary descriptor, delegate slot, P3695 capability behavior and iOS trigger |

Phase 5/6 reports under `research/analysis/` remain the evidence source. The
host RAII design, policy choices, result enums and events are prototype design
decisions; they are not asserted to be recovered firmware contracts.
