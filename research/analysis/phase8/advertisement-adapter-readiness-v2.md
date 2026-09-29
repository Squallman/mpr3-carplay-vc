# Advertisement adapter readiness v2

**MORE_RE_REQUIRED**. The structural interception and construction contracts
are stronger, but no safe client-valid second descriptor can yet be specified.

| Requirement | Phase8 result | Confidence |
|---|---|---|
| Interception/binding point | Generic delegate+0x18 / server+0x30; manager `displays` binding selects infoRequestDisplays | STRONG EVIDENCE |
| Callback ABI | Generic server/key/qualifier/error/context → CF object; bound method this/int32 error reference → array | STRONG EVIDENCE |
| Preserving original array/entries | Retaining mutable copy and original descriptor reuse structurally supported | STRONG EVIDENCE |
| Balanced original callback result | Constructor+extra retain−consumer release−dictionary destruction leaves +1; LEAK_SUSPECT | STRONG EVIDENCE (static accounting), UNKNOWN active runtime convention |
| Second descriptor construction | Complete nine-input AddScreenDisplay machine ABI; same eight-key schema, dormant | STRONG EVIDENCE |
| Client minimum / role | Helper emission does not establish iOS mandatory fields or a secondary role | UNKNOWN; external requirement reaches STATIC_LIMIT_REACHED |
| UUID identity rule | Static stock value, no second-display selection/uniqueness validator | UNKNOWN |
| Geometry | DSI window-resolution source recovered; actual second output dimensions not known | STRONG EVIDENCE source; UNKNOWN actual values |
| Lifetime/concurrency | Observed per-request construction; no demonstrated thread safety with service configuration updates | UNKNOWN |

The callback ABI distinction remains essential: infoRequestDisplays is not a
direct delegate member. A future adapter would selectively handle the generic
property call, preserve every other property, preserve all original display
entries and return one explicitly owned result reference matching the consumer.
Its handling of original callback ownership needs a documented policy verified
against the active binding. This phase does not choose one or repair stock.

The client acceptance, identity and trigger requirements cannot be inferred
from setters or generic serialization. The component remains MORE_RE_REQUIRED
while particular client questions reach STATIC_LIMIT_REACHED. It is not ready
merely because the CF SETUP adapter is independently ready. No advertisement
implementation, second UUID, role/feature constant or display geometry was added.

See [reference accounting](displays-reference-accounting.md),
[minimum descriptor](second-display-descriptor-contract.md) and
[future observations](future-runtime-evidence-plan.md).
