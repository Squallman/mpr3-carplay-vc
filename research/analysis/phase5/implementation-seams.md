# P3695 Phase 5: implementation-seam proof

## Executive summary

1. A standalone client can use the existing COMM/EMS VideoEncoding client
   machinery in principle. vesdt proves the generated client construction and
   service identity; its command parses two integers and dispatches through a
   proxy vtable. Policy authorization for a new process remains UNKNOWN.
2. `setActiveDisplayable` is a direct displayable-selection operation in the
   video encoder service. The supplied integer is looked up with
   IpTeConnection::getDisplayable, dimension-checked, and passed directly to
   CEncoder::feed. No intermediate DisplayManager compositor was recovered.
3. A sidecar can probably create a configured displayable through stock
   libdisplayinit, but only by name resolution through LayerConfig. No existing
   displayable was proven safe to take over. Displayable_Debug_1 (137) is the
   best current lead, classified POSSIBLE rather than GOOD CANDIDATE.
4. `AirPlayReceiverSessionSetup` is exported and its own library call site uses
   a PLT/JUMP_SLOT relocation. This is the strongest Phase 5 evidence that a
   preload/function-interposition strategy may preserve stock stream 110 while
   handling stream 111 externally. It is static proof of a seam, not runtime
   proof of loader behavior.
5. The AirPlay display-advertisement callback relationship remains less
   certain. `getDisplays` is directly callable and the AirPlay server delegate
   is replaceable through AirPlayReceiverServerSetDelegate, but the exact field
   supplying the /info displays array is not identified.
6. P3695 has the standard WirelessCarPlayTransportComponent serializer, but
   support for historical nested iAP2 parameter 17 / ThemeAssets remains
   UNKNOWN. No named P3695 implementation was found.

## Standalone VideoEncoding client

vesdt uses libcomm, libosal, libiplutil and libiplcommon, constructs an
AgentStarter and ServiceRegistration, and connects a generated proxy to
asi.VideoEncoding.IVideoEncoding. EMS method numbers are documented in
vesdt-set-displayable-callchain.md. This separates “RPC exists” from policy:
the client architecture supports a new executable, but AppArmor/broker access
for a new UID/profile was not proven.

## Displayable writer path

Native navStartup demonstrates the display-init sequence and the name-based
LayerConfig requirement. The minimum recovered graphics shape is:

  display-init setup
    -> dint_create_displayable(configured name)
    -> dint_get_surface
    -> EGL/context/render/swap
    -> dint_destroy_displayable

The CarPlay GAL/libairplay path selects displayable 93 but does not expose this
creation sequence. Therefore 93 creator/writer remains UNKNOWN.

## Candidate policy

The displaymanager configuration contains IDs 1..315 and generic video/debug
slots. The only candidate with a defensible low-reference profile is
Displayable_Debug_1 (137), but static data does not prove it is unoccupied.
Taking any production map, HUD, CarPlay, annotation, or media slot is a
conflict risk. No GOOD CANDIDATE is established.

## AirPlay interposition

The requested libairplay functions are exported. Internal call sites for
AirPlayReceiverSessionSetup, Screen_Setup, Screen_Create, Screen_StartSession,
Screen_ProcessFrames, AirPlayGetFeatures, ScreenStreamCreate and
ScreenStreamProcessData use PLT entries with JUMP_SLOT relocations. The local
SETUP dispatcher call at 0x4c7bc therefore has a plausible dynamic
interposition seam. The best conceptual hook is the parsed-dictionary boundary
at AirPlayReceiverSessionSetup: consume/hide type 111 while forwarding type
110 unchanged to the original.

For advertisement, the best current seam is the AirPlay server delegate or the
unidentified property/callback that supplies getDisplays. Directly interposing
CDIOManager::getDisplays is not proven to affect the /info response.

## Layer classification

| Layer | Classification | Evidence |
|---|---|---|
| VideoEncoding standalone client | STRONG EVIDENCE YES | vesdt generated COMM client |
| VideoEncoding policy for new sidecar | UNKNOWN | enforcer profiles/socket ACLs unavailable |
| Configured displayable creation | STRONG EVIDENCE YES | libdisplayinit + LayerConfig |
| Safe unowned displayable | UNKNOWN | debug slot only a candidate |
| Direct encoder input | PROVEN | getDisplayable -> dimensions -> CEncoder::feed |
| AirPlay Setup hook without ELF modification | STRONG EVIDENCE / POSSIBLE | exported symbol + PLT/JUMP_SLOT |
| AirPlay advertisement hook | UNKNOWN/POSSIBLE | delegate exists; exact displays callback unknown |
| iAP2 subparameter 17 | UNKNOWN | no named implementation or table |

## Proposed architecture arrow status

  smartphone_integrator launches dio_manager                 PROVEN
  stock stream 110 remains stock                              PROVEN if hook forwards it unchanged
  hook observes/intercepts secondary negotiation                PLAUSIBLE
  secondary owner                                             NEW COMPONENT REQUIRED
  independent ScreenStream/decoder                            REUSABLE WITH SECOND INSTANCE
  configured displayable X                                    PLAUSIBLE, subject to registration/ownership
  setActiveDisplayable(cluster, X)                            STRONG EVIDENCE client path; endpoint/policy UNKNOWN
  stock videoencoderservice                                    PROVEN existing consumer
  VC output                                                   PROVEN for a valid active endpoint/input

## Direct answers

### Can a standalone sidecar call setActiveDisplayable?

**STRONG EVIDENCE YES architecturally; policy UNKNOWN.** vesdt is a standalone
generated client using stock libraries. New-process identity/AppArmor/COMM
authorization still require proof.

### Can it create/write a configured displayable?

**PLAUSIBLE, not proven operationally.** libdisplayinit supports creation by
configured name and EGL/UBM surface access. Ownership and safe concurrent
writing are unresolved.

### Best safe existing candidate

**Displayable_Debug_1 (137), POSSIBLE only.** It has no production reference in
the static scan beyond displaymanager.json, but occupancy is unknown. No safe
candidate is proven.

### Does the encoder directly encode the selected displayable?

**YES, PROVEN.** The selected IPTE object is passed directly to CEncoder::feed.

### Can AirPlayReceiverSessionSetup be LD_PRELOAD-interposed?

**STRONG EVIDENCE YES / runtime UNKNOWN.** It is exported and called through a
JUMP_SLOT/PLT entry inside libairplay. Target dynamic-loader behavior was not
tested.

### Best type-111 seam

The parsed-dictionary `AirPlayReceiverSessionSetup` PLT seam. A fallback is the
AirPlayReceiverServer delegate/control path, whose exact callback field remains
unknown.

### Best advertisement seam

Identify and hook the AirPlay server delegate/property callback that supplies
the displays array. Direct getDisplays interposition is not yet proven to be
on the /info path.

### Historical iAP2 parameter 17 / ThemeAssets-like gate

**UNKNOWN.** P3695 has the generic component serializer but no named or tabled
implementation of subparameter 17.

### Minimum required modification boundary

At minimum: an AirPlay SETUP/capability interception seam, an independent
secondary stream owner, a registered writable displayable, and a VideoEncoding
client call. videoencoderservice itself need not be modified if the sidecar can
use the existing service and endpoint. DisplayManager/LayerConfig need not be
modified if an existing unoccupied configured displayable is proven safe.

## Final feasibility decision

The architecture can avoid modifying videoencoderservice in principle and can
preserve main stream 110 if the SETUP hook forwards it unchanged. It cannot yet
be called a complete no-firmware-modification solution because advertisement
interposition, sidecar policy authorization, displayable ownership, active
endpoint selection, and iAP2 gate semantics remain unresolved.

No firmware or tracked source was modified; no target executable was executed.
