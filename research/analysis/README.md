# MPR3 research analysis

This tree contains the chronological Phase 1–6 reverse-engineering evidence
for Audi MIB3 Premium / MPR3 P3695. Reports are preserved as evidence records;
they are not silently rewritten to match later conclusions.

Evidence labels are `PROVEN`, `STRONG EVIDENCE`, `PLAUSIBLE`, `UNKNOWN`, and
`DISPROVEN`. MHI2Q/MHI3 material is comparative only unless independently
proven for MPR3. A later phase supersedes an earlier `UNKNOWN` only when it adds
direct evidence.

| Phase | Primary report | Main question | Most important result |
|---|---|---|---|
| 1 | `p3695-carplay-vc-investigation.md` | What is the P3695 architecture? | Main CarPlay uses displayable 93; AirPlay and cluster paths were mapped initially. |
| 2 | `phase2/stream111-and-cluster-api.md` | Does SETUP handle 111 and how does cluster RPC work? | Stock SETUP rejects 111; `setActiveDisplayable(displayID, displayable)` exists. |
| 3 | `phase3/secondary-screen-architecture.md` | Can screen state be instantiated independently? | 0x340 screen context and ScreenStream state make a second instance architecturally plausible. |
| 4 | `phase4/render-target-and-negotiation.md` | How are render targets and negotiation represented? | Configured displayable creation and negotiation layers were separated; key gates remained unknown. |
| 5 | `phase5/implementation-seams.md` | Can a sidecar/hook preserve stock 110? | Direct encoder path and likely SETUP PLT seam support offline sidecar design. |
| 6 | `phase6/runtime-hook-contract.md` | What are the runtime-hook and advertisement contracts? | Three-argument SETUP ABI and display callback shape were recovered; deployment policy and descriptor details remain unknown. |

The local P3695 data is outside this tracked analysis tree at
`.local-research/mpr3/P3695/` and is intentionally ignored.
