# P3695 Phase 2: stream type 111 and cluster encoder API

## Scope and evidence grading

This is static analysis of the P3695 extracted AArch64 binaries. Target
executables were not run. `PROVEN` means directly established by control flow
or a call site; `STRONG EVIDENCE` means multiple consistent binary artifacts;
`PLAUSIBLE` and `UNKNOWN` are retained where stripped/generated code prevents
an exact assignment. This report does not alter the Phase 1 report.

## Executive result

**Stream type 111: REJECTED by the recovered SETUP dispatcher.**

`AirPlayReceiverSessionSetup @ 0x58dc0` reads the stream descriptor's integer
type at `0x59028–0x59038`, subtracts 100 at `0x59040`, and accepts only the
inclusive range 100–110 at `0x59044–0x59048`. Types greater than 110 branch to
`0x58fe8`. The type-110 case reaches
`AirPlayReceiverSessionScreen_Setup@plt` at `0x590a8`; type 111 cannot reach
that case. This is the strongest evidence currently available against native
P3695 handling of a CarPlay SETUP stream type 111.

This conclusion is about the AirPlay SETUP stream descriptor type. It is not a
claim about protocol byte `0x71`/decimal 113 in
`AirPlayReceiverSessionScreen_ProcessFrames`.

**Cluster encoder: ID-based displayable consumption is proven; exact public
RPC mapping remains partly unknown.** `videoencoderservice` passes an integer
displayable argument to `gfx::IpteConnection::getDisplayable` at `0x1e08c`,
checks width and height, creates the encoder, and later feeds the resulting
`IpteDisplayable` into `gfx::CEncoder::feed` at `0x1d498`. The generated IDL
contains `call_struct_DisplayID_I32IVideoEncodingReply`, proving a two-field
RPC call shape exists, but the stripped artifact does not allow assigning that
shape to `setActiveDisplayable` with certainty.

## Compact result table

| Question | Result | Confidence | Evidence |
|---|---|---|---|
| Does P3695 handle SETUP stream type 111? | No path in recovered dispatcher; 111 branches to common out-of-range path | PROVEN | `libairplay.so` `AirPlayReceiverSessionSetup` `0x59040–0x59060`, jump table at `0x105fe8` |
| Does it handle stream type 110? | Yes, screen setup path | PROVEN | `0x59068`, `AirPlayReceiverSessionScreen_Setup` call at `0x590a8` |
| Does it handle protocol opcode 0x71? | A screen-frame parser compares it, but this is a different layer | PROVEN | Phase 1 `AirPlayReceiverSessionScreen_ProcessFrames @ 0x5d070`, compare at `0x5d360` |
| Is the AirPlay display model multiple-screen? | SETUP dispatcher is multi-descriptor in structure, but only one screen descriptor type is accepted | STRONG EVIDENCE | array loop `0x58f98–0x59004`; one screen case at type 110; no second accepted screen type |
| What is the feature mask? | Config-derived value OR `0x6104040280` | PROVEN | `AirPlayGetFeatures @ 0x49d40` |
| Is `setActiveDisplayable` signature exact? | IDL shape suggests `(DisplayID, I32)` exists, but method mapping is UNKNOWN | STRONG EVIDENCE / UNKNOWN | `.rodata 0x376f8`; RPC stub method comparisons at `0x27200` |
| Can encoder consume a displayable selected by integer ID? | Yes, subject to getDisplayable success and nonzero dimensions | PROVEN | `0x1e088–0x1e0e0`, `0x1e250`, `0x1d498` |
| Are arbitrary IDs allowed by policy? | UNKNOWN | UNKNOWN | No whitelist/policy caller was proven in this phase |

## 1. AirPlay SETUP stream negotiation

### Recovered pseudocode

The relevant portion is equivalent to:

```c
for (i = 0; i < CFArrayGetCount(streams); ++i) {
    CFDictionaryRef d = CFArrayGetTypedValueAtIndex(streams, i, ...);
    int64_t type = CFDictionaryGetInt64(d, streamTypeKey, 0);
    switch (type) {
    case 100:
    case 101:
        goto handler_593f0;
    case 102:
        goto handler_59258;
    case 110:
        reply = CFDictionaryCreateMutable(...);
        err = AirPlayReceiverSessionScreen_Setup(screenSession, d, ...);
        CFDictionarySetInt64(reply, typeKey, 110);
        break;
    default:
        goto common_skip_or_error_58fe8;
    }
}
```

The compiler-generated jump table is more precise than string evidence:

| Type | Target | Classification |
|---:|---:|---|
| 100 | `0x593f0` | HANDLED path |
| 101 | `0x593f0` | HANDLED path |
| 102 | `0x59258` | HANDLED path |
| 103–109 | `0x58fe8` | common skip/error path, not a dedicated handler |
| 110 | `0x59068` | HANDLED screen setup |
| 111 | `0x58fe8` via `B.HI` | REJECTED |
| 112 | `0x58fe8` via `B.HI` | REJECTED |
| 113 | `0x58fe8` via `B.HI` | REJECTED |
| 114 | `0x58fe8` via `B.HI` | REJECTED |

The stream array loop proves that multiple descriptors can be iterated. It does
not prove multiple screen descriptors: only type 110 reaches the screen setup
handler in this dispatcher. No separate accepted 111 screen branch was found.

### Unsupported diagnostics

The binary contains several generic diagnostics, including
`Unsupported stream type %d` at string virtual/file offset `0x103610`, an audio
variant at `0x1036f0`, and a second AirPlay variant at `0x109f50`. The exact
diagnostic call sites are in stripped/private code and some nearby code is
mislabelled by `objdump` because dynamic symbol extents overlap private
functions. The decisive evidence does not depend on matching a diagnostic:
the range check and branch at `0x59040–0x59048` are the actual dispatcher
decision.

## 2. AirPlay display model and feature mask

`AirPlayReceiverSessionSetup` is structurally capable of walking more than one
descriptor because it obtains an array count and increments `w25` until the
count is reached (`0x58f98–0x59004`). That is **PROVEN**. It is not proof that
P3695 advertises more than one screen to iOS.

`AirPlayGetFeatures @ 0x49d40` obtains a property-derived integer and ORs the
unconditional mask `0x6104040280`. Individual bit meanings are **UNKNOWN**;
the mask cannot be labelled AltScreen from this evidence.

Phase 1’s `dio_manager` result remains valid: the observed display dictionary
path creates one CFDictionary and appends it once. Phase 2 found no second
accepted screen descriptor. Therefore the model is best classified as:

**STRONG EVIDENCE: one advertised screen in the observed P3695 path; the
generic array/dispatcher structure is potentially multi-descriptor, but native
secondary-screen support is not proven and type 111 is rejected.**

## 3. dio_manager display-property construction

`createDisplaysDictionary @ 0x1d9be0` is a direct one-entry constructor:

```c
CFArrayRef createDisplaysDictionary(bool b0, bool b1, bool b2, bool b3,
                                    uint32_t a4, uint32_t a5,
                                    uint32_t a6, uint32_t a7,
                                    uint32_t a8, uint32_t a9,
                                    uint32_t a10, int *err) {
    CFArrayRef out = CFArrayCreateMutable(...);
    CFDictionaryRef d = CFDictionaryCreateMutable(...);
    int features = 0;
    if (b0) features |= 0x2;
    if (b1) features |= 0x8;
    if (b2) features |= 0x4;
    if (b3) features |= 0x10;
    CFDictionarySetInt64(d, key_0x460, features); // 0x1d9c58–0x1d9c94
    CFDictionarySetInt64(d, key_0x478, a6);       // 0x1d9c98–0x1d9ca4
    CFDictionarySetInt64(d, key_0x260, a4);       // 0x1d9cbc–0x1d9ccc
    CFDictionarySetInt64(d, key_0x278, a5);       // 0x1d9cd0–0x1d9cdc
    CFDictionarySetInt64(d, key_0x4a8, a7);       // 0x1d9cf4–0x1d9d00
    CFDictionarySetInt64(d, key_0x4c0, a8);       // 0x1d9d04–0x1d9d10
    if (a9) CFDictionarySetInt64(d, key_0x4d8, a9);
    CFArrayAppendValue(out, d);                   // 0x1d9d30–0x1d9d38
    CFRelease(d);
    return out;
}
```

The symbolic names of keys are not retained in this stripped binary. The
function has no loop and exactly one `CFArrayAppendValue`; it cannot itself
describe multiple displays. The three lambda/property calls from
`setScreenProperties @ 0x1f73c0` remain indirect; their exact API names and
keys were not proven here.

## 4. Cluster encoder API and semantics

The encoder-side path is concrete:

```c
int select_displayable(Service *s, int displayable_id) {
    shared_ptr<IpteDisplayable> d;
    int rc = s->ipte->getDisplayable(displayable_id, d); // 0x1e088
    if (rc != 0 || !d) return error;
    uint16_t w = d->width();  // 0x1e09c–0x1e0a4
    uint16_t h = d->height(); // 0x1e0a8–0x1e0b4
    if (w == 0 || h == 0) return error; // 0x1e0cc–0x1e0e0
    encoder = CEncoder::create(config); // 0x1e250
    ...
}

void push_frame(shared_ptr<IpteDisplayable> d) {
    encoder->feed(d); // call at 0x1d498
}
```

The service contains display-ID logging and CLUSTER/MINOR_CLUSTER/HUD pipeline
configuration strings. It also contains `setActiveDisplayable` and
`setDisplayable` names, but no preserved source-level method bodies.

The generated interface contains:

```text
comm_idl_calls::call_struct_DisplayID_I32IVideoEncodingReply
```

This is strong evidence for a call carrying `DisplayID` plus one 32-bit value.
The RPC stub code at `0x27200` distinguishes method IDs 9 and 10 and serializes
arguments through the common serializer. It does not retain enough information
to map IDs 9/10 to the names `setActiveDisplayable`, `setDisplayable`,
`requestVideoConnection`, or `releaseVideoConnection`. Thus the exact claim
`setActiveDisplayable(displayId, displayableId)` is **PLAUSIBLE/strongly
suggested by the IDL type and event fields, but not PROVEN** in this artifact.

No client binary containing an unambiguous call-site for `setActiveDisplayable`
was found in the currently extracted tree or the searched listings. The normal
native cluster caller and exact display ID are therefore **UNKNOWN**.

No range/whitelist check for arbitrary displayable IDs was proven. What is
proven is API-level lookup by integer ID plus dimension validation.

## 5. Ownership results

No new owner was proven for IDs 26, 29, 40, 42, 60, or 93. The new evidence
identifies `videoencoderservice` as a consumer of a supplied displayable ID,
not as its creator or writer. In particular, the existing Phase 1 fact that
stock CarPlay uses displayable 93 remains local renderer configuration evidence;
this phase does not connect 93 to the cluster encoder.

## 6. Proven vs unknown

### PROVEN

- P3695’s recovered AirPlay SETUP dispatcher accepts a maximum stream type of
  110.
- Stream type 111, and 112–114, do not reach the screen setup case.
- The dispatcher iterates an array of stream descriptors.
- Type 110 reaches `AirPlayReceiverSessionScreen_Setup`.
- `AirPlayGetFeatures` ORs `0x6104040280` into its result.
- The encoder service resolves an integer displayable ID through
  `IpteConnection::getDisplayable` and feeds the resulting surface to
  `CEncoder` after nonzero-dimension checks.

### STRONG EVIDENCE

- The observed P3695 AirPlay display model is one screen, although generic
  descriptor iteration is multi-entry capable.
- The video-encoding IDL has a `DisplayID + I32` call shape.

### UNKNOWN

- Semantic names for the `0x6104040280` feature bits.
- Exact diagnostic call-site mapping for every `Unsupported stream type` string.
- Exact RPC method IDs for `setActiveDisplayable` and `setDisplayable`.
- Normal native cluster client, display ID, and displayable ID.
- Whether runtime policy restricts arbitrary displayable IDs.
- Creators/writers of the requested displayable IDs.

## 7. Highest-value next steps

1. Obtain the matching `IVideoEncodingS.hxx`/IDL or an extracted client proxy
   binary and map method IDs 9/10 to names and serializers.
2. Search all TFFS application binaries for the generated proxy type and the
   `DisplayID_I32` call structure; recover the native cluster caller.
3. Resolve private AirPlay diagnostic xrefs with an AArch64-aware disassembler
   that preserves function boundaries, then confirm the same type range in any
   audio-specific dispatcher.
4. Recover the configuration key table around `libairplay.so` `0x107128` and
   the `AirPlayGetFeatures` property input to assign feature-bit meanings.
5. Trace displayable registration APIs in the display-manager/graphics clients
   before considering any integration seam.

