# AI context

**Project:** MPR3 CarPlay Virtual Cockpit

**Target:** Audi MIB3 Premium / MPR3, P3695 research baseline, Linux/AArch64.

**Goal:** preserve stock CarPlay type 110 while adding an independent secondary
navigation video path to a Virtual Cockpit through stock VideoEncoding.

**Current implementation:** host mocked prototype plus opt-in P3695 CFLite
SETUP adapter, validated offline against a dedicated fake runtime.

**Stage 3:** opt-in `target/` contracts with canonical opaque three-argument
SETUP ABI, testable resolver, exact pass-through and an unlinked exported entry
object. Core architecture is unchanged. The real ABI-facing CF adapter implements
Phase 8's READY_FOR_CF_ADAPTER contract, with dynamic streams/type keys,
retaining descriptors and borrowed/owned requests. It is not wired into the
exported entrypoint, which remains pass-through. Service/advertisement/iAP2
adapters and restoration remain NOT IMPLEMENTED; no shared hook is produced.
See `docs/cf-setup-adapter.md`, `docs/target-contracts.md`,
`docs/corefoundation-target-contract.md`, and `docs/target-adapter-contracts.md`.

**Key proven seams:** `AirPlayReceiverSessionSetup`, ScreenStream lifecycle,
`dint_create_displayable`, `IpTeConnection::getDisplayable`,
`setActiveDisplayable`, and `CEncoder::feed`.

**Recovered:** P3695 CFLite helper ABIs, exact SETUP key text, scoped request
ownership, generic displays delegate+0x18 dispatch, stock descriptor fields,
WirelessCarPlay parent24/children0/1/2/4. Child17 is structurally expressible,
with its client relevance unproven.

**Unknown:** iOS type-111 trigger, child17 capability relevance, balanced displays
callback ownership and client-valid second descriptor, runtime symbol binding,
policy/loader authorization, active endpoint, safe displayable,
and target concurrent lifecycle.

**Research:** `research/analysis/`.

**Local firmware:** `.local-research/mpr3/P3695/` is intentionally ignored;
agents must not assume it exists when working from a GitHub checkout.
