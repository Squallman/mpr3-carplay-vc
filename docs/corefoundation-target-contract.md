# CoreFoundation target adapter contract

No real CF adapter is implemented. Pointers stay opaque; there are no guessed
signatures, key strings, callback tables, layouts or addresses. This is the
design contract for a future `ISetupRequest` / `ISetupDescriptor` adapter.

Sources: [SETUP dictionary](../research/analysis/phase6/setup-dictionary-contract.md),
[ABI/ownership](../research/analysis/phase6/airplay-session-setup-abi.md) and
[hook contract](../research/analysis/phase6/runtime-hook-contract.md).
Semantic streams/type identification is **STRONG EVIDENCE**; exact target key
objects, linkage and helper signatures still require proof. Labels below are
documentation, never target lookup constants.

| Required operation | Evidence | ABI known? | Implement now? |
|---|---|---|---|
| Obtain streams array; detect absent/malformed | STRONG EVIDENCE: typed lookup at 0x58f88 | UNKNOWN: helper/key object/type checks | No |
| Enumerate retained descriptors in order | STRONG EVIDENCE: typed iteration at 0x59010–0x5901c | UNKNOWN: index/count ABI and ownership | No |
| Read wide integer descriptor type | STRONG EVIDENCE: lookup at 0x59038; 100..110 dispatch | UNKNOWN: helper/key/error semantics | No |
| Shallow request copy | PLAUSIBLE: read-only request path and proposed filtering | UNKNOWN: copy/allocator/error ownership | No |
| Replacement streams array | PLAUSIBLE: reuse original dictionaries | UNKNOWN: create/append/callback ABI | No |
| Reuse original descriptors | STRONG EVIDENCE: unchanged 110 pointer at 0x590a8 | UNKNOWN: retention and hidden mutation | No |
| Preserve opaque request data | Required core contract; shallow copy PLAUSIBLE | UNKNOWN: safe copy/replace operation | No |
| Retain/release on all paths | STRONG EVIDENCE: caller releases request/response | UNKNOWN: balances and transfer rules | No |
| Return stock response ownership-neutrally | STRONG EVIDENCE: x2 output/caller release | UNKNOWN: complete error-path ownership | Pointer forwarding only |

Future `streams()` must retain objects during use, preserve order/identity and
reject malformed input without partial extraction. `type()` must distinguish
unreadable values without narrowing unfamiliar integers into 111. Other fields
remain opaque. `withStreams()` must reuse original descriptors, preserve all
other request fields and avoid source mutation. Failure must forward the
original once and skip secondary startup. Stock 110 identity remains intact.

The adapter will own retained descriptors and temporary copies/arrays and must
release them after use and partial failure. Allocators/callback retention,
borrowed references and delayed stock use remain **UNKNOWN**. These balances
must be proven before encoding RAII; mock pointers never substitute for CF.

Required evidence before implementation:

- Exact exported signatures/calling conventions for dictionary, array, type,
  integer, copy, allocator and retain/release helpers.
- Target key objects and linkage; the additional connection/port key is UNKNOWN.
- Array callback layout, retention, mutability and safe opaque shallow copying.
- Request/descriptor/response ownership on every error path, delayed stock use,
  thread affinity and teardown.
- Copy-failure handling that preserves stock response/status and suppresses
  partially extracted secondary work.
