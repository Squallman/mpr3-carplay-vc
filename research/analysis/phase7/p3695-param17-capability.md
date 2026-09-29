# Child parameter 17 capability

## Classification

**GENERICALLY_EXPRESSIBLE** — STRONG EVIDENCE at the existing P3695 generic
TLV buffer/aggregation layer. This classification does **not** mean native
WirelessCarPlay field support, a ready insertion API, emitted wire traffic,
or a proven ThemeAssets/iOS effect.

| Question | P3695 result | Evidence / confidence |
|---|---|---|
| Explicit Wireless child 17? | Not present in inspected typed composer | Selector-3 branch 0x430d8–0x435xx builds only 0,1,2,4; STRONG EVIDENCE |
| Generic raw child ID representation? | Yes, 16-bit writer has no ID allowlist | 0x3e4e0 stores high/low ID bytes at header+2/+3; PROVEN |
| Zero-payload representation? | Yes, constructor accepts payload length 0 | 0x3f9c0 adds four bytes and writes header length; stock children 2/4 use it; STRONG EVIDENCE |
| Generic nesting preserves child 17? | Yes, aggregates child byte views without interpreting ID | 0x3f410 sums child header lengths, copies buffers, writes parent length; STRONG EVIDENCE |
| Typed Wireless object accepts arbitrary children? | No such insertion path recovered | Component has four inspected fields, not an arbitrary child vector; UNKNOWN (other APIs) |
| Does P3695 ignore/reject a newly added 17 later? | No ID filter in inspected aggregation; full downstream validation UNKNOWN | This is not a universal acceptance claim |
| Does relevant iOS parse parent 24/child17 as ThemeAssets? | UNKNOWN | Historical evidence concerns parents 20/21, not this recovered parent 24 |

## Complete byte-level proof

1. Payload constructor 0x3f9c0 takes a narrow 16-bit length in w1, allocates
   payload+4 bytes and writes big-endian total length at 0x3fa2c/0x3fa30.
2. Raw ID writer 0x3e4e0 narrows w1 to 16 bits and writes ID without a switch
   or factory table.
3. Nested aggregator 0x3f410 takes x0 child-view vector, x1 parent parameter.
   It walks 24-byte views, sums header lengths (not child payload lengths),
   reserves parent storage for sum+4, copies the actual child buffers through
   temporary byte copies at 0x3f51c / 0x3f4d8, and returns true for a nonempty
   aggregate. It never tests child ID. Temporary copies are freed 0x3f4ec.
4. A zero-payload child therefore has nonzero total length 4 and is not dropped
   as an empty aggregate. The inferred header is `00 04 00 11` for ID17.
   These four bytes are a calculation from observed stores, **not captured
   P3695 output**. Parent ID must be assigned separately.

This proves more than the existence of a generic base class. It does not
recover a safe public C++ constructor/destructor ABI or an insertion hook.
The stock Wireless composer is a fixed typed emitter; augmenting its child
views would require new integration, proven buffer lifetimes and correct
parent semantics. Calling these image addresses is not a proposed mechanism.

The explicit ID17 branch found under LocationInformation is not native
support for the intended Wireless capability. Overall adapter readiness is
[MORE_RE_REQUIRED](iap2-adapter-readiness.md).
