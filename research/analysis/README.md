# MPR3 research analysis

This tree contains the chronological Phase 1–8 reverse-engineering evidence
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
| 7 | [phase7/phase7-input-contracts.md](phase7/phase7-input-contracts.md) | What input ABI, ownership, advertisement and iAP2 gates can be closed statically? | Exact CF keys/primitives and generic displays slot recovered; Wireless parent24/children0,1,2,4 distinguished from MHI3; full adapter ownership/client gates remain open. |
| 8 | [phase8/phase8-ownership-secondary-display.md](phase8/phase8-ownership-secondary-display.md) | Can request ownership and second-display/client blockers be closed without implementation? | Stock SETUP CF input contract ready; display helper and reference accounting recovered; client-valid second descriptor/type111 trigger reach a bounded static limit. |

Phase 7 supporting reports:

- [CF helper ABI](phase7/cf-helper-abi.md), [SETUP keys](phase7/setup-key-map.md), [shallow copy](phase7/cf-shallow-copy-contract.md), [response ownership](phase7/setup-response-ownership.md).
- [Delegate layout](phase7/airplay-server-delegate-layout.md), [displays callback](phase7/info-displays-callback-abi.md), [complete stock helper fields](phase7/stock-display-descriptor-complete.md), [/info path](phase7/airplay-info-response-path.md), [features](phase7/airplay-feature-advertisement.md).
- [WirelessCarPlay serializer](phase7/p3695-wireless-carplay-component.md), [child17](phase7/p3695-param17-capability.md), [MHI3 differential](phase7/mhi3-p3695-capability-diff.md), [capability state](phase7/capability-state-bridge.md), [type111 chain](phase7/ios-type111-trigger-chain-v2.md).
- Independent readiness: [CF](phase7/cf-adapter-readiness.md), [advertisement](phase7/advertisement-adapter-readiness.md), [iAP2](phase7/iap2-adapter-readiness.md).

Phase 7 corrects the earlier unnamed SETUP key address, a proposed display-name
interpretation and the assumption that disabled test-HMI dimensions establish
active geometry. Earlier evidence records remain unchanged.

Phase 8 supporting reports:

- [Request escape](phase8/request-escape-analysis.md), [cleanup matrix](phase8/setup-cleanup-matrix.md), [request parser origin](phase8/setup-request-origin.md), [CF readiness v2](phase8/cf-adapter-readiness-v2.md).
- [AddScreenDisplay ABI](phase8/airplay-add-screen-display.md), [producer inventory](phase8/display-descriptor-producers.md), [UUID](phase8/display-uuid-contract.md), [geometry source](phase8/display-geometry-source.md), [second descriptor](phase8/second-display-descriptor-contract.md).
- [Displays references](phase8/displays-reference-accounting.md), [advertisement readiness v2](phase8/advertisement-adapter-readiness-v2.md), [DSI state provenance](phase8/dsi-display-state-provenance.md).
- [Wireless identification lifecycle](phase8/wireless-carplay-identification-lifecycle.md), [child17 insertion boundary](phase8/child17-insertion-contract.md), [secondary gates](phase8/secondary-display-gates.md), [type111 structure](phase8/type111-descriptor-contract.md).
- [Final static causality](phase8/type111-causality-final-static.md), [static limit and residual local work](phase8/static-re-limit.md), [document-only future observations](phase8/future-runtime-evidence-plan.md).

Phase 8 uses `STATIC_LIMIT_REACHED` only for an exhausted boundary whose missing
fact belongs to client/runtime behavior. It does not replace UNKNOWN with a
target fact or claim all local HMI/property writers were exhausted. Historical
arrow provenance may be labeled `HISTORICAL ONLY`; it is not P3695 proof.

The local P3695 data is outside this tracked analysis tree at
`.local-research/mpr3/P3695/` and is intentionally ignored.
