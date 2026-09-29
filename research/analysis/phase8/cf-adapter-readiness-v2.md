# CoreFoundation SETUP adapter readiness v2

**READY_FOR_CF_ADAPTER**, with **STRONG EVIDENCE** for the inspected P3695
binary-plist → stock SETUP forwarding path. This is readiness to implement a
separate future adapter, not target deployment readiness or type111 readiness.

| Requirement | Phase8 result | Confidence |
|---|---|---|
| Helper symbol/register ABI | Phase7 recovered the actual CFLite definitions and callers; no Apple-header assumption | STRONG EVIDENCE |
| `streams` / `type` retrieval | Exact inline CF objects at 0x106610/0x107128; content-equal dynamic CFStrings use proven equality/hash mechanism | PROVEN (objects); STRONG EVIDENCE (retrieval) |
| Iteration / integer extraction | Count/index and typed descriptor lookup; Int64 conversion with error-out | STRONG EVIDENCE |
| Retaining received containers | Binary-plist recursive dictionary/array constructors traced to standard retaining tables | STRONG EVIDENCE |
| Identity-preserving shallow copy | Inherits original dictionary callbacks; append retains original descriptor pointers | STRONG EVIDENCE |
| Post-return request lifetime | Type110 copies scalars/keys; audio stored CF object retained; SETUP platform commands stay local | STRONG EVIDENCE |
| Ordinary failure cleanup | Wrapper references independent of stock response; same deterministic local releases on every status | STRONG EVIDENCE |
| Unwinding cleanup | Scope local references; preserve unwind; stock exceptional response balance not repaired | STRONG EVIDENCE (local contract) |

## Exact future implementation contract

Resolve exported helper functions and callback objects by symbol, using the
recovered ABI in [Phase7](../phase7/cf-helper-abi.md). Image addresses are evidence
metadata. Create dynamic content-equal `streams` and `type` keys using
CFStringCreateWithCString(null, text, 0x08000100); own/release those keys.
Do not reinterpret host ISetupRequest as a CF object.

Use CFDictionaryGetTypedValue(request, streamsKey, actual array type ID,
errorOut) for borrowed streams. CFArrayGetCount returns a 32-bit count; typed
index lookup takes a 32-bit index and dictionary type ID. Descriptor references
are borrowed from that array. CFDictionaryGetInt64 returns signed 64-bit data
and accepts an optional int32 error-out; distinguish conversion failure from a
valid integer. Unclassifiable input must be forwarded unchanged.

Create the request with CFDictionaryCreateMutableCopy(null, 0, original).
Create replacement streams with CFArrayCreateMutable(null, 0,
kCFTypeArrayCallBacks). Append the original dictionary pointers for preserved
streams, in their original order. Check every append status. Set only the
`streams` value; check SetValue status. Preserve all other request data and
never mutate stock descriptors. Type110 remains identical.

Pass session and responseOut unchanged to the original three-argument function
exactly once. Return its exact status. Release locally owned copies/arrays on
normal return and by scoped guards during unwinding. Never release borrowed
request/descriptors or the original's output. Construction failure releases
partial local state and forwards the original request once.

The local request copy is not retained wholesale by the inspected SETUP
consumers. Stored audio objects retain their own references; screen workers
receive session state. Details and negative-control asynchronous commands are
in [escape analysis](request-escape-analysis.md). Response ownership and error
edges are in [cleanup](setup-cleanup-matrix.md).

## Remaining limits

Actual symbol binding/interposition, runtime loader policy, concurrent mutation
and replacement delegates are UNKNOWN and need separately authorized validation.
Atomic refcounts do not demonstrate adapter or callback thread safety. This
decision does not assert that stock cleanup is leak-free on every C++ exception.
Those limits do not prevent implementing scoped ownership for this recovered
input adapter. No code in include/hook/sidecar/target/mocks/tests was changed.
