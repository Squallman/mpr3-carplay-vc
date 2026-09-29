# Display UUID role and lifecycle

Final requirement classification: **UNKNOWN**. Neither UNIQUE_REQUIRED nor
STABLE_REQUIRED nor SPECIFIC is established for a second descriptor.

| Finding | Evidence | Confidence |
|---|---|---|
| Stock value | Inline constant CFString in dio at 0x32afd8: e5f7a68d-7b0f-4305-984b-974f677a150b | PROVEN |
| Origin | createDisplaysDictionary inserts that static object at 0x1d9d2c | PROVEN; no config/generator on this producer |
| Lifetime | Retaining dictionary insertion, array owns dictionary; constant CFString retain/release are no-ops in this CFL runtime | STRONG EVIDENCE |
| Exposed form | Stock descriptor is serialized as part of /info displays | STRONG EVIDENCE |
| Library helper | AirPlayInfoArrayAddScreenDisplay takes caller's object and sets `uuid`, with no uniqueness/type validation | PROVEN (instructions) |
| Same literal in libairplay | CFUtilsTest within 0x8d120..0x91530 uses constant at 0x116e20; xref 0x8ff54 | PROVEN; diagnostic evidence, not an active screen match |
| Other `uuid` key uses | HID report 0x57340 and HID descriptor 0x574d8 | PROVEN; shared key does not imply shared display identity |
| UUID → streamConnectionID | SETUP reads independent integer key 0x107330; inspected screen path has no descriptor-UUID lookup | UNKNOWN relationship; no mapping recovered |
| UUID returned by phone / compared for screen selection | No active display-UUID consumer found in inspected /info/SETUP/screen branches | UNKNOWN client contract |

Generic StringToUUIDEx, CFGetUUIDEx, CFDictionaryGetUUIDEx and UUID conversion
utilities exist. Their presence is not a display selector. The actual display
key xrefs resolve to builders/HID use, rather than a recovered screen selection
comparison. No code in the inspected display path ties this UUID to an IPTE
displayable ID, physical endpoint or cryptographic streamConnectionID.

A repeated stock value shows stability in this producer, not that a client
requires stable identity. Similarly, a helper accepting arbitrary CF objects
does not prove arbitrary or duplicate UUIDs are valid at iOS. Whether a second
entry needs unique/stable identity, and whether that identity is echoed in
SETUP, are client/runtime questions after the available head-unit construction
and consumer paths. Those external facts reach **STATIC_LIMIT_REACHED** in
this scope; the target requirement itself remains UNKNOWN. No second UUID
was chosen or generated.
