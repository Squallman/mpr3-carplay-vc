# Type111 descriptor structure: evidence boundary

No exact P3695 type111 schema was recovered. **UNKNOWN** is the schema result.
The stock SETUP dispatcher has entries for100..110 and no private111 handler.
The common dictionary envelope and stock screen inputs provide preparation,
not permission to reuse them as a complete secondary descriptor contract.

| Field / structure | Classification | Actual evidence | What is not established |
|---|---|---|---|
| Top-level request `streams` array | COMMON_SCREEN_FIELD | Exact CFString0x106610, typed lookup0x58f94, count0x58fa0 | Mandatory for every valid SETUP variant; missing/empty has other stock paths |
| Each streams element dictionary | COMMON_SCREEN_FIELD | Typed lookup0x59018 and common enum loop | Additional111-specific validity conditions |
| `type` integer | COMMON_SCREEN_FIELD | Exact CFString0x107128, Int64 extraction0x59038 | Full111 dispatch/response behavior |
| `streamConnectionID` | TYPE110_ONLY (observed path) | Exact key0x107330; read0x590c8 and crypto derivation | Whether111 requires it and how client chooses it |
| `latencyMs` | COMMON_SCREEN_FIELD (stock generic screen helper) | Exact key0x10a520; Screen_Setup0x5cf54 copies Int64 into context | Applicability/requiredness for111; helper name does not prove it |
| `timingPort` | COMMON_SCREEN_FIELD (session envelope) | Top-level key0x107e10 and setup reads before streams dispatch | A111 descriptor port or a required second timing session |
| `dataPort` | TYPE110_ONLY (stock response/path) | Stock creates screen socket and writes response descriptor port around0x59138–0x59170 | A required input field for111; do not confuse response with request |
| `controlPort`, `eventPort` | UNKNOWN | Other transport paths do not establish a111 schema | Presence, direction, ownership and type111 transport use |
| Codec / video-format selector | UNKNOWN | ScreenStream/decoder configuration exists downstream | An exact111 CF key or requested format contract |
| Width / height | UNKNOWN for111 request | Active stock geometry comes from service/server state; advertisement has pixel keys | That111 SETUP includes those keys or overrides configured geometry |
| UUID / display role | UNKNOWN | Advertised display builder emits UUID; no recovered SETUP UUID selector | Echo/mapping to streamConnectionID or secondary role |
| Separate context/transport/timing IDs | UNKNOWN | Independent screen context is architecturally possible | Actual111 field names/ABI values |
| Historical type111 / enabledFeatures examples | HISTORICAL_TYPE111 | Earlier MHI2Q/MHI3 evidence | P3695/client equivalence |

## Why the generic screen helper is insufficient

AirPlayReceiverSessionScreen_Setup 0x5cf10 reads the latency value and its
configured fallback; it does not parse a complete descriptor protocol. Creation,
socket setup, cryptographic derivation, worker startup, response construction
and teardown live in other session paths. The stock context guard and one
screen slot are not a coexistence contract. Type111 needs independently owned
state and target output/pipeline behavior, all still blocked.

The safe future preparation contract is limited to a typed borrowed dictionary
inside streams, exact integer type identification, and retaining the original
secondary dictionary if a later authorized implementation needs it beyond the
call. No descriptor key other than the directly recovered common/stock keys is
invented. Stock110 dictionary identity must stay unchanged.

Actual type111 request/response bytes and their causal setup need client/runtime
evidence or the relevant client implementation. A helper exported by P3695
cannot establish omitted type111 fields. No real interception was implemented.
