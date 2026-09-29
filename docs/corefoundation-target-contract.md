# CoreFoundation target adapter contract

**READY_FOR_CF_ADAPTER — IMPLEMENTED OFFLINE/TARGET-CONTRACT LAYER.**
Phase 7 recovered the actual P3695 CFLite helper ABIs and key text; Phase 8
closed the bounded stock SETUP lifetime contract. The dedicated adapter now
implements `ISetupRequest` / `ISetupDescriptor` under `target/`, with ownership
and failure tests against a separate fake CFLite runtime. Runtime binding
validation and vehicle interception remain false. The exported compile-only
SETUP entrypoint remains pass-through.

Sources: [helper ABI](../research/analysis/phase7/cf-helper-abi.md),
[key map](../research/analysis/phase7/setup-key-map.md),
[request escape](../research/analysis/phase8/request-escape-analysis.md),
[parser ownership](../research/analysis/phase8/setup-request-origin.md), and
[readiness v2](../research/analysis/phase8/cf-adapter-readiness-v2.md).
Detailed implementation and validation: [CFLite adapter](cf-setup-adapter.md).

| Required operation | Evidence | ABI known? | Implement now? |
|---|---|---|---|
| Obtain typed streams array | STRONG EVIDENCE: CFDictionaryGetTypedValue; PROVEN exact `streams` key text | Recovered pointer/w32/error-out shape | Implemented |
| Enumerate dictionaries in order | STRONG EVIDENCE: CFArrayGetCount / CFArrayGetTypedValueAtIndex | Recovered 32-bit count/index/type selectors | Implemented |
| Read wide descriptor type | STRONG EVIDENCE: CFDictionaryGetInt64; PROVEN `type` key text | Recovered signed Int64 plus int32 error-out | Implemented |
| Create content-equal dynamic keys | STRONG EVIDENCE: CString creation, string equality/hash | Null allocator, text, recovered selector 0x08000100 | Implemented; owned keys |
| Shallow request copy | STRONG EVIDENCE: mutable copy inherits retaining source callbacks | Null allocator, 32-bit capacity, source pointer | Implemented |
| Replacement streams array | STRONG EVIDENCE: exported kCFLArrayCallBacksCFLTypes retains elements | Null allocator, 32-bit capacity, opaque callbacks | Implemented |
| Reuse original descriptor identities | STRONG EVIDENCE: append/set retain original pointers | Signed int32 mutation statuses | Implemented; every status checked |
| Preserve opaque request data | STRONG EVIDENCE: shallow dictionary copy replaces only streams | Copy/SetValue recovered | Implemented |
| Retain/release through failure and unwinding | STRONG EVIDENCE: runtime finalizers and Phase 8 stock consumers | Recovered CFRetain/CFRelease | Implemented with local guards and explicit descriptor retains |
| Preserve stock response ownership | STRONG EVIDENCE: separate response/cleanup paths | Canonical three-argument SETUP ABI | Existing pass-through only; adapter never calls stock |

The extra key `streamConnectionID` is proven but unnecessary here and is not
created. No image address or Apple type/header is used. Callback layouts stay
opaque; retaining behavior is recovered rather than invented.

Borrowed requests do not retain/release the caller's dictionary. Copied requests
own their dictionary. Descriptors explicitly retain their raw dictionary and
share the context; they can outlive enumeration and either request. A future
bounded stock forwarding path may release its temporary request after stock
returns, as established by Phase 8; this branch does not wire that path.

All adaptation failures select the original request unchanged, without a target
error code or partial secondary collection. Type110 identity and unfamiliar
non111 values are preserved. The comparison to 111 remains in core.

**UNKNOWN:** actual process symbol binding, loader policy, concurrent container
mutation, replacement delegate behavior and target thread/lifetime validation.
These require separately authorized runtime evidence. Advertisement schema,
callback reference accounting and secondary service contracts remain separate
blockers. Fake-runtime success is not target runtime validation.
