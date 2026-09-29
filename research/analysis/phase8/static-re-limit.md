# Static RE limit decision

**STATIC_LIMIT_REACHED** for proving the **iOS type111 trigger** from the same
P3695 head-unit artifacts. This is a bounded conclusion, not a declaration that
all P3695 reverse engineering is complete.

## Why this is an external limit

The available code supplies identification bytes, display dictionaries and
AirPlay response serialization. It accepts client identification feedback and
incoming SETUP. Phase8 traced the relevant builders, generic property path,
feature getter, screen setup consumers and identification start/accept/reject
branches, and inventoried all1188 ELF files in the extracted root/opt images
for additional display producers/imports. The recovered acceptance branch is
not a report of negotiated alternate-screen features; rejection data is not a
successful phone capability interpretation.

The missing sufficiency/necessity predicates belong to the phone: whether it
recognizes parent24+child17, which second-display fields/identity it accepts,
which capability gates it combines, and whether/when it chooses111. No relevant
iOS implementation or observed phone exchange is contained in the supplied
P3695 images. A head-unit helper that serializes supplied fields cannot answer
those predicates. Historical reports describe different clients/schema and
cannot substitute for the missing target-client fact.

## What further static work could still answer

| Residual local question | Exact remaining artifact/path | Why it does not close the client trigger |
|---|---|---|
| Original HMI inputFeatures/geometry writer and coding | `/hmi/bl/product.cds` compiled non-ELF data; DSI startService variants in EMS metadata; current deserializer0x2e5500 | Can identify local values/selection, not phone acceptance or sufficiency |
| Complete provenance of server features property | Any remaining setter feeding CFObjectGetPropertyInt64Sync used by AirPlayGetFeatures0x49d40 | Can refine advertised input, not client's interpretation of the mask |
| Lower iAP2 IPC scheduling/retransmission ownership | Connection proxy after dio deploy0x258088 / iap2connectionmanager transport | Can refine delivery/lifetime, not an alternate-screen decision |
| Runtime symbol binding and displays extra reference | Active process/helper binding and refcount observations | Actual binding/refcount behavior is external to static constructor arithmetic |

The first three remain **UNKNOWN / potentially useful further static RE** for
their own local contracts. They are not represented as exhaustively closed.
They also do not justify broadening Phase8 after its focused input blockers.
CONTINUE_STATIC_RE would be appropriate if the needed fact were an unexamined
local serializer or display constructor; the decisive missing validator and
trigger here are client-side.

The CF input adapter can independently become READY_FOR_CF_ADAPTER. Advertisement
and iAP2 adapters remain MORE_RE_REQUIRED. A later, separately authorized
evidence stage needs the minimum observations in the
[document-only plan](future-runtime-evidence-plan.md). This phase authorizes
none of that runtime work.
