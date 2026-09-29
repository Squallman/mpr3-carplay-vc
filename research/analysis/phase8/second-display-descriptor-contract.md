# Minimum second AirPlay display descriptor

Decision: **STATIC_LIMIT_REACHED** for establishing a client-valid minimum
second descriptor from the available head-unit artifacts. The schema is
recovered, but its sufficiency is **UNKNOWN_CLIENT_REQUIREMENT**. This is not
SUFFICIENT_FOR_STATIC_IMPLEMENTATION.

| Field | Stock DIO helper | Library builder / consumer | Classification for a second descriptor |
|---|---|---|---|
| features | Always inserts integer; four input-derived booleans | Builder takes uint32; no display-count rule or semantic validation | REQUIRED_BY_P3695_HELPER; UNKNOWN_CLIENT_REQUIREMENT |
| maxFPS | Always emitted, configured/default60 | Builder omits when input zero | CONDITIONALLY_EMITTED across builders; UNKNOWN_CLIENT_REQUIREMENT |
| widthPixels | Always emitted from window resolution | Builder inserts supplied uint32 without nonzero validation | REQUIRED_BY_P3695_HELPER; UNKNOWN_CLIENT_REQUIREMENT |
| heightPixels | Always emitted from window resolution | Same | REQUIRED_BY_P3695_HELPER; UNKNOWN_CLIENT_REQUIREMENT |
| widthPhysical | Always emitted; service checks nonzero | Builder inserts supplied uint32 without validating dimensions | REQUIRED_BY_P3695_HELPER; UNKNOWN_CLIENT_REQUIREMENT |
| heightPhysical | Always emitted; service checks nonzero | Same | REQUIRED_BY_P3695_HELPER; UNKNOWN_CLIENT_REQUIREMENT |
| primaryInputDevice | Omitted for zero | Builder always inserts, including zero | CONDITIONALLY_EMITTED across builders; UNKNOWN_CLIENT_REQUIREMENT |
| uuid | Static CFString always emitted | Builder inserts borrowed object; no validation/selection mapping | REQUIRED_BY_P3695_HELPER; UNKNOWN_CLIENT_REQUIREMENT |
| role/main/secondary, modes, context, transport, display name | Absent from this helper | Absent from AddScreenDisplay | UNKNOWN_CLIENT_REQUIREMENT, not proven unnecessary |

These classifications describe construction facts, not protocol requirements.
No field qualifies as REQUIRED_BY_LIBAIRPLAY from a recovered display-specific
validator: /info copies the callback object into a dictionary and serializes
CF data without interpreting each display dictionary. Generic plist
serializability is not iOS display validation.

## What the two builders establish

The dormant AddScreenDisplay helper can append another dictionary to an array
and uses the same keys as DIO. This proves a local structural ability to
represent several entries. It does not supply a P3695 production two-entry
example, a non-primary role, a unique/stable UUID rule, geometry selection or
client feedback. No alternate descriptor validator or secondary-specific
constructor was found in the focused producer/consumer paths.

The hypothesis “same schema + unique UUID + non-primary + distinct geometry”
remains **PLAUSIBLE**, not a recovered contract. `primaryInputDevice` is an
input-device field; zero/omission is not a proven main/secondary designation.
DSI inputFeatures are likewise not a recovered AltScreen bit. Duplicating stock
geometry/UUID does not become safe by using the exported helper.

Historical MHI2Q/MHI3 descriptors/gates remain comparative. They cannot identify
which fields P3695's phone client requires. The remaining acceptance and
negotiation decision runs on the phone, whose validator/decision code is not
part of these firmware artifacts. Further examination of the same CF setters
cannot recover that external decision. See [static-limit scope](static-re-limit.md)
and [document-only observations](future-runtime-evidence-plan.md).
