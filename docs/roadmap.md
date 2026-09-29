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

## Stage 3 — future

Evidence-backed target adapters and cross-build only; validate exports,
CoreFoundation ownership, and linking without vehicle deployment. The optional
host resolver model remains disabled by default. No target adapters are
implemented in v2.

## Stage 4 — separately authorized future work

Bench/runtime diagnostics for loader/COMM policy, displayable occupancy,
endpoint detection, concurrent pipeline behavior, and output restoration.

## Stage 5 — separately authorized future work

Controlled vehicle proof of concept only after the preceding contracts are
proven. The current repository provides no flashing, installation, vehicle
connection, or deployment tooling.
