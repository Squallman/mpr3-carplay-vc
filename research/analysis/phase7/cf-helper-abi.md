# P3695 CoreFoundation helper ABI

## Scope and evidence

Static offline analysis of the exact images identified in
[the main report](phase7-input-contracts.md#sources-and-method). Addresses below
are image virtual addresses, never runtime call addresses. `libairplay.so`
exports and implements a CFLite runtime. `dio_manager` imports these helpers
and has `libairplay.so` in DT_NEEDED; actual dynamic binding remains UNKNOWN.
Apple public header prototypes were not used.

Register use and instruction effects are PROVEN binary facts. The reconstructed
pointer/type meanings in this table are STRONG EVIDENCE. They are not recovered
source declarations. `w` arguments/results are 32 bits; do not substitute a
64-bit Apple CFIndex or CFTypeID merely because names match.

## Required and adjacent operations

All definitions in this table are in `libairplay.so`. Pointer returns use x0;
integer status/type/count returns use w0 unless explicitly stated otherwise.

| Function | Observed ABI | Semantics | Ownership | Evidence | Confidence |
|---|---|---|---|---|---|
| CFGetTypeID @ 0x769a0 | x0 object → w0 type | Wraps validated CFLGetTypeID @ 0x7a8b0; invalid → 0 | Borrowed input | Callee and typed-get callers | STRONG EVIDENCE |
| CFArrayGetTypeID @ 0x76ae0 | No consumed arguments → w0 = 1 | Array type selector | None | Tail callee 0x7ac00 | PROVEN |
| CFDictionaryGetTypeID @ 0x77550 | No consumed arguments → w0 = 5 | Dictionary type selector | None | Tail callee 0x7bbf0 | PROVEN |
| CFNumberGetTypeID @ 0x777d0; CFStringGetTypeID @ 0x77860 | No consumed arguments → w0 type | Number/string selectors; string type 7 | None | Exported wrappers and runtime class table | STRONG EVIDENCE |
| CFDictionaryGetValue @ 0x77700 | x0 dictionary, x1 key → x0 value/null | Hides internal CFL lookup status | Borrowed result | Wrapper; SETUP and /info callers | STRONG EVIDENCE |
| CFDictionaryGetTypedValue @ 0x88620 | x0 dictionary, x1 key, w2 type, x3 optional int32 error-out → x0 value/null | Null dictionary -6705; null key -6736; missing -6727; wrong type -6756; success 0 | Borrowed result | Callee; SETUP call 0x58f94 | STRONG EVIDENCE |
| CFDictionaryGetInt64 @ 0x883d0 | x0 dictionary, x1 key, x2 optional int32 error-out → x0 signed 64-bit value | Missing gives 0/-6727; conversion delegated to CFGetInt64 | Borrowed input | Callee; calls 0x59038, 0x590c8 | STRONG EVIDENCE |
| CFGetInt64 @ 0x869c0 | x0 object, x1 optional int32 error-out → x0 integer | Number, boolean, string and ≤8-byte data conversion paths; not a number-only getter | Borrowed input | 0x869c0–0x86d00 | STRONG EVIDENCE |
| CFArrayGetCount @ 0x76cd0 | x0 array → w0 count | Internal error hidden; initialized count 0 | Borrowed input | Wrapper and SETUP loop | STRONG EVIDENCE |
| CFArrayGetValueAtIndex @ 0x76d30 | x0 array, w1 index → x0 element/null | Internal status hidden | Borrowed result | Wrapper / CFLArrayGetValueAtIndex | STRONG EVIDENCE |
| CFArrayGetTypedValueAtIndex @ 0x877b0 | x0 array, w1 index, w2 type, x3 optional int32 error-out → x0 element/null | Missing -6727; wrong type -6756; success 0 | Borrowed result | Callee; SETUP and BonjourDevice_MergeInfo | STRONG EVIDENCE |
| CFDictionaryCreateMutableCopy @ 0x77650 | x0 allocator, w1 capacity, x2 source → x0 copy/null | Capacity ignored by wrapper; CFLDictionaryCreateCopy @ 0x7c250 inherits callbacks and copies key/value references | New owned reference | Callee; Bonjour call 0x75c08 | STRONG EVIDENCE |
| CFDictionaryCreateMutable @ 0x77560 | x0 allocator, w1 capacity, x2 key callbacks, x3 value callbacks → x0 dictionary/null | CFLDictionaryCreate @ 0x7bc00; allocator must be null; nonpositive capacity chooses default buckets | New owned reference | Callee; SETUP 0x58e38; display constructor | STRONG EVIDENCE |
| CFDictionaryCreate @ 0x775b0 | x0 allocator, x1 keys, x2 values, w3 count, x4 key callbacks, x5 value callbacks → x0 dictionary/null | Builds dictionary by inserting existing objects; failure cleanup | New owned reference; callback-dependent element retention | Wrapper | STRONG EVIDENCE |
| CFArrayCreateMutable @ 0x76c10 | x0 allocator, w1 capacity, x2 callbacks → x0 array/null | Capacity ignored; CFLArrayCreate @ 0x7ac10 requires null allocator | New owned reference | Callee and display constructor | STRONG EVIDENCE |
| CFArrayCreate @ 0x76af0 | x0 allocator, x1 values, w2 count, x3 callbacks → x0 array/null | Inserts existing objects | New owned reference; callback-dependent elements | Wrapper | STRONG EVIDENCE |
| CFArrayCreateMutableCopy @ 0x76c70 | x0 allocator, w1 capacity, x2 source → x0 array/null | Capacity ignored; copy inherits callbacks | New owned reference | Wrapper; Bonjour call 0x75c60 | STRONG EVIDENCE |
| CFArrayAppendValue @ 0x76e50 | x0 array, x1 value → w0 status | Tail to CFLArrayAppendValue @ 0x7b000 / InsertValueAtIndex @ 0x7aeb0; append index -1 | Retains through stored callback; stores original pointer | Callee; Bonjour 0x75f8c | STRONG EVIDENCE |
| CFDictionarySetValue @ 0x77770 | x0 dictionary, x1 key, x2 value → w0 status | Tail to CFLDictionarySetValue @ 0x7bfd0; retains new value before releasing replaced value | Retains through dictionary callbacks; stores original pointer | 0x7c048, 0x7c0a0; /info 0x4dcfc | STRONG EVIDENCE |
| CFRetain @ 0x76a40 | x0 object → x0 same object/null | CFLRetain @ 0x7a960; atomic refcount increment; constant object unchanged | Adds reference for dynamic object | Runtime and callers | STRONG EVIDENCE |
| CFRelease @ 0x76a50 | x0 object; no meaningful return consumed | CFLRelease @ 0x7a9d0; atomic decrement, finalizer/free at zero; constant/null no-op | Removes reference; containers release elements | Runtime class finalizers | STRONG EVIDENCE |
| CFNumberCreate @ 0x777e0 | x0 allocator, w1 number-type selector, x2 value-address → x0 number/null | Wraps CFL number creation | New owned reference | Callee | STRONG EVIDENCE |
| CFNumberGetValue @ 0x77830 | x0 number, w1 selector, x2 output → w0 boolean | Converts internal CFL status to success boolean | Borrowed input | Wrapper | STRONG EVIDENCE |
| CFDictionarySetInt64 @ 0x89f20 | x0 dictionary, x1 key, x2 integer → w0 status | Create number, set, release; allocation failure -6728; **discards SetValue failure and returns 0** | Temporary number released | 0x89f3c–0x89f78 | STRONG EVIDENCE |
| CFStringCreateWithCString @ 0x77ac0 | x0 allocator, x1 NUL-terminated bytes, w2 encoding → x0 string/null | strlen then copied-string creation | New owned reference | Callee; SETUP uses selector 0x08000100 at 0x58f40 | STRONG EVIDENCE |
| CFStringCreateWithBytes @ 0x779d0 | x0 allocator, x1 bytes, w2 length, w3 encoding, w4 external-representation flag → x0 string/null | Encoding-dependent conversion and copying | New owned reference | CString wrapper and callee | STRONG EVIDENCE |
| CFDictionaryGetCString @ 0x87be0 | x0 dictionary, x1 key, x2 destination, x3 capacity, x4 optional int32 error-out → x0 text pointer | Missing clears destination if capacity >0, error -6727; otherwise delegates CFGetCString | Destination owned by caller | Callee; SETUP 0x58efc | STRONG EVIDENCE |

Append/set expose real CFL status in w0. Treating these as Apple `void` APIs
would discard evidence-backed failure handling. Append can fail with -6740,
-6710 or -6728; SetValue with -6740, -6700 or -6728. These are helper statuses,
not authorized replacement AirPlay statuses.

## Callback objects and finalizers

Exported objects `kCFLArrayCallBacksCFLTypes` @ 0x159348,
`kCFLDictionaryKeyCallBacksCFLTypes` @ 0x159318 and
`kCFLDictionaryValueCallBacksCFLTypes` @ 0x1592f0 contain runtime retain/release
callbacks (0x7a9c0 / 0x7aa70). They pass the value in x1 to CFLRetain/Release;
allocator context is ignored. Copy inherits source callbacks; retention must
not be generalized to containers created with null/custom callbacks.

Runtime class table @ 0x1591b0 resolves array finalizer 0x7b310 to
CFLArrayRemoveAllValues @ 0x7b270 and dictionary finalizer 0x7c560 to
CFLDictionaryRemoveAllValues @ 0x7c490. These release contained values/keys
through stored callbacks before freeing storage. Constant CFString objects
have inline text at object+8, flag bit 0 at byte+2 and sentinel retain count.

Additional exported CFString formatting/conversion helpers exist (including
CFStringCreateWithFormat, CFStringGetCString, CFStringGetCStringPtr,
CFString2CString). Their complete variadic/conversion contracts are UNKNOWN
in this phase and unnecessary for the minimal SETUP operation set. Dynamic
/info also uses CFBinaryPlistV0Create @ PLT; its full ownership ABI is outside
the adapter primitive contract. This is an inventory of the requested paths,
not a claim that every CFLite export has been reconstructed.

Atomic retain/release does not prove thread-safe container mutation or target
adapter initialization. See [readiness](cf-adapter-readiness.md).

## Remaining direct CF calls in the inspected /info paths

The direct-call inventory of Setup, display creation/dispatch, dynamic /info,
feature retrieval and response serialization also includes the operations
below. They are not additional requirements for minimal SETUP filtering.

| Function | Observed ABI | Semantics | Ownership | Evidence | Confidence |
|---|---|---|---|---|---|
| CFEqual @ 0x76a60 | x0 object, x1 object → w0 boolean | Tail to runtime equality, including content-based strings | Borrowed inputs | PlatformCopyProperty call 0x4820c; string class 0x7a5a0 | STRONG EVIDENCE |
| CFArrayContainsValue @ 0x76e80 | x0 array, x1 packed range (low32 start / high32 length), x2 value → w0 boolean | Wrapper unpacks to internal start/end; **not assumed Apple 64-bit CFRange ABI** | Borrowed inputs | /info 0x4adf0; wrapper 0x76e80–0x76e8c | STRONG EVIDENCE |
| CFDictionarySetCString @ 0x87c60 | x0 dictionary, x1 key, x2 bytes, x3 length (-1 selects NUL termination) → w0 status | Creates string with observed selector 0x08000100; null text becomes empty; creation failure -6700; discards SetValue failure | Temporary string released after insertion | Callee; /info calls 0x4ad3c, 0x4ada4, 0x4ae84 | STRONG EVIDENCE |
| CFNumberCreateInt64 @ 0x887a0 | x0 integer → x0 number/null | Selects smallest suitable number representation | Owned created result | Callee; property callback 0x4828c | STRONG EVIDENCE |
| CFObjectGetPropertyInt64Sync @ 0x86d00 | Forwards x0–x5 to CFObjectCopyProperty; x6 saved as conversion error-out; x0 return integer | Copy property, convert via CFGetInt64, release copied object; null yields 0 | Copied property released | AirPlayGetFeatures call 0x49d6c; exact meanings of all forwarded parameters UNKNOWN | STRONG EVIDENCE (effects); UNKNOWN (complete prototype) |
| CFBinaryPlistV0Create @ 0x80f00 | Observed caller x0 dictionary, x1 length-out, x2 null → x0 byte buffer/null | Binary plist encoder | HTTP body handoff observed; complete buffer/error ownership UNKNOWN | Response helper 0x49990–0x499b8 | STRONG EVIDENCE (caller); UNKNOWN (complete contract) |

Direct SETUP CF calls are covered by the required-operation table. The
display helper additionally calls retaining constructors/append/release.
Dynamic /info adds the operations in this table; indirect C++ property
callbacks and their complete CF conversion helper inventory are not claimed
exhaustively recovered.
