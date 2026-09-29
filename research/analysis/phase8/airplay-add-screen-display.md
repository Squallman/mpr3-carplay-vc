# AirPlayInfoArrayAddScreenDisplay

## Machine ABI

Export in libairplay.so: **0x578f0, size0x174**. The complete body ends before
0x57a64. Disassembler labels `AirPlayInfoArrayAddScreenDisplay+0x180/+0x440`
refer to separate unnamed functions beyond that size, not helper callers.

**STRONG EVIDENCE**, reconstructed from argument saves and every insertion:

```text
int32 status AddScreenDisplay(
    opaqueMutableArray **inOutArray, opaqueCFObject *uuid,
    uint32 features, uint32 primaryInputDevice, uint32 maxFPS,
    uint32 widthPixels, uint32 heightPixels, uint32 widthPhysical,
    uint32 heightPhysical);

x0 = nonnull array slot; x1 = borrowed UUID object
w2 = features; w3 = primaryInputDevice; w4 = maxFPS
w5 = widthPixels; w6 = heightPixels; w7 = widthPhysical
entry stack+0 = heightPhysical; w0 return = status
```

The pseudoprototype expresses the observed register contract, not recovered
Apple typedefs or a production header. The numeric setters zero-extend each
32-bit input into a 64-bit CF integer value.

| Inserted key | CF object | Value | Condition / site |
|---|---|---|---|
| uuid | 0x107a68 | x1 object, no UUID creation/parsing | Always, 0x57964 |
| features | 0x107c68 | w2 | Always, 0x57974 |
| primaryInputDevice | 0x107c80 | w3 | Always, including zero, 0x57984 |
| maxFPS | 0x107ca0 | w4 | Only nonzero, 0x57988 → 0x57a14 |
| widthPixels | 0x107cb0 | w5 | Always, 0x5799c |
| heightPixels | 0x107cc8 | w6 | Always, 0x579ac |
| widthPhysical | 0x107ce0 | w7 | Always, 0x579bc |
| heightPhysical | 0x107cf8 | stack argument | Always, 0x579cc |

**PROVEN:** exact inline key bytes and insertion instructions. No display name,
role, modes, transport ID, secondary flag or context ID is inserted by this body.
No geometry/feature default is generated; callers supply them. Zero maxFPS means
omission, not a recovered default frame rate.

## Construction, ownership and errors

It dereferences *inOutArray at 0x57924. If null, it creates a retaining mutable
array at 0x57a34 and writes the owned array to the caller slot at 0x57a38.
An existing array is borrowed and mutated without an extra retain. It creates
a retaining mutable dictionary at 0x57948, inserts borrowed UUID via SetValue,
appends at 0x579d8 and releases the local dictionary at 0x579e0. On successful
append, array owns the same dictionary. UUID is retained by the dictionary.

Null array/dictionary allocation produces -6728. If array creation succeeds
but dictionary allocation fails, that newly created array remains in the output
slot for its caller to release. The helper discards integer setter, SetValue and
append statuses and returns zero after the attempted insertions; zero does not
prove a complete descriptor or successful append under allocation failure.
There is no UUID type, uniqueness or nonzero-geometry validation.

## Callers and relation to DIO

No direct branch caller, relocation to the helper, or function-address data
reference was recovered inside libairplay. Across **1188 ELF files** in the two
extracted filesystem images, the symbol name occurs only in its own defining
library; no other ELF import/name reference was found. Named dynamic resolution
would also need that string; computed lookup remains theoretically possible and
UNKNOWN. An exported helper is not proof of active use.

Classification: **UNUSED/DORMANT**, STRONG EVIDENCE within the inspected stock
ELF scope. Non-ELF HMI resources were inventoried, not decompiled universally.

DIO's createDisplaysDictionary independently constructs the same eight-key
schema. Its maxFPS is emitted even for zero and its primaryInputDevice is
omitted for zero, whereas this helper does the reverse conditional choices.
Thus neither omission nor emission alone establishes an iOS mandatory-field
contract. See [second descriptor](second-display-descriptor-contract.md).
