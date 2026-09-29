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

Bench/runtime diagnostics for loader/COMM policy, displayable occupancy,
endpoint detection, concurrent pipeline behavior, and output restoration.

## Stage 5 — separately authorized future work

Controlled vehicle proof of concept only after the preceding contracts are
proven. The current repository provides no flashing, installation, vehicle
connection, or deployment tooling.
