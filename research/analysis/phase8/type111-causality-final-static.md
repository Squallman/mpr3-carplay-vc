# Final static type111 causality audit

The requested chain mixes head-unit configuration, head-unit wire output and
phone decisions. It is not yet one demonstrated causal pipeline. Each arrow
below is assessed separately. HISTORICAL ONLY is a provenance label, not a
promotion of historical behavior into P3695 evidence.

| Arrow / fact | Classification | Reason and exact boundary |
|---|---|---|
| P3695 prepared Wireless object → parent24 children0/1/2/4 in IdentificationInformation0x1d01 | STRONG EVIDENCE | Named producer, selector3, nested TLV copy and message-ID/deployer path recovered |
| Parent24 bytes → phone receives/accepts identification | STRONG EVIDENCE (protocol path), UNKNOWN (specific runtime exchange) | Transport handoff and accepted/rejected parsers exist; no target/client run |
| Parent24/child17 → phone interprets secondary-screen/ThemeAssets capability | STATIC_LIMIT_REACHED; semantic fact remains UNKNOWN | No phone parser/decision code in P3695; local feedback carries no recovered secondary interpretation |
| Phone interpretation → local/internal alternate capability state | UNKNOWN | Acceptance/rejection notifications exist, but no such writer/reader edge recovered; no firmware-wide absence claim |
| Identification-derived local state → AirPlay server features | UNKNOWN | Property getter/OR floor recovered; no writer linked to identification |
| AirPlay server features → displays count/second descriptor | UNKNOWN | Separate property and builder paths; no observed dependence, no stock second entry |
| DSI ServiceConfiguration inputFeatures/window dimensions → descriptor construction | STRONG EVIDENCE | Deserialization, low-byte store, window-to-frame copy and getDisplays data flow |
| Displays callback result → /info dictionary → binary-plist client response | STRONG EVIDENCE | Generic property dispatch, retain/set/release, serializer and response destruction |
| Second display descriptor and advertised features → iOS sends SETUP type111 | STATIC_LIMIT_REACHED; semantic fact remains UNKNOWN | P3695 constructs/serializes info and parses incoming SETUP; the missing decision executes on the phone |
| Historical parent20/21 child17 → ThemeAssets / altScreen negotiation | HISTORICAL ONLY | Historical iOS/MHI3 report, different parent/schema; not parent24 proof |
| Stock P3695 already implements secondary SETUP111 | DISPROVEN | No111 handler in stock range/table; established stock boundary unchanged |

## Exhausted paths versus remaining local work

The audited boundaries include both display builders, their exact key use,
generic /info insertion/serialization, AirPlay feature getter, SETUP type
dispatch/screen creation, and identification start/accept/reject paths. The
complete ELF inventory supplies no additional named display producer or helper
import. None contains the phone-side validator or negotiation decision.

The upstream compiled HMI writer of ServiceConfiguration is still unclosed;
that local question is UNKNOWN, not STATIC_LIMIT_REACHED. Recovering it could
refine input/geometry provenance. It cannot by itself demonstrate which
parent24 extension and second descriptor a particular iOS accepts or whether
those inputs cause111. Therefore the external trigger questions reach a static
limit independently of that local residual work.

Generic child17 representation and repeated append support close structural
questions only. No inference of necessity/sufficiency follows. To prove a
trigger, the missing evidence must discriminate phone acceptance, internal
capability interpretation and actual subsequent SETUP requests. See
[static decision](static-re-limit.md) and
[future observations](future-runtime-evidence-plan.md).
