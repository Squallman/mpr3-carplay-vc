# P3695 Phase 3: secondary-screen architecture

## Scope and evidence grading

This is read-only static analysis of P3695 AArch64 ELF files, configuration,
and TFFS contents. Target firmware executables were not run. Conclusions are
marked PROVEN, STRONG EVIDENCE, PLAUSIBLE, UNKNOWN, or DISPROVEN. Phase 1 and
Phase 2 conclusions are constraints; this report does not revise the proven
type-111 rejection.

## Executive summary

1. PROVEN: the type-110 screen is owned by a separately allocated
   AirPlayReceiverSessionScreen object of allocation size 0x340. Its +0x328
   field is one direct ScreenStream pointer; +0x320 is one socket fd.
2. PROVEN: StartSession creates one ScreenStream into +0x328, configures one
   delegate/transport/render chain, starts it, and teardown releases it.
3. STRONG EVIDENCE: the lower-level ScreenStream and video implementation are
   factory/instance based, not demonstrated singletons. Reuse for type 111 is
   technically plausible only with a second screen context and independent
   state. Session-manager ownership of two contexts remains UNKNOWN.
4. PROVEN: the cluster video RPC method signature is
   setActiveDisplayable(displayID, displayable:i32). The EMS table defines
   separate display-ID enum variants for cluster/HUD and MOST/Ethernet cluster
   endpoints.
5. UNKNOWN: no native navigation process was proven to call that RPC with
   displayable 26/29/40/42/60. Diagnostic vesdt proves a generated client
   exists, but is not a native-navigation owner.
6. PROVEN: videoencoderservice performs ID lookup through
   IpTeConnection::getDisplayable, validates dimensions, and feeds the
   resulting displayable to the encoder.
7. PROVEN: displaymanager configuration names IDs 26, 29, 40, 42, 60 and 93.
   UNKNOWN: names/configuration do not prove creator or writer process.
8. PROVEN: AirPlayGetFeatures ORs 0x6104040280; meanings of all seven literal
   bits remain UNKNOWN from local evidence.
9. DISPROVEN as a sufficient design: changing only the SETUP range check cannot
   provide a second screen. Independent context, decoder, renderer, lifecycle,
   capability advertisement, and cluster-output selection are also required.

## 1. Type-110 screen lifecycle

AirPlayReceiverSessionScreen_Create at libairplay.so:0x5c950 executes
calloc(1, 0x340), initializes screen+0x320 to -1, clears +0x33c, and returns
the pointer through an output argument. PROVEN: the screen state is a
separately allocated nested object, not the generic session object itself.

| Offset | Observed role | Evidence |
|---:|---|---|
| +0x000 | SETUP descriptor/type-derived value | Screen_Setup @ 0x5cf10 |
| +0x024, +0x038 | StartSession zeroed state | StartSession @ 0x5e0b0 |
| +0x040 | copied floating-point initialization value | StartSession @ 0x5e104 |
| +0x0c8 | AES/crypto context | security setup calls |
| +0x1e8 | AES initialized/state flag | SetSecurityInfo / teardown |
| +0x1f0 | security-present flag | SetChaChaSecurityInfo |
| +0x2f8 | security key area | 32-byte copy |
| +0x318 | packet/security state | ProcessFrames |
| +0x320 | socket fd | loopback setup, read/close |
| +0x328 | ScreenStream pointer | create/process/stop |
| +0x330 | delegate callback/context | SetDelegate, UpdateState |
| +0x338 | screen state | state comparison/store |
| +0x33c | activity/stop byte | start/process/stop |

The exact source-level type and all padding are UNKNOWN. The allocated size
0x340 and listed offsets are PROVEN.

### Recovered pseudocode

    int Screen_Create(Screen **out) {
        Screen *s = calloc(1, 0x340);
        if (!s) return error;
        s->socket_fd = -1;       // s + 0x320
        s->activity = 0;         // s + 0x33c
        *out = s;
        return 0;
    }

    int Screen_Start(Screen *s, void *delegate_ctx,
                     uint32_t x, uint32_t y, uint32_t width,
                     uint32_t height, uint32_t fps, void *delegates) {
        s->field24 = 0;
        s->field38 = 0;
        s->field40 = startup_constant;
        if (OpenSelfConnectedLoopbackSocket(&s->socket_fd)) goto fail;
        if (ScreenStreamCreate(&s->stream, fps)) goto fail;
        ScreenStreamSetDelegateContext(s->stream, delegate_ctx);
        ScreenStreamSetMaxFPS(s->stream, fps);
        ScreenStreamSetWindowOffset(s->stream, x, y);
        ScreenStreamSetWidthHeight(s->stream, width, height);
        ScreenStreamCopyDelegates(s->stream, delegates);
        s->activity = 0;
        return ScreenStreamStart(s->stream);
    fail:
        Screen_Stop(s);
        return error;
    }

    void Screen_Stop(Screen *s) {
        if (s->stream) {
            ScreenStreamStop(s->stream);
            CFRelease(s->stream);
            s->stream = NULL;
        }
        if (s->socket_fd >= 0) close(s->socket_fd);
        finalize_security(s);
    }

ScreenStreamCreate @ 0xf7360 calls _CFRuntimeCreateInstance with size 0x70,
zeroes the object payload, calls IVideoAbstraction::createVideoImpl(), stores
the video object at ScreenStream+0x70, and configures it via its vtable.
PROVEN: this is an allocating factory, not a singleton accessor.

ScreenStreamProcessData is reached from ProcessFrames @ 0x5d070 through the
ScreenStream pointer around 0x5d6c8. ProcessFrames also reaches SetWidthHeight
around 0x5d9cc and SetAVCC around 0x5dd48. StopSession @ 0x5df90 calls
ScreenStreamStop, CFRelease, clears +0x328, closes +0x320, and finalizes crypto.

Writes/reads to +0x328 in the screen lifecycle:
0x5e11c, 0x5e140, 0x5e14c, 0x5e158, 0x5e168, 0x5e178, 0x5e18c;
0x5d274, 0x5d6c8, 0x5d9cc, 0x5dd48; 0x5dfb8, 0x5dfc4, clear at 0x5dfcc.
Other whole-ELF matches can be unrelated object/stack offsets.

## 2. Requirements for a hypothetical type-111 screen

| Component | Reuse classification | Reason |
|---|---|---|
| SETUP dispatcher | REUSABLE WITH SEPARATE INSTANCE | Current dispatcher rejects 111 |
| Screen_Setup | REUSABLE WITH SEPARATE INSTANCE | Operates on a supplied screen object |
| Screen context | REUSABLE WITH SEPARATE INSTANCE | calloc(0x340), direct fields |
| ScreenStream | REUSABLE WITH SEPARATE INSTANCE | CFRunTimeCreateInstance |
| socket/transport | REUSABLE WITH SEPARATE INSTANCE | socket stored per context |
| decoder/cache | REUSABLE WITH SEPARATE INSTANCE | gfx::Decoder::create and CDecoder allocations |
| delegate/video callback | REUSABLE WITH SEPARATE INSTANCE | copied into stream/context |
| display target | UNKNOWN | stock target is 93; alternate target path unproven |
| session manager | UNKNOWN | one screen pointer observed; multi-context manager not recovered |
| global/static state | UNKNOWN | no blocking singleton proven |

Stock type-110 code can therefore be reused PARTIALLY, not AS-IS. A valid
secondary screen must coexist with type 110, including separate socket, stream,
decoder, callbacks, target, start/stop and error handling.

## 3. Feature mask 0x6104040280

AirPlayGetFeatures @ 0x49d40 obtains a configuration-derived value and ORs
0x6104040280. The literal sets bits 7, 9, 18, 26, 32, 37 and 38. The complete
numeric table is in feature-bits.txt.

UNKNOWN: no local code path proved the semantic meaning of any bit or
identified an AltScreen bit. The type-111 rejection is a separate SETUP fact.

## 4. Cluster video encoder API

The authoritative local schema is
esofw/share/ems_tables.zip!/asi.VideoEncoding.ems_table.json. It explicitly
describes setActiveDisplayable(displayID, displayable:i32).

The operation appears as interface variants with method IDs 0, 3, 6 and 9.
The enum values are recorded in videoencoding-clients.txt. This is PROVEN RPC
contract evidence, including argument order.

vesdt contains generated proxy symbols, the usage string
"Usage: versedt set_displayable DISPLAYID DISPLAYABLE", and
call_struct_DisplayID_I32IVideoEncodingRequest. This proves a client-side
proxy shape, but no native-navigation call site with a constant displayable
was proven. The normal native cluster caller and active display ID are UNKNOWN.

The service-side path is PROVEN from Phase 2: integer displayable ID ->
IpTeConnection::getDisplayable() -> dimension checks -> encoder creation ->
CEncoder::feed(IpteDisplayable). No whitelist, display-class check, or owner
check was proven. Arbitrary runtime policy remains UNKNOWN.

## 5. Displayable ownership

displaymanager.json supplies names:
26 Map_Cluster; 29 Guidance_Cluster; 40 Cluster_Map;
42 Cluster_Map_Route_Guidance; 60 Hud_Map; 93 External_Smarthphone.

Only 93 has a direct relationship to the stock CarPlay renderer configuration.
The full table is in displayable-ownership.txt. No newly extracted binary
proves a creator/writer for 26, 29, 40, 42 or 60. Configuration naming and
encoder consumption are not ownership evidence.

libdisplayinit.so exposes generic displayable creation and lookup functionality.
That proves an API exists, not that it creates any target navigation ID.
Diagnostic/test binaries are not treated as stock owners.

## 6. TFFS scan and additional artifacts

The scanner read both ivi_root_C3.img and opt_ivi_C3.img directly using the
existing local TFFS parser. It reported image, path, signature and file size in
tffs-signature-hits.txt without extracting non-hit files.

Newly extracted high-value artifacts and SHA-256:

    c088f976f27743f4bf30fb02e05ae99c8efed67e313a451e22b48f6818de18a2  mcp_viwibridge
    1e29883ec04bafefa0a9a33ce4bd42ebbc9393b1489d6398bafb402459f6e809  encoderal_spec
    d9b56c5367cdc76ee5ac755a39db01718f607180b84590ddf9b07f59aa9445f6  vesdt
    2715fc09acd8169c689aed7a980ee23f4c98c25f03346e4217756d0938f182fa  dmdt-ivi
    b013278937c5305f1ba225d275cfefb56f940e1f8e4d0adfef25ff0d27432967  encoding_poc
    240a6fe97b2e34fe6786bdd044751de53074b1b275d54f3985307a23f6088938  mcp_comboeventclientsystem
    f201bbfae94e08eae9e75355c7260deab3bbf5c38f1422e13ab247352f50b265  libdisplayinit.so
    c620aa762d0fb45e3b674892a13b63b552935cffe7d89e233b29f483c44f3ff5  libdecoder_al.so
    0e11455ac3dea345ff212fa3108c8adc035eb849fd0afbfb39cf40cd39994ecd  libipte.so
    51c06e1ef3acb4ff65303e11af25ba9406d2ef2869473b106a1774684ed42cbd  liblayerconfig.so
    9b3982221c1073203e606d5b3d89a60458def018c7ba1340b0bb9c2e29c732a9  ems_tables.zip

## 7. MHI2Q comparison

| Requirement | MHI2Q comparative material | P3695 equivalent | Gap status |
|---|---|---|---|
| iOS capability advertisement | historical AltScreen/iAP2 gates | feature mask exists; no AltScreen meaning proven | UNKNOWN |
| stream type 111 acceptance | required second SETUP case | dispatcher rejects >110 | MISSING / PROVEN |
| H.264 frame interception | second screen stream path | type-110 ScreenStream path exists | PARTIAL |
| second decoder | separate decoder path | decoder factory is instance-based | PLAUSIBLE |
| second rendering surface | separate displayable/surface | stock target 93; alternate target unproven | UNKNOWN |
| cluster surface selection | cluster compositor/encoder integration | ID-based encoder API exists | PARTIAL |
| cluster video encoding | selected surface to encoder | service consumes displayable IDs | PROVEN mechanism, caller UNKNOWN |
| lifecycle/start-stop | independent AltScreen lifecycle | type-110 lifecycle is single-context | PARTIAL |
| fail-open behavior | main screen survives secondary failure | no type-111 branch/lifecycle | MISSING |

The evidence supports a medium sidecar/reimplementation as the current working
classification, not a small range-check hook and not yet a proven major
replacement. This is PLAUSIBLE because decoder and encoder primitives are
reusable, while negotiation, ownership, second context, target selection and
native cluster caller remain unresolved.

## 8. Best-supported future architecture (no implementation)

The least speculative architecture is an independent secondary-screen owner
that: adds a type-111 SETUP path while preserving type 110; allocates a second
0x340-style screen context and socket; creates an independent
ScreenStream/decoder/video callback chain; proves a distinct IPTE displayable;
selects the correct cluster video endpoint only after recovering the real
caller and display-ID variant; and starts/stops secondary state independently.

This is an architectural description, not a patch recommendation. It cannot
yet claim that ID 40, 42, 60, or a new ID is a valid CarPlay target.

## 9. Remaining highest-value unknowns

1. Which process invokes setActiveDisplayable during native navigation, and
   with which display ID/displayable ID.
2. Whether the AirPlay feature mask or iAP2 identification advertises the
   required secondary-screen feature.
3. Whether the session manager can own two screen objects concurrently.
4. Exact renderer/IPTE construction argument selecting displayable 93.
5. Creator/writer processes for IDs 26, 29, 40, 42 and 60.
6. Whether videoencoderservice has policy beyond lookup and dimension checks.

## 10. Overall conclusion

CAN STOCK TYPE-110 SCREEN CODE BE REUSED FOR A SECOND SCREEN? PARTIALLY.
Lower-level factories and lifecycle operations are instance-oriented, but the
stock owner has one direct stream/socket state and no proven multi-screen
manager.

IS THERE SINGLETON STATE THAT BLOCKS TWO CONCURRENT SCREENS? UNKNOWN.
No singleton blocker was proven; absence of a blocker is not proof of safe
concurrency.

The strongest positive evidence is an allocating ScreenStream/decoder path and
a two-argument ID-based cluster encoder API. The strongest negative evidence is
the dispatcher hard-rejection of type 111 and the lack of a proven alternate
render target or native cluster caller.
