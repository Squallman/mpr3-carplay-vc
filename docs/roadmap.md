# Roadmap

## Stage 0 — completed

Static firmware extraction and architecture reverse engineering (Phase 1–6).
Historical evidence remains unchanged.

## Stage 1 — completed

Initial offline mocked prototype preserving stock stream 110.

## Stage 2 — offline architecture completed; research outstanding

Prototype v2 separates request adapters and mocks from core orchestration.
It models displayable ownership, pipeline construction, conditional
VideoEncoding activation, typed status/events, cleanup/retry, and explicit
multiple-secondary policies. See [prototype v2](offline-prototype-v2.md).

Phase 7/8 recover exact SETUP CFLite primitives and ownership, generic displays
dispatch and stock descriptor fields. Parent24/children0/1/2/4 and generic
child17 structural expressibility are recovered, without client-capability proof.
Remaining research: client-valid secondary advertisement and callback ownership,
iOS type-111 trigger, P3695 iAP2 child17 relevance, active endpoint
and service variant, safe displayable occupancy, target lifecycle/teardown,
and loader/COMM policy. Unresolved facts have not become implementation constants.

## Stage 3 — target boundary and CFLite SETUP adapter completed offline

Canonical opaque SETUP ABI, injectable typed original resolver, exact pass-through,
self-resolution rejection, capability metadata and unlinked C export object are
implemented. Host contract tests and installed-tool AArch64 compile/symbol checks
are opt-in; core/default builds require no target libraries. See
[target contracts](target-contracts.md).

The [CFLite SETUP adapter](cf-setup-adapter.md) implements recovered P3695
request primitives, explicit descriptor retains and identity-preserving shallow
copies. Dedicated fake-runtime tests exercise ownership, mutations, allocation
failures and unchanged core filtering. It remains disconnected from the exported
entrypoint, which forwards original requests exactly. Normal host builds still
have no target dependencies; optional POSIX CFLite binding defaults OFF.

[ScreenStream/display-init/COMM adapters](target-adapter-contracts.md) remain
NOT IMPLEMENTED pending exact ABIs/ownership/policy. Output restoration blocks
target activation. Loader behavior, runtime resolver failure policy and target
thread-safety remain UNKNOWN. No shared hook or deployment mechanism is produced.

## Stage 4 — separately authorized future work

Phase 8 established **READY_FOR_CF_ADAPTER**; the dedicated adapter is now
implemented in the offline target layer. Actual process binding, lifetime and
concurrency validation remain future evidence tasks. Advertisement and iAP2
adapters remain blocked. See [Phase 8 readiness](../research/analysis/phase8/cf-adapter-readiness-v2.md).

Client-valid second-display identity/schema and the iOS type111 trigger reach
a bounded static limit. Their next evidence stage needs stock identification,
/info and SETUP observations, active geometry and callback lifetime checks,
with any controlled comparison separately justified and authorized. The
[future evidence plan](../research/analysis/phase8/future-runtime-evidence-plan.md)
is documentation only; unclosed local HMI/property provenance remains UNKNOWN.

Bench/runtime diagnostics for loader/COMM policy, displayable occupancy,
endpoint detection, concurrent pipeline behavior, and output restoration.

## Stage 5 — separately authorized future work

Controlled vehicle proof of concept only after the preceding contracts are
proven. The current repository provides no flashing, installation, vehicle
connection, or deployment tooling.
