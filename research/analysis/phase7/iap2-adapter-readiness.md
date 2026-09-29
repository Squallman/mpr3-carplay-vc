# iAP2 capability adapter readiness

**MORE_RE_REQUIRED**, independently of CF and advertisement readiness.

| Requirement | Result | Classification |
|---|---|---|
| Named Wireless producer/composer path | dio addIdentificationInformation → typed field +0x380 → composer selector3 | STRONG EVIDENCE |
| Parent/component structure | Parent24, fields0/1/2/4, order and presence tests recovered | STRONG EVIDENCE |
| Child17 byte representation | Existing raw-ID writer + zero-payload constructor + generic nesting preserve it | STRONG EVIDENCE; GENERICALLY_EXPRESSIBLE |
| Typed insertion point | Inspected Wireless emitter is fixed; no arbitrary-child API recovered | UNKNOWN (safe supported insertion mechanism) |
| Ownership / lifecycle | Local buffer copying visible; safe external mutation and IPC/exception lifetime incomplete | UNKNOWN |
| Intended capability relevance | Historical parser uses parents20/21; acceptance and ThemeAssets meaning for P3695 parent24 not established | UNKNOWN |
| iAP2→AirPlay / type111 dependency | No P3695-specific causal chain closed | UNKNOWN |

Required next evidence: parent24's protocol/client interpretation; whether the
historical gate applies to this target schema; an insertion interface that
preserves all stock identification fields and buffer lifetimes; complete IPC
handoff and exception cleanup; direct relevance to the intended capability.
Generic buffer mechanics do not supply a recovered generated C++ adapter ABI.

The LocationInformation child17 branch must not be reused as Wireless support.
Neither adding a numeric child nor modifying extdevconfig.json was performed.
