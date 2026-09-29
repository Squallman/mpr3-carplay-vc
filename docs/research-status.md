# Research status

## Proven

- P3695 is Linux/AArch64.
- Stock CarPlay screen is stream type 110.
- Stock SETUP rejects stream type 111.
- A stock screen context is separately allocated and owns socket/ScreenStream state.
- VideoEncoding directly resolves an integer IPTE displayable and feeds it.
- `setActiveDisplayable(displayID, displayable:i32)` exists.
- `AirPlayReceiverSessionSetup` has strong evidence for `(session, request, responseOut)`.
- The symbol is exported and internally called through PLT/JUMP_SLOT.
- Configured displayable creation resolves a LayerConfig name.
- SETUP CFString objects are exactly `streams`, `type` and `streamConnectionID`; the last is at image address 0x107330, correcting the earlier approximation. See [Phase 7 key map](../research/analysis/phase7/setup-key-map.md).
- The stock display helper inserts features, maxFPS, pixel/physical dimensions, conditional primaryInputDevice and uuid; it does not insert a display name. See [Phase 7 descriptor inventory](../research/analysis/phase7/stock-display-descriptor-complete.md).

## Strong evidence

- ScreenStream/decoder factories are suitable for separate instances.
- `infoRequestDisplays(int&)` returns a retained display array.
- The stock display helper emits one descriptor with width/height/physical fields and a feature field.
- CFLite helper register ABIs, shallow-copy mechanics and retaining container callbacks are recovered. Phase 8 closes parser/storage and stock SETUP consumer lifetime sufficiently for **READY_FOR_CF_ADAPTER**: release the temporary copy after ordinary return and scope local references during unwinding. This is input implementation readiness, with runtime binding/concurrency validation separate; no adapter is implemented. See [CF readiness v2](../research/analysis/phase8/cf-adapter-readiness-v2.md).
- Displays dispatches through generic delegate+0x18 (server+0x30), then the manager's `displays` property binding, rather than a direct method slot. See [delegate layout](../research/analysis/phase7/airplay-server-delegate-layout.md).
- P3695's named WirelessCarPlay composer emits parent24 with children0/1/2/4. Child17 is generically expressible by existing TLV buffer routines, without native Wireless field or client-capability proof. See [serializer](../research/analysis/phase7/p3695-wireless-carplay-component.md) and [child17 classification](../research/analysis/phase7/p3695-param17-capability.md).
- AirPlayInfoArrayAddScreenDisplay has a reconstructed nine-argument ABI and constructs the same eight-key display schema; no stock caller/import was recovered across the filesystem ELF inventory. It does not establish a second-display client contract. See [helper](../research/analysis/phase8/airplay-add-screen-display.md).
- Displays callback accounting leaves a net extra array reference on the inspected ordinary /info path (**LEAK_SUSPECT**); no cache/transfer explanation was found. This is static accounting, not an observed runtime leak. See [reference accounting](../research/analysis/phase8/displays-reference-accounting.md).
- DSI ServiceConfiguration+0x60 is inputFeatures; its low byte feeds manager+0x158. Advertised pixel dimensions come from configured window resolution, copied into frame state. Actual live values remain unknown. See [DSI provenance](../research/analysis/phase8/dsi-display-state-provenance.md) and [geometry](../research/analysis/phase8/display-geometry-source.md).
- Parent24 is sent in IdentificationInformation0x1d01; start0x1d00, accepted0x1d02 and rejected0x1d03 feedback exists, including retries. No recovered feedback reports alternate-screen interpretation. Generic internal object insertion is structurally supported, without an external adapter ABI. See [lifecycle](../research/analysis/phase8/wireless-carplay-identification-lifecycle.md) and [insertion boundary](../research/analysis/phase8/child17-insertion-contract.md).

## Plausible

- A preload or equivalent in-process hook could preserve 110 while privately handling 111.
- A separate configured displayable could feed the existing encoder.
- `Displayable_Debug_1` is a possible test candidate, not a safe production choice.

## Unknown

- Exact iOS type-111 trigger.
- P3695/client relevance of historical child17 semantics; safe external insertion ABI/integration lifecycle and identification-to-AirPlay dependency.
- Minimum valid second display descriptor, balanced callback-result ownership and complete delegate consumer map.
- Active display geometry: 1540×720 / 235×110 are disabled test-HMI configuration values, not proven live state. See [Phase 7 corrections](../research/analysis/phase7/stock-display-descriptor-complete.md).
- AppArmor/runtime loader authorization.
- Active cluster endpoint.
- Safe displayable occupancy and target concurrent lifecycle.

The client-valid minimum second descriptor, UUID/role rules and iOS type111
trigger reach a bounded **STATIC_LIMIT_REACHED** from these head-unit artifacts.
The originating HMI configuration writer and complete server-feature-property
provenance remain local UNKNOWN questions, not exhaustively closed. Advertisement
and iAP2 adapters remain **MORE_RE_REQUIRED**. See the [Phase 8 decision](../research/analysis/phase8/static-re-limit.md)
and [document-only future evidence plan](../research/analysis/phase8/future-runtime-evidence-plan.md).

## Disproven

- Merely changing the 100..110 range check is sufficient.
- Stock P3695 already handles type 111.
- MHI2Q/MHI3 similarly named objects prove MPR3 behavior.
- The recovered display delegate contains a direct infoRequestDisplays pointer; it uses generic property dispatch.

Detailed evidence is indexed in [`research/analysis/README.md`](../research/analysis/README.md).
