# CFLite shallow request-copy contract

## Recovered construction and ownership

STRONG EVIDENCE: the CF container primitives can construct a shallow request
copy containing a new streams array and the original descriptor objects.
This is not yet approval to integrate that copy into a target SETUP hook.

| Operation | Evidence | Ownership / failure contract | Confidence |
|---|---|---|---|
| Mutable dictionary copy | CFDictionaryCreateMutableCopy 0x77650 → CFLDictionaryCreateCopy 0x7c250 | Null allocator, ignored w1 capacity; copies source callback tables, iterates original key/value pointers; owned result or null; partial result released on failure | STRONG EVIDENCE |
| New mutable array | CFArrayCreateMutable 0x76c10 → CFLArrayCreate 0x7ac10 | Null allocator required; ignored capacity; use exported CFL type callbacks for retaining elements; owned result or null | STRONG EVIDENCE |
| Append existing descriptor | CFLArrayInsertValueAtIndex 0x7aeb0 | Retain callback called with original pointer, which is then stored unchanged; allocation failure before insertion returns status | STRONG EVIDENCE |
| Replace streams value | CFLDictionarySetValue 0x7bfd0 | Retains replacement before releasing old value; no deep copy; return status must be checked | STRONG EVIDENCE |
| Destroy array / dictionary | Finalizers 0x7b310 / 0x7c560 → remove-all routines | Releases retained elements / keys / values via inherited callbacks | STRONG EVIDENCE |

An existing structural example is BonjourDevice_MergeInfo @ 0x75ba0 in the
same library: mutable dictionary copy 0x75c08; mutable array copy 0x75c60;
append existing object 0x75f8c; set replacement array 0x75df8; release local
array reference 0x75e88 after transferring the copied dictionary at 0x75e80.
This is actual container usage, not SETUP filtering or a proven 111 path.

## Answers for a future adapter

1. Original descriptor identity **can be preserved**: append stores the same
   pointer; shallow copy reuses all other request values.
2. Append retains with `kCFLArrayCallBacksCFLTypes`; retention is not guaranteed
   for arbitrary callback tables.
3. SetValue retains with CFL dictionary value callbacks. Mutable copy inherits
   the source callbacks, which must be established for a received request.
4. The constructor caller owns the mutable request's initial reference.
5. The array constructor caller owns its initial reference; successful set
   adds the copied dictionary's reference, allowing the local reference to
   be released. Keeping both until call completion is also balanced.
6. In an eventual verified synchronous forwarding contract, release local
   array, copied request and created key references on every path; borrowed
   original request/descriptors and stock responseOut must not be released by
   the adapter. On construction failure, release only successfully acquired
   references and forward the untouched original request. This is a proposed
   adapter rule, not an implemented or fully verified target lifecycle.
7. **UNKNOWN as a universal guarantee**: stock may retain request contents or
   invoke callbacks. The stock caller releases the whole original request
   after Setup on success/error, which is STRONG EVIDENCE for a borrowed-call
   contract, but does not itself prove every consumer retains escaped objects.
8. Deferred threads are visible for audio/screen state. PlatformControl receives
   the request (0x597a0 / 0x59d20); its typed enumeration 0x4863c–0x48860
   copies scalar state and explicitly retains a string at 0x487d4. Screen_Setup
   @ 0x5cf10 reads a scalar from the descriptor. No whole-request escape was
   found in these paths. Session delegate fallback and all exceptional/deferred
   consumers have not been exhaustively audited.

Container reference mechanics are recovered; the remaining integration blocker
is the complete received-request callback/escape lifetime, including allocator
failure and exceptional callback paths. Atomic refcounting does not establish
concurrent dictionary mutation safety. See [response ownership](setup-response-ownership.md)
and [readiness](cf-adapter-readiness.md).
