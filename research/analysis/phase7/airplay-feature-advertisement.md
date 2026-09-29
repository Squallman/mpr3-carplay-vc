# AirPlay feature advertisement

PROVEN: AirPlayGetFeatures @ libairplay 0x49d40 reads the server `features`
property with CFObjectGetPropertyInt64Sync and ORs **0x6104040280**, setting
bits 7, 9, 18, 26, 32, 37 and 38. This is an OR floor, not proof that the
complete runtime mask equals that constant. Additional property-supplied
bits remain possible.

STRONG EVIDENCE: AirPlayCopyServerInfo calls it at 0x4ad44 and inserts the
result under exact `features` CFString object 0x103ac8 at 0x4aea8. Displays
are queried separately by the dynamic property list within the same response.
AirPlayGetFeatures contains no display-count branch. The inspected display
constructor and /info insertion path do not feed a count into that mask.

| Bit | Binary fact | Semantic result | Confidence |
|---|---|---|---|
| 7 | Set by constant OR | UNKNOWN | PROVEN (set); UNKNOWN (meaning) |
| 9 | Set by constant OR | UNKNOWN | PROVEN (set); UNKNOWN (meaning) |
| 18 | Set by constant OR | UNKNOWN | PROVEN (set); UNKNOWN (meaning) |
| 26 | Set by constant OR | UNKNOWN | PROVEN (set); UNKNOWN (meaning) |
| 32 | Set by constant OR | UNKNOWN | PROVEN (set); UNKNOWN (meaning) |
| 37 | Set by constant OR | UNKNOWN | PROVEN (set); UNKNOWN (meaning) |
| 38 | Set by constant OR | UNKNOWN | PROVEN (set); UNKNOWN (meaning) |

No feature bit was newly assigned an AltScreen or other semantic meaning.
Co-presence of fields in /info is not a control/data dependency.

dio infoRequestExtendedFeatures @ 0x1d9060 builds an array and appends a
constant CFLString @ 0x32ad78 containing `vocoderInfo`. That is a separate
`extendedFeatures` property, not evidence that any numbered mask bit means
secondary screen. The descriptor's `features` field is another distinct
number assembled from manager+0x158 bits; it must not be conflated with the
server's 64-bit mask or historical enabledFeatures strings.

Searches covered AirPlayGetFeatures callers, /info dynamic enumeration,
descriptor construction and service-state input. Complete property-writer
provenance and all secondary-screen branches elsewhere remain UNKNOWN.
