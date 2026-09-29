# Later target adapters and restoration blocker

These are documented boundaries, all **NOT IMPLEMENTED**. Core interfaces retain
prototype v2 semantics; host ownership policies do not prove target lifecycle.

## Secondary pipeline: ScreenStream, decoder, render context

Core expects `ISecondaryPipelineFactory::create(descriptors, displayable, config)`
to return an owned `SecondarySessionOwner` or failure. It starts stream then
decoder, stops decoder then stream and owns the render context. Partial failure
must unwind only secondary resources. Descriptors and displayable outlive it.

**PROVEN:** a separately allocated stock screen context owns socket/stream state.
**STRONG EVIDENCE:** instance-oriented stream/decoder machinery. Sources:
[Phase 5 seams](../research/analysis/phase5/implementation-seams.md) and
[Phase 6 contract](../research/analysis/phase6/runtime-hook-contract.md).
A working second target instance is not established.

**UNKNOWN:** ScreenStream constructor, decoder factory, context layouts,
callbacks, transport setup, descriptor fields, render binding, thread affinity
and stop/join order. The adapter must own only its secondary socket, stream,
decoder, callbacks and render/EGL resources; callbacks must quiesce before
destruction. Exact ABIs, independent construction, binding, partial failure and
concurrent teardown require proof before implementation. Stock 110 remains
untouched; no descriptor fields or endpoint are supplied here.

## Displayable provider: libdisplayinit, LayerConfig, IPTE surface

Core expects `IDisplayableProvider::createByConfiguredName` to return a lease
preserving the configured name and optional ID. It rejects name/ID conflicts.
The host destroys the pipeline before the lease. No safe name/ID is defaulted.

**STRONG EVIDENCE:** stock nav uses creation, surface acquisition, EGL/context,
render/swap and destruction; LayerConfig resolves configured names. Source:
[stock client API](../research/analysis/phase5/stock-display-client-api.md).
**UNKNOWN:** display-init C argument layout, errors/output ownership, graphics
initialization ABI, IPTE surface leases, occupancy and safe concurrency. No slot
is proven safe: [occupancy evidence](../research/analysis/phase6/displayable-occupancy-analysis.md).

The provider must distinguish owned/borrowed resources and release only its own,
after renderer and encoder are detached. Exact create/surface/destroy ABIs,
configured name resolution, safe occupancy, authorization, thread/lifetime rules,
partial cleanup and restore-before-destroy behavior must be proven. The host
lease destruction order alone cannot guarantee target safety.

## VideoEncoding: asi.VideoEncoding.IVideoEncoding / COMM

Core expects semantic `IVideoEncodingClient::setActiveDisplayable(displayId,
displayableId)` returning `ActivationResult`. This is not a generated target
C++ proxy ABI. Mock activation establishes no target readiness.

**PROVEN:** the service resolves an integer IPTE displayable, checks dimensions
and feeds it directly to the encoder. **STRONG EVIDENCE:** vesdt constructs a
COMM/EMS client and dispatches two integers. Sources:
[vesdt call chain](../research/analysis/phase5/vesdt-set-displayable-callchain.md)
and [Phase 5 seams](../research/analysis/phase5/implementation-seams.md).

**UNKNOWN:** generated proxy construction/destruction/layout, registration,
discovery, active method/service variant, reply/error mapping, callback lifetime,
authorization and active MOST/Ethernet endpoint. Static enums do not select a
runtime family: [endpoint evidence](../research/analysis/phase6/cluster-endpoint-selector.md),
[policy matrix](../research/analysis/phase6/runtime-policy-matrix.md).
The adapter must own its agent/registration/proxy/callbacks and finish in-flight
work before release. Exact ABIs, policy, selection and restoration must be
proven; no broker address, vtable layout or endpoint is invented.

## Output restoration blocks target activation

Before any target output activation implementation, establish:

| Required fact | Evidence | Implementation blocker |
|---|---|---|
| Previously active displayable and owner | UNKNOWN | No known restoration destination |
| Query API or authoritative previous-selection source | UNKNOWN | Cannot capture prior selection safely |
| Safe restoration using setActiveDisplayable, including failures | UNKNOWN | Selection alone does not prove restoration |
| Native-navigation/secondary race arbitration | UNKNOWN | Stale restoration could overwrite newer native selection |
| Restore/detach completion before displayable destruction | UNKNOWN | Encoder must not retain destroyed input |
| Cleanup on partial activation, stop, disconnect and exit | UNKNOWN | Every outcome must preserve normal CarPlay |

No query API is assumed to exist. No deactivation, rollback or restoration is
implemented. `OutputReadinessEvidence` records required gates; all current gates
are unset. Tests reject readiness whenever any restoration prerequisite is
absent, even with every other prerequisite hypothetically proven. The predicate
is metadata only, not activation or a runtime readiness detector. Selection
ownership, acknowledgement and destruction order require direct evidence.

## Advertisement and iAP2 placeholders

Existing core capability interfaces remain unchanged. Advertisement delegate
slot, complete second descriptor, P3695 parameter-17/ThemeAssets and causal iOS
type-111 trigger remain **UNKNOWN**. No infoRequestDisplays hook, delegate patch
or capability emission is implemented. MHI2Q/MHI3 findings remain comparative.
