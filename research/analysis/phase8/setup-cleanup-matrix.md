# SETUP cleanup and error ownership

**STRONG EVIDENCE:** wrapper-owned copies have deterministic cleanup on ordinary
stock status returns. Scoped cleanup is also required if the original unwinds.
This matrix is an implementation contract, not implemented behavior.

| Path | Temp request release | Temp streams release | Stock response | Secondary descriptor retain | Notes |
|---|---|---|---|---|---|
| Key lookup/type conversion cannot safely classify request | No copy created, or release partial copy | Release any locally owned array | Untouched; forward original request once | None needed | Fail open; preserve stock interpretation |
| Mutable copy / array allocation fails | Release acquired copy | Release acquired array | Untouched; forward original once | Only container-owned references, released with container | Do not invent a target error code |
| Append / replacement SetValue fails | Release copy | Release local array and its retained elements | Untouched; forward original once | Release only explicit owned references | Check actual CFL status; helpers are not Apple void APIs |
| Successful forwarding with replacement | Release after original returns | Drop local array reference after successful SetValue; copy owns remaining array reference | Original writes/returns it; wrapper does not release it | Reused stock descriptors owned by retaining containers | Descriptor identity and order preserved |
| Null session / initial stock allocation error | Same after original returns | Same | Setup returns -6740 at 0x5a088 or -6728 at 0x5a090; no output clearing | Same | Preserve exact original status |
| Missing/empty streams | Same | Same | May reach platform handling/success; not inherently malformed | Same | Do not impose a new mandatory streams rule |
| Wrong descriptor type / type handler error | Same | Same | Internal response release at 0x591dc–0x591e0 on failing paths; output not generally written | Same | Preserve actual stock branch/status |
| Unsupported stream type, including111 | Same | Same | Stock handling remains unsupported; ordinary failure cleanup applies when error returned | Same | No new type111 acceptance contract |
| Screen response dictionary / screen setup failure | Same | Same | New stream response reference released on 0x59c80 cleanup; aggregate response on common error edge | Same | Screen cleanup and temporary key wiping remain stock-owned |
| Worker-start error | Same | Same | 0x5a070 status → response descriptor release 0x5a07c → 0x59c88 cleanup | Same | No queued request pointer in worker |
| Original throws/unwinds | Release by future scoped guard during unwind | Release locally owned array by scoped guard | Stock exceptional response balance not universally established | Container finalizers release owned elements | Preserve unwinding; do not translate into guessed status |

## Exact response convention

SETUP creates its aggregate response at 0x58e38. On success,
0x597ac–0x597b8 transfers that reference through nonnull responseOut. It does
not clear *responseOut at entry and generally does not write it on error.
The stock caller initializes its local output to null at 0x4c5a0, invokes
SETUP at 0x4c7bc, serializes on success at 0x4c824 and releases request and
nonnull response at 0x4c83c/0x4c850. A nonzero result takes 0x4cd70 to HTTP
400 and the same ordinary cleanup.

A successful call with null responseOut bypasses both transfer and local
aggregate release in the inspected stock code. A wrapper must preserve this
argument and must not try to repair that stock behavior. Exceptional stock
response cleanup also remains a stock limitation; it does not authorize a
wrapper to release a caller-owned response or preinitialize the output.

## Reference arithmetic for the copy

New mutable copy: one local reference. New retaining streams array: one local
reference. Each successful append adds one container reference to the original
descriptor. Successful SetValue adds one array reference to the copy and
releases its previous streams value. Drop the local array reference after set;
release the copy after stock return. Its dictionary finalizer releases the
replacement array, whose finalizer releases descriptors. The original request
and original streams keep their own graph throughout. No separate descriptor
retain is required merely to reuse it in that array.

See [escape analysis](request-escape-analysis.md) and
[adapter contract](cf-adapter-readiness-v2.md).
