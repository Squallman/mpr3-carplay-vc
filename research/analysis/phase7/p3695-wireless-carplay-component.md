# P3695 WirelessCarPlay component serialization

## Named structure to serializer control flow

Source: libesoiap2.so identificationInformation composer @ 0x43f60 and shared
transport helper @ 0x427b0; dio_manager producer @ 0x2631f0. Helper labels in
objdump sometimes name the preceding exported function; those labels are not
used as recovered helper names.

STRONG EVIDENCE: the named WirelessCarplayTransportComponents field is at
identification-object+0x380 (presence), +0x384 (count), +0x388 (array pointer).
Named trace routine reads it at 0x2b3b4–0x2b3b8, invoking the named components
formatter. Component formatter @ 0x28c90 uses literal @ 0x675f0:
`compId`, `name`, `supportsIAP2Connection`, `supportsCarPlay`. Its component
layout is presence+0, identifier presence+2/value+4, name presence+8/string+0x10,
and zero-payload flags +0x30/+0x31. Array stride is 56 bytes.

The composer checks this field and calls helper 0x427b0 at 0x465dc with
selector w1=3. Jump table @ 0x69f70 selects 0x430d8 for selector 3. This
branch loops the same +0x388 array, constructing children in the order below.
This named-field/caller/callee alignment is stronger than numeric-ID searching.

| Child ID | Type | Payload / total TLV length | Source | Meaning | Confidence |
|---|---|---|---|---|---|
| 0 | UInt16 bytes, big-endian | 2 / 6 bytes | component+4 when presence+2; configured identifier 3 | Component identifier | STRONG EVIDENCE |
| 1 | NUL-terminated string | string length+1 / length+5 | component string+0x10 when presence+8; configured `WirelessCarPlayTransportComponent` | Component name, not an assumed UUID | STRONG EVIDENCE |
| 2 | None/void marker | 0 / 4 bytes | component+0x30 | supportsIAP2Connection | STRONG EVIDENCE |
| 4 | None/void marker | 0 / 4 bytes | component+0x31 | supportsCarPlay | STRONG EVIDENCE |

Exact child ID setter calls: 0x4330c, 0x4339c, 0x43438, 0x43498.
Identifier byte stores: 0x4332c / 0x4333c. Name bytes/length update: 0x433cc.
Markers use zero-payload constructors at 0x431f4 / 0x4323c. Their payload
length is zero, but their TLV length includes the four-byte header.

PROVEN numeric fact: after nested aggregation at 0x434e4, the helper sets
parent ID **24 (0x18)** at 0x434fc, then appends its view to the overall
parameter vector. Empty child collections are not emitted. The association
of this ID with WirelessCarPlay is STRONG EVIDENCE from the trace above.
It is not historical MHI3 parent 21.

## Producer and conditions

dio CIAP2AccessoryIdentificationWireless::addIdentificationInformation @
0x2631f0 tests getSupportedCarPlayTechnologies against values 2/3 and reaches
Wireless construction 0x2637a0. It sets presence/count=1, allocates one
56-byte component plus allocation header, sets identifier/name from getters
0x264b90 / 0x264bb0 (ExtDevConfig EKeys 23/24), and sets both flags at
0x263864 / 0x26386c. extdevconfig.json contains identifier 3 and the name
above. Config values are PROVEN; live emission additionally depends on this
branch and connection/identification lifecycle.

The payload is owned by the identification object. Composer uses local
typed parameter arrays, 24-byte buffer views and a parent buffer; aggregator
copies child bytes into parent storage. Temporary allocations and C++ cleanup
exist. Complete exported mutation API, IPC handoff ownership and exception
lifecycle suitable for an injected adapter remain UNKNOWN. No raw object
layout was implemented.

## Child 17 and negative control

The closed typed Wireless branch tests only the four fields above. No extra
child collection/raw-ID setter is exposed through that inspected component
layout. Numeric 17/18/20 construction found around 0x45578–0x456fc belongs
instead to **LocationInformation**, named by the trace at 0x2b1e8 using
identification+0x338; its parent is **22**, set at 0x456c0. It does not prove
Wireless ThemeAssets support. [Generic byte serialization](p3695-param17-capability.md)
is a separate capability.

iap2connectionmanager symbols/config accessors and libesoiap2 named formatters
were searched alongside dio. The direct identification producer recovered
here is in dio_manager, not an inferred string-based path in the connection
manager. No observed wire capture or target execution is claimed.
