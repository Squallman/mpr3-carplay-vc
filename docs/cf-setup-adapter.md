# P3695 CFLite SETUP adapter

**READY_FOR_CF_ADAPTER** is now **IMPLEMENTED OFFLINE/TARGET-CONTRACT LAYER**.
The adapter operates through the recovered P3695 ABI; host validation uses a
dedicated fake runtime. Actual process symbol binding and vehicle execution
remain **UNKNOWN**. The exported SETUP entrypoint remains pass-through and
does not call this adapter or extract type111.

## Evidence and low-level ABI

[Phase 7 helper ABI](../research/analysis/phase7/cf-helper-abi.md),
[key map](../research/analysis/phase7/setup-key-map.md) and
[Phase 8 readiness](../research/analysis/phase8/cf-adapter-readiness-v2.md)
provide the implementation basis. Register facts are **PROVEN**; reconstructed
helper signatures and the bounded stock request-lifetime contract are
**STRONG EVIDENCE**. Historical platform findings supply no implementation
constants. No historical research reports were changed.

[`cflite_abi.hpp`](../target/include/mpr3/target/cflite_abi.hpp) declares only
opaque project types and these function-pointer shapes:

| Functions | Recovered shape |
|---|---|
| CFArrayGetTypeID, CFDictionaryGetTypeID | No arguments; unsigned 32-bit result |
| CFDictionaryGetTypedValue | Dictionary, string key, unsigned 32-bit type, signed 32-bit error-out; borrowed pointer result |
| CFDictionaryGetInt64 | Dictionary, string key, signed 32-bit error-out; signed 64-bit result |
| CFArrayGetCount | Array; 32-bit count result |
| CFArrayGetTypedValueAtIndex | Array, 32-bit index, unsigned 32-bit type, signed 32-bit error-out; borrowed pointer result |
| CFDictionaryCreateMutableCopy | Opaque allocator, 32-bit capacity, source dictionary; owned dictionary result |
| CFArrayCreateMutable | Opaque allocator, 32-bit capacity, opaque callbacks; owned array result |
| CFArrayAppendValue | Mutable array, object; signed 32-bit status |
| CFDictionarySetValue | Mutable dictionary, string key, object; signed 32-bit status |
| CFStringCreateWithCString | Opaque allocator, NUL-terminated text, unsigned 32-bit selector; owned string result |
| CFRetain | Object; same-object pointer result adding a reference |
| CFRelease | Object; no consumed return |

Count/index/capacity use statically validated 32-bit `int`; integer extraction
uses 64-bit `long long`. Pointers must be 64 bits. These are recovered register
shapes, not Apple CFIndex/CFTypeID declarations or object layouts. The only
resolved data export is `kCFLArrayCallBacksCFLTypes`; its layout stays opaque.
All constructors use a null allocator and zero capacity. No firmware address
is used for lookup or invocation.

## Resolution and context

`CFLiteApi` has const members and is usable only when all 13 function pointers
and the retaining-array callback object are present. `resolveCFLiteApi` asks
`ICFLiteSymbolLookup` for the functions in the table's order, then the callback
object, exactly once each. It reports the first missing exact export name;
failure contains no API table. Function lookup and data lookup are distinct.
There is no guessed fallback, firmware path, library loading or mutable global
function table.

Optional `PosixCFLiteSymbolLookup` uses RTLD_DEFAULT. Its source alone performs
POSIX data-pointer/function-pointer conversions. Function-pointer retyping in
the generic resolver preserves the recovered signature. Enable it explicitly
with `MPR3_ENABLE_POSIX_CFLITE_RESOLVER`; it defaults OFF and never binds
firmware libraries in the host tests. Its presence does not prove binding in
the actual MPR3 process.

`CFLiteSetupContext::create` validates the API, obtains type selectors through
the two type-ID functions and creates content-equal dynamic `streams` and
`type` strings. The selector `recoveredCStringEncodingSelector` is
`0x08000100`, using the inspected P3695 CString creation path; it is not named
as an Apple encoding. No `streamConnectionID` key is needed or created.

The result carries either a shared const context or a local typed failure
(incomplete API, invalid selectors, key creation, host allocation). Successful
contexts own the two created strings and release both on final destruction.
Requests and descriptors share context ownership, keeping keys and the API
alive. The resolved code/data and any injected provider must remain valid
for those lifetimes. There is no singleton or cache.

## Request and descriptor ownership

`CFSetupRequest::borrowed` creates a wrapper that neither retains nor releases
the caller's dictionary. The caller must keep that dictionary alive while
using the wrapper. `withStreams()` constructs an internally owned wrapper;
its destructor releases the copied dictionary once. Ownership flags and the
adopting constructor are private. Owning wrappers cannot be copied.

`streams()` obtains a borrowed typed array, reads its 32-bit count and performs
typed dictionary lookup at every index. It preserves order and returns a full
collection or `nullopt`; malformed entries, failed retains or allocation
exceptions unwind the entire enumeration. It never releases borrowed array
elements directly.

Each `CFSetupDescriptor` takes its own explicit retain and releases it once.
Its identity is the original dictionary pointer. `type()` extracts the full
signed Int64 with the dynamic `type` key and returns `nullopt` on helper error;
valid zero, negative and wide integers remain distinct. Descriptors are never
mutated. Shared descriptors remain valid after enumeration, original request
destruction and filtered-request destruction because they own both their raw
object reference and their context.

## Shallow copying and cleanup

`withStreams()` first verifies every descriptor is a `CFSetupDescriptor` from
the exact same context. An arbitrary `identity()` token, null descriptor or
another context is rejected before target allocation.

1. Make a mutable shallow dictionary copy, inheriting the source callbacks.
2. Create an empty mutable array with the recovered retaining callback object.
3. Append original descriptor pointers in the supplied order, checking every status.
4. Set only `streams` in the copy, checking the status.
5. Return an owned request wrapper; drop the local array reference after the
   dictionary retains the replacement array.

| Resource | During construction | After success | On failure |
|---|---|---|---|
| Original request/array | Borrowed | Unchanged and caller-owned | Unchanged and caller-owned |
| Descriptor wrapper | One explicit dictionary retain | Shared wrapper owns it | Partial wrappers unwind |
| Mutable request copy | Local owned guard | Owned request wrapper | Guard releases it |
| Replacement array | Local owned guard; appends retain elements | Copied dictionary owns it | Guard releases it and appended elements |
| Dynamic keys | Context-owned | Context shared by wrappers | Partial context creation releases its keys |

RAII guards precede host wrapper allocation. If object allocation or a
shared-pointer control block fails after a retain, the guard or wrapper
destructor releases that reference. Failure after SetValue also releases both
local references and the dictionary's retained array. Original opaque fields
and preserved descriptor identities remain unchanged. Empty replacement arrays
are supported structurally; this does not assert target acceptance of any
particular future type111 exchange.

Phase 8 supports releasing a temporary request copy immediately after the
inspected stock SETUP path returns: persistent consumers copy scalar values or
retain the objects they store. This adapter does not call stock, manage its
response or repair exceptional stock response ownership. Caller-owned response
data must remain untouched in later integration.

## Core integration and fail-open behavior

`filterTargetSetupRequest(context, rawRequest)` creates a borrowed wrapper and
calls the existing `splitSecondaryDescriptors` unchanged. The exact comparison
to 111 remains solely in core. The result exposes `parseFailed`, original or
filtered selection, the selected raw dictionary, and ordered retained secondary
descriptors. A filtered dictionary is available only through a validated target
request instance.

Missing context/API/keys, malformed input, extraction failures, incompatible
descriptors, target construction/mutation failures and host allocation failures
select the original raw request, with no partial secondary collection and no
invented AirPlay status. No-111 input also selects the original, successfully,
without copying it. This is an offline forwarding decision, not an invocation
of an original handler or secondary controller.

## Validation and remaining boundary

`./scripts/build-local.sh` still runs only core, mocks and host tests.
`./scripts/build-target-contracts.sh` additionally builds the static adapter,
isolated ABI consumer and fake-runtime tests. `./scripts/check-target-abi.sh`
cross-compiles only dependency-free SETUP/CFLite objects for AArch64/Linux;
the STL adapter is not cross-linked without a sysroot. No shared hook, install
rule, packaging or deployment mechanism exists. All three build options default
OFF in CMake; the target-build script explicitly opts into contracts only.

The dedicated fake models identities, type selectors, content-equal strings,
retaining dictionaries/arrays, shallow copies, Int64 extraction and injected
failure statuses. Test-only allocation interception exercises object/control
block failures. It does not reproduce target layout, all CFLite conversions,
threading or real runtime binding. Production objects contain no fake runtime.

Validation here: host **34 groups / 687 assertions**; target contracts
**13 / 106**; CFLite adapter **25 / 774**. All three also pass combined
ASan/UBSan. Both optional POSIX sources compile while tests use fakes. Apple
Clang 21 produces ELF64 relocatable AArch64 SETUP and CFLite consumers without
a sysroot; exported SETUP remains an unmangled global function. CMake is not
installed on this host, so direct-script builds were exercised.

The C entrypoint source is unchanged and still forwards original pointers and
exact stock status through the pass-through bridge. Advertisement ownership,
client-valid second descriptor/UUID, iOS type111 causality, secondary service
ABIs, runtime binding/policy, concurrency and output restoration still block
actual target interception. This adapter supplies none of those missing facts.
