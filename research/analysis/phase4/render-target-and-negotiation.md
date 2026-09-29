# P3695 Phase 4: render target and negotiation

## Executive result

The stock CarPlay renderer receives target 93 through configuration, but its
exact displayable creator/writer was not proven. Native navigation is different:
navStartup directly calls the display-init API, which resolves a configured
displayable name through LayerConfig and creates an EGL/UBM-backed surface.
The stock HMI/navigation artifacts do not reveal a caller of
setActiveDisplayable, so the active Audi cluster endpoint and the normal encoder
switch remain unknown.

The Phase 2/3 constraint remains intact: AirPlayReceiverSessionSetup accepts
descriptor types only in 100..110. Type 111 reaches the common skip/error path;
it is rejected by stock P3695. This phase did not revisit or weaken that proof.

## 1. Displayable 93

dio_manager.json names Driver_Display, Displayable_External_Smarthphone, layer
Layer_Media_Base and displayableID 93. gal.json independently sets the video
sink renderer displayableID to 93 and esogfx.displayable to the same name.
This is STRONG EVIDENCE that 93 is selected at the renderer boundary.

ScreenStreamCreate in libairplay creates the existing video implementation and
the decoder path. libairplay imports decoder/video implementation symbols but
not libdisplayinit or IpTe display-creation symbols. GAL imports decoder and
VideoSink APIs but no recovered dint_create_displayable call. Therefore the
exact creator and writer of 93 are UNKNOWN; assigning ownership to dio_manager,
GAL, or DisplayManager from configuration would be speculation.

The strongest safe chain is:

  dio_manager.json / gal.json
    -> VideoSink renderer configuration, displayableID 93
    -> ScreenStreamCreate / CESOGfxVideoImpl
    -> decoder factory and video sink feed
    -> UNKNOWN IPTE/displayable registration/writer

## 2. Display registration API

libdisplayinit.so exposes dint_create_displayable, dint_destroy_displayable,
dint_get_surface and dint_set_displayable_properties. Disassembly of
dint_create_displayable at 0x7cc0 calls
mcp::CLayerConfig::getDisplayableId(string const&). An unresolved name logs
displayable could not be resolved to an ID and fails. The successful path stores
the resolved ID and dimensions, creates an EGL/UBM surface, and returns an
object. This proves that this creation route requires a displayable registered
in LayerConfig/display configuration; an arbitrary new numeric ID is not enough.

libipte provides lookup by name and by integer through
IpteConnection::getDisplayable. It exposes width, height, format, stride, size
and native-buffer information. No writer/reader ownership policy, process
sharing rule, or cross-process write authorization was proven.

## 3. Native Audi navigation

Audi application_configuration.json maps:

  cluster_map -> Displayable_Cluster_Map, DSI instance 1
  cluster_overlay -> Displayable_Cluster_Map_Route_Guidance, DSI instance 1
  hud -> Displayable_Hud_Map, DSI instance 2

It also supplies the view geometry and cluster guidance resolutions. The file
does not select the video encoder endpoint.

navStartup has a direct call to dint_create_displayable at 0x69e2bc, a destroy
call at 0x69dd0c, and calls to dint_get_surface and EGL/context/swap functions.
The displayable name is indirect through a navigation object, so the exact
association of that call with IDs 40, 42, and 60 is UNKNOWN. It is nevertheless
PROVEN that native navigation creates and renders at least one configured
displayable through display-init, rather than merely naming it in JSON.

The navi-fpk JS config uses displayable name, display name, layer name and
source/target rectangles. HMI lookup calls are present, but no literal
setActiveDisplayable or setDisplayable call was found in the inspected HMI JS,
navStartup, or navPatch. The stock navigation caller of the video encoder API is
UNKNOWN.

## 4. Cluster endpoint

videoencoderservice.json defines CLUSTER, MINOR_CLUSTER and HUD pipelines with
separate MOST/Ethernet settings. displaymanager.json defines physical display
IDs 1 and 80 plus virtual MOST/Ethernet IDs 90 through 95. EMS metadata defines
several endpoint enum families. None of the inspected stock callers proves
which branch is active in this Audi P3695 runtime.

Therefore the exact endpoint and displayID required by stock Audi navigation are
UNKNOWN. In particular, “MOST” is not asserted solely from the existence of
videoovermost.json.

## 5. iAP2 and AirPlay input negotiation

P3695 emits/configures a WirelessCarPlayTransportComponent with identifier 3.
The identification builder has wireless transport component code and the
standard component records. No ThemeAssets named class, string, or config key
was found. Historical MHI2Q AltScreen material describes a ThemeAssets and
enabled-features gate, but it is comparative evidence only and does not prove
the same wire IDs on MPR3.

AirPlay getDisplays builds one observed screen descriptor from the Driver/
external-smartphone/93 configuration. A generic array-append helper and
displayUUID/modes concepts exist, but no second descriptor is proven. AirPlay
features OR the unresolved mask 0x6104040280. No bit meaning can be assigned
locally.

The exact advertisement needed to cause iOS to send type 111 is UNKNOWN. The
safe architectural requirement is layered: a compatible iAP2 capability if
required by this generation, an AirPlay screen/features advertisement, a SETUP
dispatcher accepting 111, and independent screen state. These layers must not
be treated as interchangeable.

## 6. Can the stock renderer be reused?

The existing screen context and ScreenStream are per-instance by structure and
allocation, and the earlier phases found no blocking global singleton. Thus the
decoder/stream portion is REUSABLE WITH SECOND INSTANCE in the architectural
sense. The renderer target is UNKNOWN: configuration has a per-sink target,
but a second runtime instance and its displayable registration/writer were not
proven. IPTE creation is REUSABLE WITH SECOND INSTANCE only when the target
displayable name is present in LayerConfig and dimensions are valid.

This is PARTIALLY, not fully, reusable. Changing the 100..110 range alone would
not provide a valid second path: dispatch, capability advertisement, transport,
decoder, render target, cluster selection, coexistence and teardown are all
required.

## 7. MHI2Q comparison

| Requirement | MHI2Q historical solution | P3695 equivalent | Status |
|---|---|---|---|
| iOS capability advertisement | ThemeAssets/iAP2 gate described in history | Wireless CarPlay component; no ThemeAssets proof | UNKNOWN |
| stream type 111 | accepted by historical AltScreen path | rejected by SETUP range | SMALL HOOK REQUIRED |
| H.264 interception | screen stream processing | type 110 ScreenStream exists | REUSABLE WITH SECOND INSTANCE |
| second decoder | separate secondary stream path | decoder factory is present; concurrency unproven | UNKNOWN |
| second surface | secondary displayable/composition | display-init can create configured names | REUSABLE WITH SECOND INSTANCE |
| cluster surface selection | cluster composition/encoder seam | encoder API exists; caller/endpoint unknown | UNKNOWN |
| cluster video encoding | existing encoder pipeline | displayable ID -> getDisplayable -> feed | AVAILABLE AS-IS |
| lifecycle | start/stop secondary session | no type111 lifecycle | NEW SIDECAR REQUIRED |
| fail-open | historical hook design | not present for type111 | UNKNOWN / NEW SIDECAR REQUIRED |

## 8. Feasibility decision

An LD_PRELOAD/shared-library hook plus a secondary owner/sidecar is technically
plausible, but not proven sufficient. A hook only in dio_manager cannot make
stock libairplay accept type 111 unless an interposable SETUP/session seam is
found. The technically required modification boundary is the AirPlay dispatch
and capability path, not merely cluster selection. If those functions can be
interposed, a sidecar could own the second context and use existing IPTE/video
services. If not, libairplay replacement/interposition is technically required.

## 9. Evidence status table

| Question | Result | Confidence | Evidence |
|---|---|---|---|
| Who creates/writes 93? | Exact owner unknown; target selection is config-driven | UNKNOWN / STRONG EVIDENCE | dio_manager.json, gal.json; no create import in libairplay/GAL |
| Can second renderer target another displayable? | Architecturally plausible, runtime policy unknown | PLAUSIBLE | per-sink target fields; displayinit LayerConfig resolution |
| How are native cluster displayables created? | navStartup calls dint_create_displayable and renders via EGL | PROVEN | navStartup 0x69e2bc; Audi application config |
| Who calls setActiveDisplayable? | No caller proven | UNKNOWN | HMI/nav scans and TFFS client hits |
| Exact Audi endpoint/displayID? | Unknown | UNKNOWN | videoencoder/displaymanager/EMS config only |
| What makes iOS send type 111? | Exact P3695 gate unknown | UNKNOWN | iAP2/AirPlay evidence separated |

## 10. Newly extracted artifacts

  navigation/config/audi/application_configuration.json
  navigation/bin/navStartup
  navigation/bin/navPatch
  hmi/ui/app/main.17a1455a44197c571024685...js
  hmi/ui/app/navi-fpk-fpk-navi-module-ngfactory...js

Exact paths and SHA-256 values are recorded in
audi-navigation-displayables.txt and the investigation notes. No firmware
image was modified and no tracked repository source was changed.
