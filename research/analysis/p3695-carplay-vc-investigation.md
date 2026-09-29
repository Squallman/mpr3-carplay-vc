# P3695 CarPlay -> Virtual Cockpit investigation

Research date: 2026-09-29  
Firmware family: `MPR3_ER_AU_P3695`  
Scope: static analysis only; no target firmware execution, modification, flashing, packaging, or tracked-source changes.

## 1. Executive summary

The P3695 artifacts prove a conventional single CarPlay main-screen pipeline:

```text
iPhone screen stream -> libairplay ScreenStream -> AirPlay/GAL video renderer
 -> Displayable_External_Smarthphone (93) on Driver_Display
```

The P3695 configuration names one CarPlay screen target: `Driver_Display`, displayable 93. `dio_manager::getDisplays` calls `createDisplaysDictionary` once, and the helper creates one mutable CFArray and appends one screen dictionary. This is strong evidence against P3695 advertising multiple CarPlay screen descriptors in the observed path.

The AirPlay library has a generic `AirPlayInfoArrayAddScreenDisplay` routine capable of appending a screen dictionary to an array, and contains numeric stream-type values including 111, 112, and 113. That proves generic stack vocabulary and reusable infrastructure, not that P3695 advertises or renders an AltScreen. No explicit AltScreen/view-area/UI-context feature identifiers were found in the inspected P3695 strings. Stream type 111 remains **UNKNOWN**: the inspected screen receive loop has a direct `0x71` (113) opcode comparison and the screen startup path is single-instance, but a complete higher-level SETUP dispatcher for every stream type was not recovered.

The local cluster encoder is architecturally displayable-oriented. `videoencoderservice` imports `gfx::IpteConnection::getDisplayable`, logs and validates `display_id`/`displayable`, and exposes `setActiveDisplayable`, `setDisplayable`, `requestVideoConnection`, and `releaseVideoConnection`. The three configured encoder classes are `CLUSTER`, `MINOR_CLUSTER`, and `HUD`, with separate MOST/Ethernet transport paths. This makes a second decoded surface feeding an existing cluster composition plausible, but does not prove that arbitrary displayable IDs can be selected by an external client or that the cluster composition accepts a CarPlay surface.

Route Guidance is strictly separate from secondary video. P3695 contains `routeGuidance=%i` in `smartphone_integrator` and `iap2connectionmanager`, but the inspected configs do not register Apple's 0x5200-0x5204 family, and the P3695 reports contain no corresponding message-family evidence. The origin and semantics of that boolean remain UNKNOWN.

Overall conclusion: native secondary CarPlay video on the P3695 Virtual Cockpit is **not proven**. The strongest negative evidence is the one-entry display advertisement and absence of explicit AltScreen capability identifiers. The strongest positive evidence is the generic stream/display/decoder infrastructure and the displayable-driven cluster encoder. Neither is sufficient to claim native support or impossibility.

## 2. Firmware/artifact inventory and SHA-256

All paths below are under `.local-research/mpr3/P3695/extracted/files/`.

| Artifact | ELF / config identity | SHA-256 |
|---|---|---|
| `smartphone_integration/bin/dio_manager` | ELF 64-bit LSB PIE AArch64, stripped, BuildID `9c7564d992ad5486cbddfa229d2b06be5cbc8f0c` | `1b3257c53c795f2477cd7ce0abc1e2ad22d43d8b001a4b265d74d6f124e2b490` |
| `smartphone_integration/bin/smartphone_integrator` | ELF 64-bit LSB PIE AArch64, stripped, BuildID `e2f8937adaa60e20d56a5f476297b2463649335b` | `f3dee3240f06c57784227da0514d29b9d215855c6f59e441d579edc431a699a5` |
| `smartphone_integration/bin/gal` | ELF 64-bit LSB PIE AArch64, stripped, BuildID `e725ec43df5756c15ec644802e6b97acfc4b3f59` | `b4c758439dac91d5721e91c8159f948fddb800ecf7f9f015f8d13673902df13e` |
| `smartphone_integration/lib/libairplay.so` | ELF 64-bit LSB AArch64, stripped, BuildID `11a62aa396c94fb5ffc28057ae6e4e9a54b6e754` | `67be0a5639a8e4bb4f55677f26aeb3eadae3d4baf68180db3bcaeb77536b5c28` |
| `smartphone_integration/lib/libautoreceiver.so` | ELF 64-bit LSB AArch64, stripped, BuildID `195baefa3cedc1e745ed3353f2c4161ad9df8087` | `7dff6f7435c5ca917b21219227e2cce70f981132170b48bdd4e22aaca2f6946a` |
| `iap2/bin/iap2connectionmanager` | ELF 64-bit LSB PIE AArch64, stripped, BuildID `8ebed8427bae7ff7d1241de01ff79527c3dbebf9` | `83bfd58b8b677bc4967d7f4a9d36cd05c849564c84ea8c7afe68e9604b36ebce` |
| `iap2/lib/libesoiap2.so` | ELF 64-bit LSB AArch64, stripped, BuildID `8e237a519ef66a6ce6438b60f8a692d6fdd68a10` | `8df39ce9002bd5744baa9d74f4a307a58d79a5ec3612caee9a903d1de0400507` |
| `usr/bin/videoencoderservice` | ELF 64-bit LSB PIE AArch64, stripped, BuildID `e3256918a64be6c7a21905c1867b94f80dc4fe90` | `a65e646a905357f13816073a94ec855d492b8f9b4e2ca193123c0ffdef168797` |
| `smartphone_integration/etc/dio_manager.json` | ASCII configuration | `2935ee50fb643de9de9225b060d381a1efed41568a52c6e41d2eb523bbbbdc95` |
| `etc/eso/displaymanager.json` | ASCII configuration | `ef755528b147d0b8c8615600754d4795f5ae3f2a207c514969b963576a1e66ac` |
| `etc/eso/videoencoderservice.json` | ASCII configuration | `49c3a56de4a249500a4b2dd4d3179b3352b9a3c16d22b2b11dda3bd0933cdf55` |
| `etc/eso/videoovermost.json` | ASCII configuration | `5a2e54032c15d95923a4c508c351b04b6f7c767d6c7e61a8a20489e09c2b25cb` |

No additional firmware files were extracted during this investigation.

## 3. Confirmed P3695 architecture

### Proven: the inspected firmware is Linux/AArch64

Evidence: `file` identifies all inspected executables/shared objects as AArch64 ELF files with `/lib/ld-linux-aarch64.so.1` for PIE executables. Target binaries were not executed.

### Proven: stock CarPlay screen target is displayable 93

Evidence:

* `smartphone_integration/etc/dio_manager.json`, `screen` section: `display="Driver_Display"`, `displayable="Displayable_External_Smarthphone"`, `displayableID=93`.
* `smartphone_integration/etc/gal.json`, renderer section: `displayableID=93`.
* `etc/eso/displaymanager.json`: displayable map entry `93: Displayable_External_Smarthphone`.

The spelling `Smarthphone` is firmware/config nomenclature.

### Proven: display configuration contains cluster and virtual output objects

`displaymanager.json` defines `Cluster_Display` id 1, `Cluster_Display_Subframe` id 80, and virtual displays 90-95 for cluster/HUD MOST/Ethernet and minor-cluster paths. These names identify display infrastructure only; they are not evidence that any one object is a CarPlay AltScreen target.

## 4. Main CarPlay screen negotiation

### Proven: `getDisplays` creates one display dictionary in the observed path

`dio_manager` dynamic symbol:

```text
_ZN3dio11CDIOManager11getDisplaysERi @ 0x1db510, size 0x1cc
_ZN3dio24createDisplaysDictionaryEbbbbjjjjjjjiRi @ 0x1d9be0, size 0x254
```

At `getDisplays+0xf8` (`0x1db608`), the function calls `createDisplaysDictionary` exactly once. `createDisplaysDictionary`:

* creates a mutable CFArray at `0x1d9c28`;
* creates one mutable CFDictionary at `0x1d9c44`;
* sets feature/mode and geometry integer keys;
* optionally sets a display UUID-related value through the caller-provided/config-derived values;
* appends the dictionary once at `0x1d9d30-0x1d9d38`;
* releases the dictionary and returns the array.

There is no loop around the dictionary construction and no second append in this helper. **Strong evidence:** P3695's concrete `getDisplays` path advertises one descriptor for each call.

### Proven: the generic AirPlay display-array helper is structurally append-capable

`libairplay.so` exports `AirPlayInfoArrayAddScreenDisplay @ 0x578f0`, size `0x174`. It creates a screen dictionary, sets several integer fields, and appends it to the array at `0x579d0-0x579d8`. Therefore the underlying AirPlay API can represent multiple entries if called repeatedly.

This is not evidence that P3695 calls it more than once for CarPlay negotiation. A direct P3695 `dio_manager` call to this symbol was not found in the inspected disassembly; `dio_manager` instead constructs its own display dictionary and imports `AirPlayCreateModesDictionary`.

### Proven: `displayUUID` exists in the P3695 negotiation vocabulary

The P3695 string/dynamic-symbol report includes `displayUUID`, `displays`, `getDisplays`, `createDisplaysDictionary`, and `AirPlayInfoArrayAddScreenDisplay`. The exact runtime UUID source is not fully resolved from stripped code. `getDisplays` passes config/coding-derived values into `createDisplaysDictionary`; no second navigation-specific UUID source was proven.

### Strong evidence: `Driver_Display`/93 is a local renderer target, not proof of iOS advertisement semantics

The `Driver_Display` and displayable 93 values are present in the screen renderer and GAL config. The screen advertisement helper receives separate booleans/geometry/UUID-like arguments and constructs a CF dictionary. The available evidence does not establish that local displayable 93 is copied verbatim into the iPhone-facing display descriptor. Treat these as local render-target configuration unless the AirPlay HTTP/RTSP dictionary path is traced further.

## 5. AirPlay screen/stream support

### Proven: standard screen stream API and decoder integration are present

`libairplay.so` exports or contains dynamic symbols for:

* `_ScreenSetup`, `_ScreenStart`, `_ScreenStreamDecode`, `_ScreenStreamFinalize`;
* `ScreenStreamCreate @ 0xf7360`;
* `ScreenStreamProcessData @ 0xf7b10`;
* `ScreenStreamSetAVCC @ 0xf7630`;
* `ScreenStreamStart @ 0xf7970`;
* `CESOGfxVideo::createDecoder`, `decodeCallback`, `cbDecoderRenderCb`, `cbDecoderResolutionCb`;
* `CESOGfxVideoImpl::configure`, `decode`, `initialize`, `start`, and `stop`.

`AirPlayReceiverSessionScreen_StartSession @ 0x5e0b0` calls `ScreenStreamCreate` at `0x5e124`, then sets delegate context, max FPS, window offset, width/height, copies delegates, and calls `ScreenStreamStart` at `0x5e190`. The stream object is stored at session offset `+0x328`.

### Strong evidence: the confirmed screen startup object is single-instance per screen session

The observed `StartSession` path initializes one `ScreenStream` pointer at session offset `+0x328`, configures it once, and starts it once. This does not rule out a separate session/object elsewhere, but no second screen-stream member or AltScreen-specific renderer object was proven.

### Proven: decoded input is H.264/NAL-oriented

`ScreenStreamProcessData` validates the stream object and input, then calls `H264GetNextNALUnit` repeatedly (`0xf7c38` and `0xf7d2c`) before invoking the decoder callback path. `ScreenStreamSetAVCC` is called from the receive-frame path at `0x5dd54`. This proves the ordinary H.264 screen decode path, not secondary-screen semantics.

## 6. Explicit analysis of stream type 111

### Direct observations

1. The AirPlay library contains a numeric enum/string conversion sequence at `0x45eec-0x45f24` that emits values `110`, `111`, `112`, `113`, `114`, etc. Thus 111 is present as a recognized numeric AirPlay vocabulary value.
2. `AirPlayReceiverSessionScreen_ProcessFrames @ 0x5d070` receives a frame/opcode and at `0x5d35c-0x5d364` compares its first byte with `0x71` (decimal 113). The equal branch proceeds through the screen-frame handling path; the non-equal branch follows another path. This is a screen protocol opcode comparison, not by itself proof that AirPlay SETUP stream type 111 is rejected.
3. `AirPlayReceiverSessionScreen_StartSession @ 0x5e0b0` starts one `ScreenStream` instance and stores it at session `+0x328`.
4. During teardown, a command with type `0x71` (113) is sent at `0x523d8`, again showing that 113 is used in the observed screen command protocol.
5. The strings include generic `Unsupported stream type %d` and `Unsupported stream type: %d` diagnostics, but no static evidence here binds those diagnostics specifically to 111.

### Result

**UNKNOWN:** P3695 handling of AirPlay SETUP stream type 111.

Classification rationale:

* **Against native AltScreen support:** no explicit AltScreen identifiers, no second advertised display entry, no proven second renderer instance, and no recovered type-111-to-second-display dispatch.
* **For residual possibility:** numeric 111 is present in the library's stream vocabulary; generic AirPlay screen/decoder infrastructure is present; stripped private dispatch code remains incompletely named.
* **Not proven:** that 111 is rejected, ignored, mapped to the existing main screen, or routed to a hidden alternate object.

The claim “P3695 does not handle type 111” would exceed the evidence currently available.

## 7. iAP2 / `routeGuidance` investigation

### Proven: the literal is present in two non-AirPlay binaries

`strings -a -t x` finds `routeGuidance=%i` at:

* `smartphone_integration/bin/smartphone_integrator`, string offset `0x2c5f69`;
* `iap2/bin/iap2connectionmanager`, string offset `0xc7671`.

The same literal is represented in the combined report for the corresponding extracted artifacts. It does not occur in `libairplay.so`.

### Strong evidence: no configured Apple Route Guidance message family was found

The inspected `dio_manager.json`, `iap2connectionmanager.json`, and iAP2-related configuration do not register 0x5200, 0x5201, 0x5202, 0x5203, or 0x5204. The P3695 string report does not contain those identifiers, their decimal equivalents as message names, or Route Guidance message registration names. This is strong negative evidence against a config-driven implementation of the Apple Route Guidance family, but not proof that a binary-generated registration table cannot exist.

### Result: origin and meaning remain UNKNOWN

The literal's `%i` format suggests logging a boolean/integer capability, but the inspected static artifacts do not establish whether it comes from FEC, vehicle coding, CarPlay capability, an iAP2 identification parameter, or another smartphone feature. No parser, switch table, or forwarding path for Apple's 0x5200-0x5204 family was proven.

Route Guidance metadata therefore cannot be used as evidence for AltScreen video.

## 8. CarPlay video decoder/render pipeline

### Proven: local configuration enters screen setup

`dio_manager.json` supplies screen resolution 1540x720 and the screen display/displayable values. `CAirPlayHostThread::setScreenProperties @ 0x1f73c0` reads configuration fields at object offsets `0x474/0x478` and `0x464/0x468`, and makes three AirPlay-server API calls through `airplayServerAPI @ 0x1f5960`. A third property uses `CDioManagerComp::getValueInt(0x98)` at `0x1f7530`. This proves several configured screen-property calls, but the stripped lambda targets were not recovered well enough to assign every call to a named AirPlay property.

### Proven: decoder setup is parameterized by width/height/offset/FPS

`AirPlayReceiverSessionScreen_StartSession` passes runtime width, height, window offsets, and max FPS into `ScreenStreamSetMaxFPS`, `ScreenStreamSetWindowOffset`, and `ScreenStreamSetWidthHeight`. The downstream `CESOGfxVideo`/`CESOGfxVideoImpl` symbols include resolution callbacks and decoder configuration. Decoder creation is therefore not visibly hard-coded to one numerical resolution.

### Strong evidence: displayable 93 is selected by GAL renderer configuration

`gal.json` explicitly documents `Displayable_External_Smarthphone=93` and sets `renderer.displayableID=93`. The AirPlay screen config independently names the same displayable. This establishes where 93 enters the stock runtime configuration.

### UNKNOWN: whether the renderer can target another displayable without code/config changes

The GAL configuration exposes a single `displayableID` field and the inspected stock path uses 93. The symbols show reusable renderer/decoder abstractions, but no proven runtime setter or second-instance factory tied to an arbitrary displayable was recovered. A second decoder is theoretically plausible from object/API structure, not proven in P3695.

## 9. Cluster/HUD rendering and encoding pipeline

### Proven: the encoder service is displayable-oriented

The `videoencoderservice` string table contains:

* `asi.VideoEncoding.IVideoEncoding`;
* `getDisplayable`, `setDisplayable`, `setActiveDisplayable`;
* `requestVideoConnection`, `releaseVideoConnection`;
* `displayID`, `displayable`, `fps` validation/logging;
* `gfx::IpteConnection::getDisplayable(int, shared_ptr<IpteDisplayable>&)`;
* `gfx::CEncoder::feed(shared_ptr<IpteDisplayable>)`.

The encoder path therefore consumes an IPTE displayable object, obtains its dimensions, and feeds that object into the encoder. The `CVideoStreamEncoder` diagnostics explicitly log `try getDisplayable(%u)`, source width/height, `m_displayable_id`, and encoder/muxer/transport state.

### Proven: three output classes and separate transport configurations exist

`videoencoderservice.json` defines `CLUSTER`, `MINOR_CLUSTER`, and `HUD`. The cluster path uses `/dev/vsd/link-txpipe1`; minor cluster uses `link-txpipe2`; HUD uses `link-txpipe3`. Each has H.264 encoder/muxer settings. `videoovermost.json` identifies MOST connection labels including `CL_HUDVIDEO` and other cluster/video labels.

### UNKNOWN: exact `setActiveDisplayable` callers and RPC parameter semantics

The service exports the generated ASI/RPC vocabulary and logs `displayID`, `displayable`, and `fps`, but the stripped binary's dynamic table does not expose human-readable method bodies for the generated interface. A complete caller inventory across every extracted binary was not possible from the current artifact set. The evidence supports a method shape involving a display ID and displayable ID; it does not prove which display ID corresponds to the cluster encoder in every coding variant.

### Plausible, not proven: cluster encoder can consume an existing alternate displayable

Because `getDisplayable(id)` and `CEncoder::feed(IpteDisplayable)` are generic, an existing displayable ID could be accepted if it has an IPTE surface, valid dimensions, and a matching encoder/transport connection. What is not proven is whether `setActiveDisplayable` accepts arbitrary IDs, whether DisplayManager composes multiple layers before exposure, or whether the chosen cluster link permits switching while the stock pipeline is active.

## 10. Displayable ownership/map

### Proven: names and IDs exist in configuration

`displaymanager.json` maps at least these IDs:

| IDs | Config names |
|---:|---|
| 20-30 | Map, Map_Route_Guidance, Main_Secondary, Map_Cluster, Guidance_Main, Guidance_Hud, Guidance_Cluster, Map_Main |
| 40-48 | Cluster_Map, Cluster_Google_Earth_Map, Cluster_Map_Route_Guidance, cluster navigation stencil/speedcam/ETA/DTD/logo |
| 60-63 | Hud_Map and map annotation elements |
| 70 | AR_HUD |
| 80-93 | video/display objects, including 93 `External_Smarthphone` |

### UNKNOWN: process ownership and creation call sites

The configuration names do not prove which process creates IDs 20-30, 40-48, 60+, or 93. The current P3695 artifact set does not include a complete display-manager client inventory or all IPTE/gfx clients. Therefore the following are not asserted:

* that 40/26/29 are created by navigation rather than a generic display manager;
* that 93 is owned exclusively by `dio_manager` rather than GAL/video services;
* that any cluster map displayable is a CarPlay secondary-video target.

## 11. Comparison with historical MHI3/MHI2Q findings

The historical commit `e9c2b9a6c231b192420be21fdea78a8565068c47` documents MHI3/MHI2Q AltScreen gates, explicit feature strings, and MHI2Q stream-dispatch behavior. It is comparative evidence only.

Relevant historical facts:

* MHI2Q research identified explicit iOS-side AltScreen negotiation keys and a type-111 gate in that older stack.
* MHI2Q is QNX/ARMv7 and its binary implementation is not portable to this Linux/AArch64 MPR3 stack.
* The current repository intentionally implements route-guidance/maneuver integration and a cluster display-context seam, not full AltScreen.
* Current docs describe MHI2Q displayable/context composition and encoder routing; those facts cannot establish that MPR3's similarly named displayables or encoder methods behave identically.

P3695 differs materially in the observed evidence: AArch64 Linux binaries, different AirPlay build (`AirPlay/320.17.7` strings), different display-manager/encoder configuration, and no explicit AltScreen feature-name strings. No MHI3/MHI2Q conclusion has been transferred to P3695 as a fact.

## 12. Proven versus still unknown

| Question | Result | Confidence | Evidence |
|---|---|---|---|
| Does P3695 advertise >1 CarPlay screen? | Observed `getDisplays` path constructs/appends one descriptor; generic API can append more | STRONG EVIDENCE: one advertised entry; UNKNOWN for every hidden path | `dio_manager` `getDisplays @ 0x1db510`; `createDisplaysDictionary @ 0x1d9be0`; `AirPlayInfoArrayAddScreenDisplay @ 0x578f0` |
| Does it handle stream type 111? | Numeric 111 exists; complete SETUP handling is unresolved | UNKNOWN | `libairplay` enum sequence `0x45eec-0x45f24`; screen opcode compare `0x5d360`; unsupported-stream diagnostics |
| Is Route Guidance implemented? | `routeGuidance=%i` is logged, but Apple message registration/parsing/forwarding is unproven | UNKNOWN; strong negative evidence for configured 0x5200 family | `smartphone_integrator`, `iap2connectionmanager`, configs and strings |
| Can CarPlay renderer target another displayable? | Abstraction/config fields are parameterized; stock P3695 path selects 93 | PLAUSIBLE, not proven | `gal.json`; `ScreenStreamStartSession @ 0x5e0b0`; CESOGfxVideo symbols |
| Can cluster encoder consume arbitrary displayables? | Generic `getDisplayable`/`feed` path exists; arbitrary-ID policy unknown | PLAUSIBLE, not proven | `videoencoderservice` strings; `videoencoderservice.json` |

## 13. Candidate integration seams (research ranking only)

1. **Existing displayable/IPTE -> cluster encoder selection — highest evidence.** The encoder explicitly resolves a displayable and feeds it. Before use, prove the exact ASI method signatures, the cluster display ID, arbitrary-ID acceptance, context/composition semantics, and safe concurrent ownership.
2. **Second CarPlay decoder/renderer instance — medium evidence.** AirPlay/GAL classes expose reusable decoder and renderer APIs, and width/height are parameters. Before use, prove object ownership/lifetime, a second displayable binding, independent stream dispatch, and frame synchronization.
3. **Existing cluster displayables 40/26/29 — low evidence.** Names suggest navigation roles, but ownership and composition are not established. Before use, identify creation callers and runtime surface producers.
4. **AirPlay negotiation extension — currently weak evidence.** `AirPlayInfoArrayAddScreenDisplay` and numeric stream vocabulary exist, but P3695's observed advertisement path is one-entry and explicit AltScreen feature keys are absent. Before use, recover `/info`/SETUP dictionary construction and the complete stream dispatcher, including type 111.
5. **Route Guidance path — separate and not an AltScreen seam.** The boolean-like log string is insufficient. Before use, identify its source and prove Apple's message registration/parser/forwarding behavior.

No candidate was implemented or patched.

## 14. Exact next reverse-engineering steps

1. Recover the private AirPlay SETUP dispatcher around the calls that feed `AirPlayReceiverSessionScreen_Setup`, including all stream-type branches and the source of the `Unsupported stream type` logs.
2. Trace all callers of `AirPlayInfoArrayAddScreenDisplay`, `AirPlayCreateModesDictionary`, and `AirPlayGetFeatures` using relocation/import xrefs and a disassembler that understands AArch64 ELF relocations.
3. Decode the three `CAirPlayHostThread::setScreenProperties` lambda targets at `0x1f7440`, `0x1f74d8`, and `0x1f7578` to name the exact server properties and determine whether any is an alternate/private display descriptor.
4. Locate the runtime addresses of the `displayUUID` CFString and the display dictionary keys, then reconstruct the dictionary fields passed to iOS.
5. Enumerate ASI/RPC method IDs in `videoencoderservice` and locate every client of `asi.VideoEncoding.IVideoEncoding` in all extracted trees. Confirm parameter order and display-ID mappings.
6. Extract the P3695 displaymanager service/client binaries if present in the TFFS images and trace displayable creation/attachment for IDs 26, 29, 40, 42, 60+, and 93.
7. Resolve `routeGuidance=%i` xrefs in both producer binaries and inspect nearby coding/FEC/iAP2 identification code; separately search raw relocations for 0x5200-0x5204 in both endian forms.
8. Compare a second MPR3-family firmware image byte-for-byte at the relevant binaries/configs; do not treat P3695 as vehicle-firmware identical.
9. If static dispatch remains ambiguous, use controlled runtime logging on authorized bench hardware only after the architecture is proven; do not execute target binaries on the host.

## Evidence discipline

“AltScreen unsupported” is not concluded from absent strings. The defensible P3695 statement is narrower: explicit AltScreen identifiers were not found in the inspected artifacts; the observed display advertisement is one-entry; type-111 handling remains unresolved; and the downstream encoder is generically displayable-oriented. Native secondary-video support therefore remains UNKNOWN.
