# AI context

**Project:** MPR3 CarPlay Virtual Cockpit

**Target:** Audi MIB3 Premium / MPR3, P3695 research baseline, Linux/AArch64.

**Goal:** preserve stock CarPlay type 110 while adding an independent secondary
navigation video path to a Virtual Cockpit through stock VideoEncoding.

**Current implementation:** host-only mocked prototype at repository root.

**Stage 3:** opt-in `target/` contracts with canonical opaque three-argument
SETUP ABI, testable resolver, exact pass-through and an unlinked exported entry
object. Core architecture is unchanged. CF/service adapters and restoration
remain NOT IMPLEMENTED; no shared hook is produced. See `docs/target-contracts.md`,
`docs/corefoundation-target-contract.md`, and `docs/target-adapter-contracts.md`.

**Key proven seams:** `AirPlayReceiverSessionSetup`, ScreenStream lifecycle,
`dint_create_displayable`, `IpTeConnection::getDisplayable`,
`setActiveDisplayable`, and `CEncoder::feed`.

**Unknown:** iOS type-111 trigger, parameter 17, advertisement delegate slot
and descriptor, policy/loader authorization, active endpoint, safe displayable,
and target concurrent lifecycle.

**Research:** `research/analysis/`.

**Local firmware:** `.local-research/mpr3/P3695/` is intentionally ignored;
agents must not assume it exists when working from a GitHub checkout.
