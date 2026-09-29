# SETUP CF key map

## Exact objects, not semantic guesses

Source: `libairplay.so`, AirPlayReceiverSessionSetup @ 0x58dc0. Addresses are
image-relative evidence metadata. The objects below have CFL constant-string
headers and inline NUL-terminated text at object+8. Constructor initialization
is unnecessary for these static constants. No exported per-key global symbol
was recovered. Calls consuming each key establish use, rather than strings alone.

| Key object | String/semantic name | Used by / call site | Confidence |
|---|---|---|---|
| 0x106610 | `streams` (literal @ 0x106618) | Typed array lookup 0x58f94 | PROVEN |
| 0x107128 | `type` (literal @ 0x107130) | Integer extraction 0x59038; response descriptor insertion 0x59160 | PROVEN |
| 0x107330 | `streamConnectionID` (literal @ 0x107338) | Type-110 integer lookup 0x590c8; other stream paths 0x592b4, 0x59440 | PROVEN |
| 0x107e10 | `timingPort` | Read 0x58e68, response set 0x58eac | PROVEN |
| 0x107fc0 | `keepAliveLowPower` | Read 0x58ebc | PROVEN |
| 0x1080c8 | `osBuildVersion` | Text read 0x58efc | PROVEN |
| 0x1080e0 | `ct` | Integer read 0x5936c | PROVEN |
| 0x1080f0 | `audioFormat` | Reads 0x59380, 0x594ac | PROVEN |
| 0x108108 | `redundantAudio` | Read 0x59850 | PROVEN |
| 0x108120 | `usingScreen` | Read 0x59864 | PROVEN |
| 0x108138 | `controlPort` | Read 0x59880, response set 0x5a3f4 | PROVEN |
| 0x108150 | `audioLatencyMs` | Reads 0x598a0, 0x59db8 | PROVEN |
| 0x108168 | `latencyMin` | Read 0x5a1f8 | PROVEN |
| 0x108180 | `latencyMax` | Read 0x5a20c | PROVEN |
| 0x108198 | `spf` | Read 0x598f8 | PROVEN |
| 0x108288 | `dataPort` | Read 0x5a574; response sets 0x59170, 0x596c8, 0x5a3e4 | PROVEN |
| 0x108048 | `eventPort` | Response set 0x5a18c | PROVEN |
| 0x1083e8 | `keepAlivePort` | Response set 0x5a284 | PROVEN |

This is the direct dictionary-helper key inventory in SETUP; it is not a
complete schema of nested objects handled by callees. Literal names do not
prove units, optionality or meanings for abbreviated audio fields.
`setUpStreams` @ 0x108390 is a platform-control command passed at 0x597a0 /
0x59d20, not a request dictionary field. Other nearby constants used as an
ADRP/add base must not be counted as helper arguments.

## Correction to the previous unnamed key

PROVEN: the effective address is 0x107330, formed from page 0x107000 plus
0x268 plus 0xc8. Phase 6's approximate 0x1072c8 omitted the intermediate add;
that address lies in diagnostic text, not the key object. The name is
`streamConnectionID`, not dataPort/controlPort/eventPort. This correction
supersedes the earlier uncertainty without editing historical evidence.

## Retrieval and xref limitations

STRONG EVIDENCE: a newly created CFL string with exactly `streams` or `type`
is a valid content-equal key for dictionaries with CFL type key callbacks.
The key callback table @ 0x159318 uses CFLEqual/CFLHash; string class equality
@ 0x7a5a0 compares constant/dynamic combinations with strcmp, and string hash
@ 0x7a860 hashes the same bytes. This supplies a retrieval mechanism without
runtime firmware addresses or invented exported key symbols.

SETUP reads and writes above provide multiple xrefs for `type`, ports and
streamConnectionID. AirPlayReceiverSessionPlatformControl @ 0x48410 also
performs a streams/type enumeration (0x4863c, 0x486bc, 0x486d4), using distinct
content-equivalent constant objects. The linear ADRP/load/add scan was used
to find candidates, followed by manual call-argument checks. It is not a
complete CFG-aware xref database; exhaustive all-image xrefs and indirect
references remain UNKNOWN. No function was called to validate a key at runtime.
