# Complete field insertion inventory of createDisplaysDictionary

Source: dio_manager helper @ 0x1d9be0, getDisplays @ 0x1db510. The helper
symbol demangles to four bools, seven unsigned ints, one int and int&.
Register/stack field sources and every SetValue/SetInt64 insertion were
traced. Reconstructed field values below do not claim live vehicle state.

| Field/key object | Type | P3695 value / source | Condition | Evidence | Confidence |
|---|---|---|---|---|---|
| `features` @ 0x32b080 | CFNumber integer | bool arguments contribute 0x2, 0x8, 0x4, 0x10; getDisplays supplies manager+0x158 bits 0,2,1,3 respectively | Always attempted | 0x1d9c58–0x1d9c94 | PROVEN (construction); UNKNOWN (bit semantics/live value) |
| `maxFPS` @ 0x32b098 | CFNumber integer | First stack unsigned; config EKey 152, default 60 when getter returns 0; screen.maxFPS configured 60 | Always attempted | 0x1d9ca4; getDisplays 0x1db5a8 | STRONG EVIDENCE |
| `widthPixels` @ 0x32ae80 | CFNumber integer | w6, metadata manager+0x454+0x10 | Always attempted | 0x1d9ccc | PROVEN (source); UNKNOWN (active value) |
| `heightPixels` @ 0x32ae98 | CFNumber integer | w7, metadata manager+0x454+0x14 | Always attempted | 0x1d9cdc | PROVEN (source); UNKNOWN (active value) |
| `widthPhysical` @ 0x32b0c8 | CFNumber integer | Stack unsigned at callee x29+0x68; metadata+0x18 | Always attempted | 0x1d9d00 | PROVEN (source); UNKNOWN (active value/units) |
| `heightPhysical` @ 0x32b0e0 | CFNumber integer | Stack unsigned at callee x29+0x70; metadata+0x1c | Always attempted | 0x1d9d10 | PROVEN (source); UNKNOWN (active value/units) |
| `primaryInputDevice` @ 0x32b0f8 | CFNumber integer | Signed stack int; metadata+0x30 enum 1..8 maps through table 0x32af98 to [3,1,0,1,0,0,0,2]; otherwise 0 | Only nonzero | Branch 0x1d9d18; set 0x1d9ddc | STRONG EVIDENCE |
| `uuid` @ 0x32afb8 | CFString | Constant object 0x32afd8, text `e5f7a68d-7b0f-4305-984b-974f677a150b` | Always attempted | SetValue 0x1d9d2c | PROVEN |

Keys and UUID value have CFL constant-string headers and inline text at +8;
the insertion call arguments establish exact field identity. Incoming w4/w5
are unused by the insertions; no semantic names are invented for them.

The helper returns **an array containing one dictionary**, despite its name.
It uses exported CFL type key/value/array callbacks, appends the dictionary
0x1d9d38, releases its local dictionary reference 0x1d9d40 and sets error 0.
Allocation failures release acquired objects and report -6728. Mutation
statuses are ignored; CFDictionarySetInt64 itself discards SetValue errors.
Consequently error=0 does not prove a complete descriptor under allocation
failure. A new adapter must not duplicate that failure weakness.

## Corrections to the baseline

The numbers 1540, 720, 235 and 110 appear in **disabled test-HMI configuration**
in dio_manager.json (test section around lines 613–627, enable=false), together
with `Audi MMI`. Actual getDisplays values are service state, populated by
CDSICarplayImpl::startService @ 0x2170c0; e.g. stores at 0x2172ac–0x21730c
and subsequent metadata stores. Config existence is PROVEN; actual active
dimensions are UNKNOWN. Earlier phase conclusions are retained as historical
records and superseded here where they implied active values.

`Audi MMI` is **not a field emitted by this helper**. The value inserted at
0x1d9d2c is the UUID above, not a display name.

## Absent and still required

ABSENT FROM THIS HELPER: display name, separate display ID, modes, main/primary
role, overscan, rotation, pixel density, transport/context identifier, and
other capability fields beyond features/primaryInputDevice. This does not
prove that iOS never requires them, or that no other helper emits them.

STRONG EVIDENCE: these eight fields describe this stock helper's successful
construction. UNKNOWN: minimum structurally/client-valid second descriptor,
unique UUID requirements, secondary role/features, and client acceptance of
duplication. A copied first descriptor is not evidence for a valid second
screen. No second descriptor or constants were implemented.
