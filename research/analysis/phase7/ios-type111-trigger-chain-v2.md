# Updated iOS type-111 causal chain

Evidence labels remain PROVEN, STRONG EVIDENCE, PLAUSIBLE, UNKNOWN and
DISPROVEN. `HISTORICAL ONLY` below is the requested provenance label for an
arrow supported solely by the historical report; it is not target evidence.

| Arrow / fact | Classification | Evidence and limit |
|---|---|---|
| P3695 identification object → Wireless TLVs parent24 / children0,1,2,4 | STRONG EVIDENCE | Named producer, named trace field and selector-3 composer |
| Wireless identification → internal P3695 CarPlay capability state | UNKNOWN | No writer/reader IPC edge closed |
| Internal identification-derived state → server feature advertisement | UNKNOWN | Feature property provenance not tied to identification |
| Feature mask → displays advertisement | UNKNOWN | Same /info build, separate calculations; no count/mask dependency found |
| DSI ServiceConfiguration → descriptor feature byte / geometry | STRONG EVIDENCE | startService stores manager state consumed by getDisplays; originating iAP2 edge missing |
| Bound displays callback → /info displays array → binary-plist response | STRONG EVIDENCE | Generic property slot and insertion/serialization call chain |
| Advertised second display → iOS SETUP type111 | UNKNOWN | Minimum second descriptor/client gates not recovered; no target/client execution |
| Historical parents20/21 child17 → iOS ThemeAssets state | HISTORICAL ONLY | Historical iOS26.1 parser report; P3695 emits Wireless parent24 |
| Historical ThemeAssets state → proposed altScreen feature | HISTORICAL ONLY | Historical CoreAccessories/CarKit/Ferrite report |
| Historical enabledFeatures response → negotiated altScreen | HISTORICAL ONLY | Historical activation report; not a P3695 response contract |
| Stock P3695 SETUP accepts type111 | DISPROVEN | Range handling rejects 111+, preserved from Phase6 |

```text
P3695 Wireless identification
  -- UNKNOWN --> identification-derived internal state
  -- UNKNOWN --> AirPlay feature mask
  -- UNKNOWN --> displays advertisement dependency
  -- UNKNOWN --> iOS sends type111

DSI ServiceConfiguration
  -- STRONG EVIDENCE --> stock descriptor construction
  -- STRONG EVIDENCE --> /info displays insertion and serialization
```

This chain must not be used to infer sufficiency of child17, a feature bit or
duplicating the stock descriptor. Generic child17 serialization closes a
byte-representation question; none of the client causal gates above becomes
PROVEN as a consequence.
