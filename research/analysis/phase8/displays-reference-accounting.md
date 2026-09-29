# Displays callback reference accounting

Result: **LEAK_SUSPECT**, **STRONG EVIDENCE** of a net extra reference on the
inspected ordinary stock /info path. No cache, ownership transfer or
autorelease-equivalent operation was found to explain it as balanced. This is
static accounting, not an observed runtime leak or permission to repair stock.

## Normal success

| Step | Site | Array reference delta | Count |
|---|---|---|---|
| Fresh retaining array construction | dio createDisplaysDictionary 0x1d9be0 | +1 | 1 |
| Return through getDisplays | call 0x1db608, result x19 at 0x1db60c, return 0x1db62c | 0 | 1 |
| Additional retain when incoming error is zero | infoRequestDisplays 0x1db720 | +1 | 2 |
| Manager map / infoResponse / generic callback return | 0x1df49c / 0x1df610 / 0x1df750 | 0 | 2 |
| Insert into /info dictionary | airplay 0x4dcfc; retaining value callback | +1 | 3 |
| Consumer releases callback result | 0x4dd04 | -1 | 2 |
| /info dictionary destruction after serialization | 0x4b1c4; dictionary finalizer releases value | -1 | 1 |

The dictionary/array finalizers at 0x7c560/0x7b310 reach their stored release
callbacks. Dictionary finalization contributes the last -1 above; it is not
an additional release of the initial constructor reference. The dictionary
inside the array is separately balanced by append-retain and local release
0x1d9d38/0x1d9d40; that does not cancel the array's extra reference.

## Why a hidden transfer was not assumed

getDisplays creates a fresh array for each call. Its entire normal body returns
it without storing it in manager state and without dropping the constructor
reference. infoRequestDisplays returns the same pointer. serverCopyProperty
map invocation and std::string destructors do not release it. The generic
producer initializes its local error to zero at 0x1df768. The /info consumer
releases exactly once after insertion. Later destruction of the response's
dictionary is included in the table. No second caller release or retained cache
owner was recovered on this chain.

## Other paths

| Path | Accounting / behavior | Confidence |
|---|---|---|
| Array/dictionary construction allocation failure | DIO helper releases partial owned objects and returns null; getDisplays local error reports failure only locally; incoming error reference is not written | STRONG EVIDENCE |
| Null callback result | CFRetain(null) is a no-op; /info omits property; no array reference to balance | STRONG EVIDENCE |
| Nonzero incoming error with nonnull array | infoRequestDisplays skips extra retain; consumer insertion/release/destruction would balance the constructor reference; ordinary generic callback instead starts error=0 | STRONG EVIDENCE; conditional path |
| Generic property callback reports error | PlatformCopyProperty releases any nonnull returned object at 0x482b0 before returning null; this does not by itself cancel a producer's two references | PROVEN (consumer release); producer-specific failure ownership must be respected |
| SetValue insertion fails before retaining value | Consumer ignores status and still releases once; response never owns value, leaving the original array reference | STRONG EVIDENCE from CFL SetValue failure edges |
| Append/set inside descriptor builder fails | Builder discards statuses; it may return partial/empty structures with success | STRONG EVIDENCE; no complete-schema guarantee under failure |
| Allocation/exception outside these ordinary edges | C++ unwind and runtime rebinding do not establish universal balance | UNKNOWN complete behavior |

No autorelease mechanism is visible on these paths; no intentional-transfer
explanation was established. A future adapter must define its own result
reference convention and preserve stock ownership conservatively. It must not
double-release the original result to “fix” this arithmetic. Whether an active
runtime binding differs, or the extra reference is an actual leak, requires
separately authorized observation.
