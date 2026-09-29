# SETUP dictionary contract

**STRONG EVIDENCE:** `AirPlayReceiverSessionSetup @ 0x58dc0` obtains a streams
object from the request with `CFDictionaryGetTypedValue` at `0x58f88`. Each
element is required/expected to be a CFDictionary: the code obtains
`CFDictionaryGetTypeID` and calls `CFArrayGetTypedValueAtIndex` around
`0x59010-0x5901c`. The descriptor type is read by `CFDictionaryGetInt64` at
`0x59038`; the code normalizes it by subtracting 100 and rejects values above
110 at `0x59044-0x59048`.

For type 110, the original descriptor pointer is passed unchanged to
`AirPlayReceiverSessionScreen_Setup` at `0x590a8`. A second integer is read
from the descriptor around `0x590b8-0x590c8` using a key at image address
`0x1072c8`; its semantic name (port, connection ID, or similar) is **UNKNOWN**.

The function creates a separate mutable response dictionary at `0x58e38` and
writes response values, including a type-110 value at `0x59158-0x59160`.

## Safe transformation boundary

The least-invasive conceptual transformation is:

```text
newStreams = CFArrayCreateMutable(...)
for each original descriptor:
    if descriptor.type != 111:
        CFArrayAppendValue(newStreams, descriptor)  // retains object
secondary = retain(original type-111 descriptor)
call stock setup with a request copy whose streams value is newStreams
```

This is **PLAUSIBLE / STRONG EVIDENCE**, not a tested implementation. It does
not rebuild or mutate the type-110 dictionary. A shallow array copy is likely
sufficient because the stock code reads descriptors and passes type 110
directly; whether an unobserved helper mutates a descriptor is **UNKNOWN**.
Deep-copying would be safer for mutation isolation but risks changing object
types/ownership semantics. The original array and dictionaries should be
treated as immutable inputs unless a future wrapper proves otherwise.

Known semantic keys:

| Key | Evidence | Confidence |
|---|---|---|
| streams | request lookup at 0x58f88, typed-array iteration follows | STRONG EVIDENCE |
| type | per-descriptor integer lookup at 0x59038; 100..110 dispatch | STRONG EVIDENCE |
| descriptor connection/port field | integer lookup around 0x590b8 using image key 0x1072c8 | UNKNOWN name |
| response type | response dictionary write at 0x59158 | STRONG EVIDENCE |
| audio/video/screen fields | not independently named in this function | UNKNOWN |
| latency/dataPort/controlPort/eventPort | not proven | UNKNOWN |
