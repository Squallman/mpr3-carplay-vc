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

Remaining research: complete secondary advertisement and callback/delegate
slot, iOS type-111 trigger, P3695 iAP2 parameter-17 behavior, active endpoint
and service variant, safe displayable occupancy, target lifecycle/teardown,
and loader/COMM policy. None has been promoted from UNKNOWN to a constant.

## Stage 3 — target boundary completed offline; adapters blocked

Canonical opaque SETUP ABI, injectable typed original resolver, exact pass-through,
self-resolution rejection, capability metadata and unlinked C export object are
implemented. Host contract tests and installed-tool AArch64 compile/symbol checks
are opt-in; core/default builds require no target libraries. See
[target contracts](target-contracts.md).

Real [CF manipulation](corefoundation-target-contract.md) and
[ScreenStream/display-init/COMM adapters](target-adapter-contracts.md) remain
NOT IMPLEMENTED pending exact ABIs/ownership/policy. Output restoration blocks
target activation. Loader behavior, runtime resolver failure policy and target
thread-safety remain UNKNOWN. No shared hook or deployment mechanism is produced.

## Stage 4 — separately authorized future work

Phase 8 establishes **READY_FOR_CF_ADAPTER** for a dedicated input-adapter
implementation branch using the recovered stock SETUP lifetime and scoped
cleanup contract. No CF implementation was added by that research. Advertisement
and iAP2 adapters remain blocked. See [Phase 8 readiness](../research/analysis/phase8/cf-adapter-readiness-v2.md).

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
