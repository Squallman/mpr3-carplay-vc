# Child17 structural insertion contract

Insertion mechanism: **GENERIC_OBJECT_SUPPORTED**, **STRONG EVIDENCE** at the
internal serializer boundary. Native typed Wireless support remains absent from
the recovered field layout. Phase7's child17 capability classification remains
**GENERICALLY_EXPRESSIBLE**; neither classification grants semantic permission
to emit it or provides a stable external C++ adapter ABI.

## Most exact recovered boundary

The internal nesting routine at libesoiap2 0x3f410 takes x0=a vector-like
begin/end/capacity object of **24-byte parameter views**, x1=an owned mutable
parent parameter record. It reads each child's big-endian 16-bit length,
sums lengths at 0x3f434–0x3f444, reserves/resizes parent storage to sum+4 at
0x3f46c–0x3f49c and writes the parent's big-endian length at 0x3f4a4/0x3f4a8.
It copies children in view order into the parent at the 16-bit cursor stored
at parent+0x30 (0x3f4c8), advances that cursor at 0x3f4e8, and frees its
temporary child copy at 0x3f4ec. It does not whitelist IDs.

Zero-payload record construction at 0x3f9c0 supports a four-byte TLV header;
the raw ID setter at 0x3e4e0 writes a supplied 16-bit ID. Together they can
represent a void child17 view. This is proved from constructor/setter/copy
control flow, not from a generic base-class name. No child17 was constructed or
emitted during research.

| Boundary | Result | Evidence / limitation |
|---|---|---|
| Typed Wireless object | Not TYPED_SUPPORTED for arbitrary17 | Only identifier/name/supportsIAP2Connection/supportsCarPlay fields recovered |
| Generic nested views before parent aggregation | GENERIC_OBJECT_SUPPORTED | Routine sums and copies views without filtering ID; owning record can represent empty payload |
| Wireless stock call site | Fixed current order0,1,2,4 | 0x43438 sets ID2, 0x43498 ID4; aggregator 0x434e4 then parent ID24 at 0x434fc |
| After parent aggregation / raw final buffer | Not needed for structural expression | Earlier generic boundary exists; raw patching is neither proposed nor performed |

## Ownership, length and cleanup constraints

Stock allocates parameter objects and view vectors dynamically/on stack for
the component count; views refer to owning records and must not outlive them.
The children are copied into the parent before their owning records are
destroyed; parent is copied into the final control message before composer
cleanup. Normal composer cleanup iterates virtual record destructors, frees
view-vector storage and releases message buffers after deploy. Cold landing
pads free constructed records/vectors and resume C++ unwinding. This is evidence
of stock lifecycle structure, not a completely recovered generated C++ ABI
safe for an external plugin to manufacture.

A future extension would need a correctly owned fifth record/view before
0x434e4, sufficient vector capacity, the established order decision, and intact
exception cleanup. Typed field mutation alone cannot add that view. Parent and
cursor lengths are encoded in 16 bits; overflow checks cannot be inferred from
allocation size arithmetic and any future builder must reject oversized data.
No numeric client requirement for order or inclusion17 is recovered.

The semantic necessity of child17, parent24 interpretation and safe integration
of an added generic object remain UNKNOWN. The iAP2 adapter remains
MORE_RE_REQUIRED. No post-serialization mutation or firmware modification was
performed. See [lifecycle](wireless-carplay-identification-lifecycle.md).
