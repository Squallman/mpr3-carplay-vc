# CoreFoundation SETUP adapter readiness

**MORE_RE_REQUIRED** for a real integrated P3695 ISetupRequest/ISetupDescriptor
adapter. The required helper operation set and key retrieval mechanism are
recovered; the remaining barrier is complete request/escape ownership across
target forwarding paths, not an invented missing CF key.

| Requirement | Result | Evidence classification |
|---|---|---|
| Required helper symbol/register ABIs | Recovered from definitions and multiple callers; append/set return actual CFL status | STRONG EVIDENCE |
| streams/type keys | Exact static objects and inline text, consumed by Setup; content-equal dynamic key retrieval established | PROVEN (objects/use); STRONG EVIDENCE (retrieval) |
| Array iteration / integer extraction | Narrow count/index/type arguments; borrowed elements; Int64 conversion with explicit error-out | STRONG EVIDENCE |
| Mutable dictionary copy / new array | Shallow reference copy, inherited callbacks, null allocator and retaining CFL array callbacks | STRONG EVIDENCE |
| Append/set/retain/release | Callee retain/release and finalizer mechanics recovered | STRONG EVIDENCE |
| Received request callback tables | Mutable copy inherits them; exact received-request construction and all callback-table paths not exhaustively closed | UNKNOWN (complete coverage) |
| Forwarding lifetime / all wrapper paths | Stock caller releases request after success/error; inspected consumers copy scalars or retain objects; all delegate/exception/deferred paths not closed | STRONG EVIDENCE (ordinary caller); UNKNOWN (universal lifetime) |

The primitive construction contract is concrete: resolve the exported CFL
helpers/callback objects; create content-equal `streams`/`type` CFString keys
using the observed CString ABI/encoding selector; obtain typed borrowed array
and dictionaries; use stock Int64 conversion/error behavior; shallow-copy the
request; create a retaining array; append original descriptor pointers; replace
only streams; check allocation/mutation failures and release locally acquired
references. Never modify descriptor objects, release borrowed values, use
image addresses at runtime or reinterpret host interfaces as CF pointers.

Before implementation, close the request parser's callback tables and lifetime
contract through platform/session delegates and cold exceptional edges; verify
that any asynchronously held request contents are retained/copied. Define
cleanup on every adapter construction/mutation failure, exceptional original
call and ordinary status return. Stock's immediate request release is strong
support, not a complete audit. Retain/release atomics do not prove callback
thread safety. No adapter was implemented in Phase7.

See [helper ABI](cf-helper-abi.md), [keys](setup-key-map.md),
[shallow copy](cf-shallow-copy-contract.md), [response ownership](setup-response-ownership.md).
