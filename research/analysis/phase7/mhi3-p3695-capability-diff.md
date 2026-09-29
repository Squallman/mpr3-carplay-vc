# Historical MHI3 / P3695 capability differential

Historical source: commit `e9c2b9a6c231b192420be21fdea78a8565068c47` in the
local sibling repository `mib2q-carplay-rgi`, `docs/altscreen_gate_analysis.md`
and `docs/MIB3.md`, read with git show. The commit was not available in this
repository. No historical blobs were imported. MIB3.md primarily covers
navigation/cluster behavior; the gate document supplies the transport/iOS
claims below. Historical binaries and iOS binaries were **not independently
reanalyzed in Phase 7**.

The confidence column distinguishes current-target evidence from claims in
the historical report. Historical-only provenance is not a sixth evidence
classification and does not imply PROVEN for P3695.

| Mechanism | Historical MHI3 | P3695 | Same? | Confidence |
|---|---|---|---|---|
| Wireless parent | Report identifies parent 21, composer 0x4f9a0 | Named field/selector-3 trace emits parent 24 | No in inspected composers | STRONG EVIDENCE (P3695); historical report only |
| Wireless child 0 | Report: UInt16 identifier | UInt16, configured 3 | Structural similarity; numbering alone does not establish protocol equivalence | STRONG EVIDENCE |
| Wireless child 1 | Report calls it 32-byte UUID | Named formatter says name; NUL-terminated configured name | Different recovered semantics | STRONG EVIDENCE |
| Wireless child 17 | Report: zero-payload void, vtable 0xa64e8 | No native field; generic TLV representation possible | Not native-equivalent | STRONG EVIDENCE |
| Wireless child 18 | Report: zero-payload void | Absent from inspected Wireless typed emitter | Semantics/equivalence UNKNOWN | UNKNOWN |
| Wireless child 20 | Report: zero-payload void | Absent from inspected Wireless typed emitter | Semantics/equivalence UNKNOWN | UNKNOWN |
| Other P3695 ID17/18/20 | Historical report associates these with Wireless gates | LocationInformation parent22 also emits 17/18/20 | Not the same component | STRONG EVIDENCE |
| ThemeAssets | Historical iOS26.1 parser parents20/21 child17 → byte+131 → accessory capability bit21 → CarKit supportsThemeAssets | No P3695-specific causal or parser acceptance proof for parent24 | UNKNOWN | UNKNOWN |
| enabledFeatures | Reported MHI3 SETUP-response CFArray; key @ 0x115478 | No recovered equivalent gate/response insertion in inspected SETUP path | UNKNOWN | UNKNOWN |
| altScreen | Reported MHI3 HasFeatureAltScreen and iOS negotiation | P3695 stock rejects type111; no equivalent gate proven | No stock handling equivalence | PROVEN (P3695 rejection); UNKNOWN (trigger) |
| uiContext | Reported negotiated enabledFeatures string | No P3695 corresponding semantics proven | UNKNOWN | UNKNOWN |
| cornerMasks | Reported negotiated enabledFeatures string | No P3695 corresponding semantics proven | UNKNOWN | UNKNOWN |
| focusTransfer | Reported negotiated enabledFeatures string | No P3695 corresponding semantics proven | UNKNOWN | UNKNOWN |

The historical gate report also discusses USBDevice parent20 child17,
constructor payload-size 4 versus Wireless payload-size 0. That difference
must not be normalized away. It leaves children18/20 unnamed void flags;
Phase 7 does not invent their semantics. Its older MHI2Q encoder limitation
is not a P3695 limitation: P3695 has named transport components and a proven
generic nested-buffer serializer.

DISPROVEN: assuming that historical parent21/child17 can be copied directly
into P3695's named Wireless composer without establishing its parent24
protocol meaning and client acceptance. Generic encodability is independent
of the historical ThemeAssets gate and of enabledFeatures negotiation.
