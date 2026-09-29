# Phase 6 — runtime-hook and advertisement contract

## Executive result

The static evidence is sufficient for an **offline, mocked prototype**, but not
for vehicle deployment. The strongest practical seam is the internal PLT call
to `AirPlayReceiverSessionSetup`: it has a recoverable three-argument ABI and a
JUMP_SLOT relocation. The display-advertisement seam is higher level and
architecturally attractive—`infoRequestDisplays(int&)`—but its exact
0xa0-byte delegate slot is still unresolved. `dlsym(RTLD_NEXT)` is likely valid
for the SETUP symbol, subject to target loader/policy verification.

Stock type 110 can remain untouched conceptually. A wrapper can preserve its
descriptor object and send it to the original implementation while privately
retaining a type-111 descriptor. That does not solve iOS capability
advertisement, secondary transport/decoder ownership, displayable occupancy,
or endpoint selection.

## 1. AirPlayReceiverSessionSetup ABI

**STRONG EVIDENCE:**

```c
int AirPlayReceiverSessionSetup(
    AirPlayReceiverSessionPrivate *session,
    CFDictionaryRef request,
    CFDictionaryRef *responseOut);
```

Evidence is `libairplay.so:0x58dc0` entry saves and uses x0/x1/x2 as session,
request, and output pointer; the only recovered caller is
`AirPlayReceiverServerControl:0x4c7bc`, which checks the integer result and
releases request/output objects. x3–x7 are not used at entry in the recovered
function. Exact retain counts and every error-path ownership transition are
UNKNOWN.

## 2. SETUP dictionary contract

**STRONG EVIDENCE:** the request’s `streams` object is fetched around
`0x58f88`; elements are typed CFDictionary objects; `type` is read around
`0x59038`; the dispatcher normalizes `type - 100`, accepts through 110, and
rejects 111+. For type 110, the descriptor pointer is passed unchanged to
`AirPlayReceiverSessionScreen_Setup:0x590a8`.

A wrapper can plausibly create a new CFArray and append original non-111
descriptor objects, preserving type 110 exactly. It must not mutate the source
array/dictionaries without proving their mutability and ownership. The full
port/connection key set remains incomplete.

## 3. Loader and launch contract

`AirPlayReceiverSessionSetup` is exported at `0x58dc0` and has an
`R_AARCH64_JUMP_SLOT` at `0x15ebc0`; the internal call is through PLT at
`0x4c7bc`. No DF_SYMBOLIC/BIND_NOW flag was visible in available dynamic
output. **RTLD_NEXT LIKELY VALID**, but actual loader scope, symbol versioning,
and target environment are untested.

`smartphone_integrator` launches `dio_manager` through a configured `posix_spawn`
child and `/usr/bin/enforcer`. The carplay environment explicitly sets
`LD_LIBRARY_PATH` and config variables. A mirrorlink child explicitly uses
`LD_PRELOAD`, proving the launcher can transmit the variable; carplay does not
set it in stock configuration. AppArmor profiles referenced by Enforcer were
not present in the extracted data. Thus preload under stock policy is UNKNOWN.

## 4. Advertisement callback

`CAirPlayServer::registerDelegates:0x1fd340` copies 0xa0 bytes and invokes
`AirPlayReceiverServerSetDelegate`; libairplay stores them at server+0x18.
`CDIOManager::infoRequestDisplays:0x1db6e0` calls `getDisplays`, passes an
error reference, and retains the returned array when error is zero. This is
the best conceptual place to append a second descriptor. The exact delegate
byte field is UNKNOWN; do not encode a slot based only on the method name.

## 5. Stock descriptor

`getDisplays` constructs one entry. Proven/strong fields are:

```text
features       boolean-derived mask using bits 0x1,0x2,0x4,0x8,0x10
widthPixels    1540 (config path)
heightPixels   720  (config path)
widthPhysical  235  (config path)
heightPhysical 110  (config path)
display name   Audi MMI (config correlation)
```

`displayUUID`, `modes`, primary/main role, rotation and overscan are not proven
in `createDisplaysDictionary`. The second descriptor shape is consequently
UNKNOWN.

## 6. iAP2 and iOS causal chain

Historical MHI3 material proves a WirelessCarplayTransportComponent nested
subparameter 17 void marker under parent 21, used by an Apple-side ThemeAssets
gate. P3695 has generic iAP2 identification machinery but no proven P3695
emission or handling of this child. Classification is UNKNOWN between generic
expressibility and a new serializer requirement.

The chain `iAP2 identification -> capability -> AirPlay /info -> iOS SETUP
type 111` is only partly supported: the first relationship is strong/historical;
the P3695-specific arrows are UNKNOWN. Stock SETUP handling of type 111 is
DISPROVEN.

## 7. Displayable and cluster feasibility

The best unclaimed-looking name is `Displayable_Debug_1` (137), but diagnostic
evidence explicitly warns that configured displayables may already be occupied.
It is only a possible test candidate, not safe production storage.

`videoencoderservice` directly resolves the integer displayable with
`IpTeConnection::getDisplayable`, validates dimensions, and feeds the resulting
displayable into `CEncoder`; no intermediate cluster compositor is proven.
The VideoEncoding proxy/API is therefore technically reusable, but authorization
and the active endpoint remain unresolved. Endpoint enums cover MOST, Ethernet,
and minor variants; static coding values needed to select one are absent.

## 8. Feasibility classification

| Layer | Classification | Evidence |
|---|---|---|
| iAP2 advertisement | UNKNOWN | P3695 child-17 status unresolved |
| AirPlay display advertisement | SMALL HOOK REQUIRED / UNKNOWN ABI | one-entry callback path proven; descriptor shape/slot unresolved |
| type-111 dispatch | SMALL HOOK REQUIRED | stock range rejects 111 |
| second screen context | REUSABLE WITH SECOND INSTANCE | independent 0x340 context, socket, ScreenStream |
| ScreenStream | REUSABLE WITH SECOND INSTANCE | instance-oriented factory/path |
| H.264 decoder | REUSABLE WITH SECOND INSTANCE | no blocking singleton proven |
| renderer | REUSABLE WITH SECOND INSTANCE, target selection unresolved | stock renderer path/config evidence |
| IPTE displayable creation | AVAILABLE AS-IS for configured names | `dint_create_displayable` resolves LayerConfig name |
| cluster selection | UNKNOWN | endpoint selector/coding unresolved |
| cluster encoding | AVAILABLE AS-IS after valid displayable/endpoint | direct getDisplayable -> feed path |
| teardown/fail-open | UNKNOWN | secondary lifecycle not implemented/proven |

## 9. Sidecar architecture decision

The proposed architecture is technically plausible without modifying
`libairplay.so` on disk, `videoencoderservice`, or LayerConfig, but this is not
yet deployment-proven. The minimum required modification boundary is an
in-process negotiation seam (preferably a correctly interposed SETUP call or a
recovered delegate callback) plus a new secondary owner. The owner would need a
validated configured displayable and the runtime-selected cluster endpoint.

Preserving type 110 byte-for-byte is plausible because the original descriptor
can be passed unchanged. The sidecar cannot receive type 111 unless an input
advertisement/SETUP seam first causes or observes it; stock P3695 will reject it.

## 10. Offline prototype decision

**READY FOR OFFLINE PROTOTYPE.** The source-level prototype may model the
three-argument wrapper, CF array preservation, independent screen state,
VideoEncoding proxy, direct encoder input, and mocked display ownership. It
must not claim that it can load on the vehicle until the remaining policy,
delegate-slot, descriptor, iAP2, endpoint, and occupancy questions are solved.

See the companion files in this directory for the evidence ledger and each
contract.
