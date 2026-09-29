# Phase 7 — CoreFoundation / advertisement / iAP2 input contracts

## Executive summary

Static offline P3695 analysis recovered the required CFLite helper register
ABIs, exact SETUP keys, retaining shallow-copy mechanics, the generic displays
delegate dispatch, all stock display-helper insertions and the named
WirelessCarPlay serializer. No implementation or firmware was changed.

Three corrections matter: SETUP's unnamed type-110 integer key is
`streamConnectionID` @ 0x107330; displays uses generic delegate+0x18 rather
than a direct method slot; and P3695's named WirelessCarPlay composer emits
parent24 with children0/1/2/4, rather than historical MHI3 parent21/child17.
The stock display helper emits a UUID, not an Audi MMI display-name field;
the familiar dimensions occur in disabled test-HMI configuration and do not
establish live geometry.

The CF operation set is recovered, but full request escape/callback ownership
is not yet closed. Advertisement still needs a valid second descriptor and
balanced result lifetime. Child17 is GENERICALLY_EXPRESSIBLE as a TLV, without
proof of the intended P3695/client capability. All three integrated adapter
readiness decisions remain MORE_RE_REQUIRED.

## CoreFoundation ABI result

STRONG EVIDENCE: libairplay.so contains the CFLite definitions, rather than
relying on assumed Apple public prototypes. Required functions include
CFDictionaryGetTypedValue, CFDictionaryGetInt64, CFArrayGetCount,
CFArrayGetTypedValueAtIndex, CFDictionaryCreateMutableCopy,
CFArrayCreateMutable, CFArrayAppendValue, CFDictionarySetValue,
CFStringCreateWithCString, CFRetain/CFRelease and exported CFL type callbacks.
Count/index/type selectors are observed in 32-bit registers. Append/set expose
CFL status in w0; CFDictionarySetInt64 discards underlying mutation failure.
See [helper ABI inventory](cf-helper-abi.md).

## SETUP key result

PROVEN constant objects, inline literals and helper call arguments establish
`streams` @ 0x106610, `type` @ 0x107128 and `streamConnectionID` @ 0x107330.
The earlier 0x1072c8 approximation was incomplete effective-address arithmetic.
Content-based CFLString equality/hash gives STRONG EVIDENCE for creating keys
by exact recovered text without runtime firmware addresses.
See [key map](setup-key-map.md).

## Shallow-copy / ownership result

STRONG EVIDENCE: mutable copy inherits callbacks and reuses original key/value
pointers; CFL type array append and dictionary set retain the same objects;
container finalizers release them. BonjourDevice_MergeInfo supplies an actual
copy/append/set/release example. The stock Setup caller releases request and
returned response after ordinary success/error; Setup transfers a response
reference on ordinary success and does not generally clear responseOut on
failure. Full parser callback-table, deferred/delegate escape and exceptional
ownership coverage remains UNKNOWN. This separates recovered primitives from
an unconditional safe hook-lifetime claim.
See [copy contract](cf-shallow-copy-contract.md) and
[response ownership](setup-response-ownership.md).

## Delegate layout result

The 0xa0-byte producer record is accounted for qword by qword. STRONG EVIDENCE:
displays dispatches through **delegate+0x18 / server+0x30**, generic
s_serverCopyPropertyCallback @ dio 0x1df750. The manager's property registry
binds exact string `displays` to infoRequestDisplays @ 0x1db6e0. It is not a
direct delegate member. Session/audio/screen callbacks and contexts are mapped;
unused slot0x08 and complete downstream audio invocation behavior remain open.
See [delegate layout](airplay-server-delegate-layout.md).

## Display callback ABI result

STRONG EVIDENCE: generic callback receives server, CFString key, qualifier,
int32 error pointer and manager context, returning an object in x0. The bound
method receives `this` in x0 and nonnull int32 error-reference address in x1,
returning array/null. An extra retain follows an owned array construction;
only one returned reference is released by the /info consumer. This apparent
imbalance and allocation error propagation are unresolved; do not declare a
balanced exactly-+1 callback contract. See [callback ABI](info-displays-callback-abi.md).

## Stock descriptor result

PROVEN insertion inventory: features, maxFPS, widthPixels, heightPixels,
widthPhysical, heightPhysical, conditional primaryInputDevice and uuid.
UUID is `e5f7a68d-7b0f-4305-984b-974f677a150b`. Descriptor features derive
from service state, maxFPS uses configuration/default60. Active geometry is
UNKNOWN; 1540×720 / 235×110 belong to disabled test-HMI configuration.
Name/modes/role/rotation/transport/context fields are ABSENT FROM THIS HELPER,
which does not prove client requirements. Minimum valid second descriptor
remains UNKNOWN. See [complete helper inventory](stock-display-descriptor-complete.md).

## AirPlay /info result

STRONG EVIDENCE: /info URI dispatch → AirPlayCopyServerInfo → dynamic property
enumeration → generic property delegate → manager displays binding → new
array → SetValue under `displays` object 0x104938 → release callback reference
→ binary plist HTTP body → release response dictionary. Factory constructs
the dynamic implementation. No displays-result cache is visible on this path;
manager state and concurrency remain unresolved. See [full path](airplay-info-response-path.md).

## Feature advertisement result

PROVEN: AirPlayGetFeatures ORs floor 0x6104040280 (bits7,9,18,26,32,37,38)
with the server property. No new semantic bit assignment. Features and displays
are populated in the same /info response through separate paths; a count/mask
dependency was not found. ExtendedFeatures includes vocoderInfo in the
inspected construction. See [feature report](airplay-feature-advertisement.md).

## P3695 WirelessCarPlay serializer result

STRONG EVIDENCE: named identification field+0x380 → composer selector3 →
parent24 containing child0 UInt16 identifier, child1 NUL-terminated name,
child2 supportsIAP2Connection and child4 supportsCarPlay zero-payload markers.
dio's Wireless producer sets one configured component (ID3/name from config)
and both flags when the supported-technologies branch is selected.
See [serializer reconstruction](p3695-wireless-carplay-component.md).

## Child-17 result

**GENERICALLY_EXPRESSIBLE** at the existing generic buffer layer: the raw-ID
writer, payload-size-zero constructor and nested byte-copy routine together
can represent/preserve a header-only child17. The stock typed Wireless branch
does not expose it. Explicit P3695 child17/18/20 elsewhere belongs to
LocationInformation parent22. No relevant emitted Wireless child17 or client
meaning is established. See [capability classification](p3695-param17-capability.md).

## MHI3 differential

The local historical ref documents MHI3 parent21 children17/18/20 and an
iOS26.1 ThemeAssets/negotiated-features chain. It does not establish meaning
or acceptance for P3695 parent24. Children18/20 are not assigned guessed
semantics. Historical enabledFeatures/altScreen/uiContext/cornerMasks/
focusTransfer are not imported into target constants.
See [differential](mhi3-p3695-capability-diff.md).

## Capability-state bridge

UNKNOWN: no P3695 identification→AirPlay causal IPC/state edge was closed.
A real DSI ServiceConfiguration+0x60 → manager+0x158 → descriptor feature
path was recovered, independently of identification. Its original writer,
bit semantics and any identification dependency remain missing.
See [candidate state audit](capability-state-bridge.md).

## Updated iOS type-111 chain

Identification→internal state, state→feature advertisement,
features→displays dependency and second display→iOS type111 each remain
UNKNOWN independently. Displays callback→/info wire construction has
STRONG EVIDENCE. Historical child17→ThemeAssets→proposed/negotiated altScreen
is historical-only provenance. Stock P3695 acceptance of111 is DISPROVEN.
See [arrow-by-arrow chain](ios-type111-trigger-chain-v2.md).

## Implementation readiness

Readiness values describe complete adapters, not just recovered subcontracts.

| Component | Before Phase 7 | After Phase 7 | Ready to implement? |
|---|---|---|---|
| CoreFoundation SETUP adapter | Helper/key/ownership boundaries unresolved | Helper ABI, exact keys, shallow-copy/retaining mechanics recovered; complete received-request/deferred ownership open | MORE_RE_REQUIRED |
| AirPlay advertisement adapter | Displays slot/full schema unknown | Generic slot/ABI and all helper fields recovered; result balance and valid second descriptor open | MORE_RE_REQUIRED |
| iAP2 capability adapter | Historical17 transfer unsupported | Native parent24/children0,1,2,4 recovered; child17 generically encodable; safe integration/client relevance open | MORE_RE_REQUIRED |
| SETUP type111 hook | Pass-through target contract only; stock rejects111 | Input primitives improved; causal gates, full lifetime and secondary descriptor still blocked | No |
| Secondary pipeline | Core abstraction only; target constructor/render ABI incomplete | Unchanged readiness; callback-copy evidence does not recover complete pipeline ABI | No |
| Displayable adapter | LayerConfig/IPTE evidence; creation/lifecycle contract incomplete | Unchanged; active endpoint/safe ID not proven | No |
| VideoEncoding adapter | COMM/proxy and restoration blockers | Unchanged; output restoration/concurrent ownership unresolved | No |

Independent decisions and evidence requirements:
[CF](cf-adapter-readiness.md),
[advertisement](advertisement-adapter-readiness.md),
[iAP2](iap2-adapter-readiness.md).

## Sources and method

Firmware root: ignored `.local-research/mpr3/P3695/extracted/files/`.

| Image (relative to firmware root) | SHA-256 |
|---|---|
| smartphone_integration/lib/libairplay.so | 67be0a5639a8e4bb4f55677f26aeb3eadae3d4baf68180db3bcaeb77536b5c28 |
| smartphone_integration/bin/dio_manager | 1b3257c53c795f2477cd7ce0abc1e2ad22d43d8b001a4b265d74d6f124e2b490 |
| iap2/lib/libesoiap2.so | 8df39ce9002bd5744baa9d74f4a307a58d79a5ec3612caee9a903d1de0400507 |
| iap2/bin/iap2connectionmanager | 83bfd58b8b677bc4967d7f4a9d36cd05c849564c84ea8c7afe68e9604b36ebce |
| iap2/etc/extdevconfig.json; identical smartphone_integration/etc copy | 95dc0a3e0b952a4ad4fd6f346d50eed01b4b79cfbb627840090c09edcbe1a9e8 |

Static tools: installed /usr/bin/objdump (LLVM), symbol/relocation/section
inspection, strings/config reads and small local Python stdlib ELF64/relocation
readers. Disassembly, one-off ADRP reference scans, excerpts and historical
git-show output remain under ignored `.local-research/mpr3/phase7-work/`.
Linear xref candidates were checked manually for effective address, object
header/literal and call-register use; the scan is not exhaustive CFG analysis.
No external ABI documentation or live target behavior was substituted.

Reproduction uses read-only operations on each image:

```text
objdump -T -C <image>       # dynamic symbols
objdump -R <image>          # relocations
objdump -h -p <image>       # sections/dependencies
objdump -d -C <image>       # static AArch64 disassembly
```

Correlate ADRP/add/ldr effective addresses with ELF section bytes and RELATIVE /
symbol relocations. For CFLString constants inspect the header and text at
object+8, then verify the helper call receives that object. For serializers
trace named formatters to object fields, caller selector/jump table and final
ID/header byte stores. These instructions are analysis, not deployment steps.

## Validation

All 18 requested Phase7 reports were created. Only this new directory, the
analysis index and changed research-status conclusions were edited. No
implementation source, Phase1–6 report, target contract or build/deployment
tooling was changed. All 5,620 firmware entries have unchanged mode, size and
mtime against the before-analysis snapshot; all six primary source hashes
match. Git diff --check passes, as does a separate no-index whitespace check
for each untracked report. All local document links resolve. Final working-tree
scope contains two modified index/status documents and the new Phase7
directory. No firmware files are tracked. No files are staged, committed or
pushed. Host tests were not rerun because no implementation/build files changed.
