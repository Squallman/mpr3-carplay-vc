# AirPlayReceiverSessionSetup ABI

## Result

**STRONG EVIDENCE:** the recovered prototype is:

```c
int AirPlayReceiverSessionSetup(
    AirPlayReceiverSessionPrivate *session,  // x0
    CFDictionaryRef request,                 // x1
    CFDictionaryRef *responseOut             // x2
);
```

The entry at `libairplay.so:0x58dc0` saves x0 as the session object, x1 at a
local used as the request dictionary, and x2 at a local used as the response
out-parameter. The function returns a 32-bit status in w0. The only recovered
direct caller is `AirPlayReceiverServerControl` at `0x4c7bc`, which stores w0,
branches on zero/nonzero, and later releases both the request dictionary and
the object loaded from its response slot.

## Evidence

- `libairplay.so:0x58dc0`: x0/x1/x2 are the only entry arguments used in the
  recovered prologue; x3-x7 have no observed role.
- `libairplay.so:0x58df0`: `[sp+0xc8] = x1`.
- `libairplay.so:0x58e00`: `[sp+0xa0] = x2`.
- `libairplay.so:0x59008-0x59040`: request stream array is read through
  CoreFoundation dictionary/array helpers.
- `libairplay.so:0x4c7ac-0x4c7bc`: caller passes session at `[x20+0x10]`,
  request in x1, and `sp+0xd0` in x2.
- `libairplay.so:0x4c7c0`: caller stores w0 as status.
- `libairplay.so:0x4c83c`: caller releases the request.
- `libairplay.so:0x4c844-0x4c850`: caller loads and releases the response
  object from the output slot.

## Ownership and mutability

**STRONG EVIDENCE:** the request is treated as a read-only CFDictionary by the
recovered code; no mutation call on the input descriptor or streams array was
found. The caller retains responsibility for releasing the request. The
response is returned through x2 and is also released by the caller. Exact
retain-count details at every error path remain **UNKNOWN**.

Only one direct caller was recovered in the stripped image, so the requested
three-call-site comparison is not possible from this artifact.
