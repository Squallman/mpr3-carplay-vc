# Phase8 — ownership, second-display contract and type111 causality

## Executive summary

**READY_FOR_CF_ADAPTER**, with STRONG EVIDENCE for the recovered stock P3695
binary-plist/SETUP path. The parsed request graph uses retaining containers and
copied byte storage. Type110 consumers copy scalars/crypto state; persistent
audio CF values are retained. SETUP's platform commands do not take the generic
queued-params fallback. A future wrapper can release its temporary shallow
request copy after ordinary stock return and use scoped cleanup on unwinding.
No adapter was implemented.

Advertisement remains **MORE_RE_REQUIRED**. AirPlayInfoArrayAddScreenDisplay
was completely reconstructed: nine arguments, same eight display keys, retaining
construction, no recovered stock callers. It supplies no client-valid second
descriptor, UUID rule or main/secondary role. Full callback reference accounting
leaves a net+1 array reference: **LEAK_SUSPECT**, not a proven active runtime leak.

DSI source state is now identified as inputFeatures and configured window
geometry. Wireless parent24 is carried in IdentificationInformation0x1d01;
start/accepted/rejected feedback exists, including rejection retries, without a
recovered alternate-screen interpretation. Generic object-level child17
representation is structurally supported; client relevance remains UNKNOWN.

The external phone validation/type111 trigger reaches **STATIC_LIMIT_REACHED**
for these P3695 artifacts. This does not mean every upstream HMI writer or
feature-property setter is closed. Those local residual questions stay UNKNOWN.

### Evidence scope

Research used read-only local P3695 data. Addresses are image-relative evidence
metadata, never runtime call targets. The principal unchanged sources are:

| Source | SHA256 |
|---|---|
| libairplay.so | `67be0a5639a8e4bb4f55677f26aeb3eadae3d4baf68180db3bcaeb77536b5c28` |
| dio_manager | `1b3257c53c795f2477cd7ce0abc1e2ad22d43d8b001a4b265d74d6f124e2b490` |
| libesoiap2.so | `8df39ce9002bd5744baa9d74f4a307a58d79a5ec3612caee9a903d1de0400507` |
| iap2connectionmanager | `83bfd58b8b677bc4967d7f4a9d36cd05c849564c84ea8c7afe68e9604b36ebce` |

The small selected extraction was supplemented by read-only traversal of
ivi_root_C3.img and opt_ivi_C3.img: 1015+173 ELF files, zero inventory errors.
Display-key/helper candidates occurred only in libairplay and dio; both matched
the selected source hashes. EMS CarPlay metadata was read from
`/esofw/share/ems_tables.zip` in the opt image. Non-ELF product.cds was identified
as an unclosed upstream HMI resource, not claimed exhaustively decompiled.

Reproduction uses installed host objdump symbol/relocation inventories and
bounded AArch64 disassembly ranges, plus ELF PT_LOAD reads for inline CFString
bytes and relocation targets. For example, inspect the complete helper range
0x578f0..0x57a64, parser0x81a80 and recursive helper0x806c0, stock SETUP0x58dc0
and platform0x48410, and DSI startService0x2170c0/deserializer0x2e5500. Temporary
readers, inventories and disassembly references remain ignored under
`.local-research/mpr3/phase8-work/`; no firmware blobs or new tooling were tracked.

## Request lifetime result

**YES** on ordinary return, STRONG EVIDENCE within the inspected stock forwarding
path. Request and streams remain borrowed local values. Screen_Setup0x5cf10
copies latency; SETUP copies streamConnectionID-derived key material and port
state. Worker binding0x57abc carries session state. AudioType retain0x487d4
precedes persistent store0x487dc. Platform setUpStreams/tearDownStreams match
local handlers; other commands can queue borrowed params and must not inherit
this lifetime conclusion. [Complete analysis](request-escape-analysis.md).

## Cleanup/error ownership result

Stock caller initializes its response slot to null, releases request on ordinary
success/error and releases any returned response after serialization. SETUP
does not initialize caller output; success transfers a separate created response,
ordinary failure releases its internal response. Null-output success and C++
exception response balance have stock limitations. The future wrapper owns only
its constructed copy/array/key references, releases them on every construction
failure and normal return, scopes them during unwinding, and leaves responseOut
untouched. [Matrix](setup-cleanup-matrix.md), [parser origin](setup-request-origin.md).

## CF adapter readiness

**READY_FOR_CF_ADAPTER**. Phase7 primitive/key recovery plus Phase8 retaining
parser and downstream ownership close the input contract sufficiently for a
dedicated implementation branch. Exact helper ABIs, content-equal key creation,
typed borrowing, mutation status checks, identity-preserving copy and fail-open
cleanup are specified in [readiness v2](cf-adapter-readiness-v2.md). Runtime
binding, loader policy and concurrency are not asserted ready.

## AirPlayInfoArrayAddScreenDisplay result

At0x578f0, size0x174, machine inputs are array slot(x0), UUID(x1), features(w2),
primary input(w3), maxFPS(w4), pixel width/height(w5/w6), physical width(w7),
physical height(entry stack+0); w0 is status. Creates retaining array if needed,
constructs/sets/appends a fresh dictionary, releases local dictionary. maxFPS0
is omitted; primary input0 is emitted. Allocation errors return-6728; mutation
statuses are discarded. No stock caller/import was recovered.
[Complete ABI and insertion table](airplay-add-screen-display.md).

## Display descriptor producer inventory

Active producer: DIO createDisplaysDictionary0x1d9be0 reached by getDisplays.
Test-HMI variant feeds the same helper. Dormant export: AddScreenDisplay.
CFUtilsTest uses the same UUID as diagnostic text, and HID helpers use `uuid`
for a different schema. No active two-display/cluster descriptor producer was
found in the covered ELF paths. [Coverage and limits](display-descriptor-producers.md).

## UUID contract

**UNKNOWN** requirement. Stock uses constant
e5f7a68d-7b0f-4305-984b-974f677a150b. No proven uniqueness/stability requirement,
phone echo or streamConnectionID mapping was recovered. Helper validation does
not provide one. No second UUID was invented. [Evidence](display-uuid-contract.md).

## Geometry source

Normal path **RUNTIME_STATE**; including test-HMI overrides **MIXED**.
ServiceConfiguration video/window/physical fields feed manager state;
setFrameResolution0x215930 copies window dimensions into advertised frame
dimensions+0x464/+0x468. Actual values remain UNKNOWN. Physical nonzero checks
are service policy, not client minimum-field proof.
[Source contract](display-geometry-source.md).

## Minimum second-display descriptor

**STATIC_LIMIT_REACHED** for client sufficiency. Both builders use features,
maxFPS, widthPixels, heightPixels, widthPhysical, heightPhysical,
primaryInputDevice and uuid, with differing maxFPS/primary-input omissions.
The /info path serializes without a recovered display-specific validator.
“Same schema, unique UUID, non-primary, distinct geometry” remains PLAUSIBLE,
not a target contract. [Field classifications](second-display-descriptor-contract.md).

## Displays callback reference accounting

**LEAK_SUSPECT**: create+1, method retain+1, dictionary insertion+1, callback
result release−1, response destruction−1 leaves+1. No fresh-array cache or
second-owner transfer was found on this path. Allocation and insertion failure
paths do not supply a balancing release. Do not fix stock by double-releasing
its result. [Symbolic accounting](displays-reference-accounting.md).

## Advertisement adapter readiness

**MORE_RE_REQUIRED** independently of CF readiness. Generic delegate+0x18 /
server+0x30 and `displays` registry binding remain exact; direct method-slot
interpretation remains DISPROVEN. Ownership convention, client descriptor/UUID
acceptance and runtime concurrency are not sufficiently established.
[Readiness v2](advertisement-adapter-readiness-v2.md).

## DSI state provenance

ServiceConfiguration+0x60 is uint32 **inputFeatures**, deserialized at0x2e5a18;
low byte copied to manager+0x158 at0x21726c. Constructor default is zero; test
E198 reaches the same store. Low four bits feed descriptor input-feature masks.
Metadata and stock HID diagnostic corroborate meaning. The upstream compiled
HMI sender/coding choice remains UNKNOWN, not a static-limit result.
[Writer/readers and false positives](dsi-display-state-provenance.md).

## Wireless parent24 lifecycle

Prepared identifier/name/IAP2/CarPlay flags serialize into parent24 in message
0x1d01, then control-message deploy/connection proxy. Phone start0x1d00,
accepted0x1d02 and rejected0x1d03 are parsed. Accepted notifies modules;
rejected may trigger retry. None exposes a recovered negotiated alternate-screen
state. [Message and feedback path](wireless-carplay-identification-lifecycle.md).

## Child17 insertion capability

**GENERIC_OBJECT_SUPPORTED** internally: a zero-payload owned parameter record
and view can be added before the nesting routine0x3f410 used at0x434e4; parent
length and data are recomputed/copied. Typed Wireless layout remains fixed0/1/2/4.
External C++ ABI/safe extension integration and client relevance remain UNKNOWN.
No child17 was emitted. [Structural contract](child17-insertion-contract.md).

## Secondary-display gates

No local count>1/secondary-role gate was recovered in the inspected builders,
/info, feature getter, screen creation or identification feedback. Stream-array
count is iteration, not advertised-screen capability. No server feature bit
was newly identified as AltScreen. [Focused audit](secondary-display-gates.md).

## Type111 descriptor structure

Exact common envelope: streams array → dictionary → integer type. Stock screen
evidence adds streamConnectionID, latencyMs and response dataPort behavior;
timingPort belongs to the session envelope. None establishes a complete111
schema, mandatory port/codec/geometry fields or UUID mapping.
[Inventory and classifications](type111-descriptor-contract.md).

## Final static causality chain

```text
parent24 identification
  → phone alternate-capability interpretation: STATIC_LIMIT_REACHED (UNKNOWN fact)
  → local alternate state: UNKNOWN
  → AirPlay features: UNKNOWN
  → second displays descriptor: UNKNOWN dependency
  → iOS SETUP111: STATIC_LIMIT_REACHED (UNKNOWN fact)

DSI input/window configuration
  → stock descriptor: STRONG EVIDENCE
  → /info serialized displays: STRONG EVIDENCE
```

Start/accept/reject feedback does not fill those unknown arrows. Historical
ThemeAssets/altScreen arrows stay HISTORICAL ONLY. Stock111 implementation
remains DISPROVEN. [Per-arrow audit](type111-causality-final-static.md).

## Static RE limit decision

**STATIC_LIMIT_REACHED** for the external iOS trigger, not for every local
writer/property. Further local HMI/property/transport analysis may refine
inputs but cannot prove client necessity/sufficiency without client code or
observations. [Precise residual work](static-re-limit.md).
The [future plan](future-runtime-evidence-plan.md) is document-only: stock
identification, /info and SETUP captures with client/version/timing, active
geometry and callback lifetime observations, plus an independently authorized
known-valid reference/controlled comparison if needed. No runtime work ran.

## Implementation readiness

| Component | Phase7 | Phase8 | Ready? | Remaining blocker |
|---|---|---|---|---|
| CoreFoundation SETUP adapter | MORE_RE_REQUIRED | READY_FOR_CF_ADAPTER | Yes, bounded input implementation | Dedicated implementation/tests; runtime binding/policy/concurrency validation separate |
| Advertisement adapter | MORE_RE_REQUIRED | MORE_RE_REQUIRED; client minimum reaches static limit | No | Result ownership convention, UUID/role/client acceptance, runtime concurrency |
| iAP2 adapter | MORE_RE_REQUIRED | MORE_RE_REQUIRED; generic internal insertion refined | No | External insertion ABI/lifecycle and parent24 child17 semantic relevance |
| type111 interception | Real target behavior unresolved | Still unresolved | No | Trigger and actual111 schema, advertisement, independent target lifecycle/output |
| Secondary pipeline | NOT IMPLEMENTED / incomplete target ABI | Unchanged | No | ScreenStream constructor/configuration and decoder/render lifecycle ABI |
| Displayable adapter | NOT IMPLEMENTED / incomplete target ABI | Unchanged | No | Display-init C ABI, LayerConfig/service variant, safe occupancy/endpoint |
| VideoEncoding adapter | NOT IMPLEMENTED / incomplete target ABI | Unchanged | No | Generated COMM ABI, policy, active endpoint and output restoration |

Only Phase8 reports and justified research index/status/roadmap updates are
changed. Phases1–7, target-contract and production sources are preserved.
No firmware modification, target binary execution, child emission, hook link,
deployment tooling, commit or push is part of this phase.

### Validation

`git diff --check` passed. Untracked Phase8 reports were also checked individually
for whitespace and relative local links. P3695's5620-entry metadata snapshot
(mode/size/mtime) matches startup and all six principal binary/config source
hashes match the baseline. No P3695 file is tracked and no file is staged.
Tracked changes are confined to research/analysis/README.md,
docs/research-status.md and docs/roadmap.md; the20 new files are Phase8 Markdown
reports. No implementation or Phase1–7 report changed. Host builds were not run
for this documentation-only research phase; no target binary was executed.
