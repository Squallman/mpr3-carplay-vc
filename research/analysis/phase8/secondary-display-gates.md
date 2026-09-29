# Focused secondary-display and screen-count gate audit

**UNKNOWN**: no P3695-specific count/role/secondary gate linking identification,
AirPlay features and type111 was recovered. The direct construction and dispatch
facts below are stronger than a keyword search but do not prove firmware-wide
absence of every possible indirect state path.

| Inspected path | Control/data fact | Result |
|---|---|---|
| DIO getDisplays → createDisplaysDictionary | One newly created dictionary appended once; input-feature booleans and runtime window geometry | No count>1 production branch found; STRONG EVIDENCE |
| AddScreenDisplay 0x578f0..0x57a64 | Can append to an existing array; no count/UUID/role branch | Structural repeated append supported; no negotiation gate; PROVEN |
| /info dynamic property enumeration | `displays` callback object inserted with SetValue, released once; generic plist serialization | No per-display validator/count gate on this path; STRONG EVIDENCE |
| AirPlayGetFeatures 0x49d40..0x49d84 | Server property getter then OR floor0x6104040280; no displays-array argument/read | This getter's floor is independent of display count; PROVEN |
| SETUP enumeration 0x58f94..0x59060 | Streams count controls enumeration; type-index dispatch range100..110 | Stream count is not an advertised display count; PROVEN |
| Type110 state guard 0x59068..0x59070 | Tests session's existing screen state before setup | Single stock screen lifecycle guard, not a second-display enabling condition; STRONG EVIDENCE |
| Screen_Create / Screen_StartSession | Allocate context and consume copied geometry/delegates | No recovered secondary role/display UUID selection; STRONG EVIDENCE within these paths |
| DSI inputFeatures → descriptor features | Low byte/low four bits; input-interface diagnostic and metadata | Input-state path, not proof of alternate screen; STRONG EVIDENCE |
| Identification acceptance/rejection | ID0x1d02 module notification; ID0x1d03 rejected-field/retry handling | Feedback exists without recovered screen-count/alternate interpretation |

Exact `uuid`, primaryInputDevice and pixel-geometry key references were followed
in libairplay. Non-helper `uuid` users are HID utility/report paths. The two
filesystem ELF inventories found no additional display constructor key bundle
or imported AddScreenDisplay helper. ServiceConfiguration and the underlying
HMI sender remain separate from these count tests.

No AirPlay server-mask bit was newly identified as AltScreen. Bits7,9,18,26,32,
37,38 remain the observed mask floor, without a recovered secondary semantic.
The per-descriptor feature field is distinct: inputFeatures bits0/1/2/3 map
through helper booleans to descriptor masks2/4/8/16 respectively. The stock log
associates input bits with knob/low-fidelity/high-fidelity/touchpad; this is not
an AirPlay protocol specification for those descriptor masks.

Historical enabledFeatures/altScreen/uiContext/cornerMasks/focusTransfer paths
are **HISTORICAL ONLY** provenance. They do not establish a dormant P3695 parser
or a parent24 child17 trigger. The stock type111 handling remains unsupported.
See [causality](type111-causality-final-static.md).
