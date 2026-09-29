# Display descriptor producer inventory

## Coverage

The selected local `extracted/files` subtree contains only a small subset of
firmware. Phase8 therefore also read the two TFFS images directly, without
modification: ivi_root_C3.img (6405 directory entries, 1015 ELF files) and
opt_ivi_C3.img (18893 entries, 173 ELF files). Entry CRC32C and allocation-chain
bounds were checked by an ignored, read-only local reader; the scan reported
zero errors. Directory entries include links/metadata and are not a count of
unique regular files.

The **1188 ELF** content inventory searched exported/imported helper names,
`createDisplaysDictionary`, exact display-key bytes, the stock UUID and
`altScreen`. Only dio_manager and libairplay matched the display constructor
key bundle. The image copies of both have the same hashes as the selected
disassembled files. The scan is a candidate inventory, not proof from strings.
Actual production/helper classification comes from the call chains below.
It cannot exclude dynamically synthesized key strings or opaque HMI bytecode.

| Producer | Classification | Evidence | Confidence |
|---|---|---|---|
| dio::createDisplaysDictionary 0x1d9be0 | ACTIVE_PRODUCTION | getDisplays calls it at 0x1db608; registry displays property reaches getDisplays during /info | STRONG EVIDENCE |
| Same DIO helper with test-HMI overrides | CONFIG_VARIANT | startService EKey182 branch selects E183–190/E198/E199 data instead of DSI fields | STRONG EVIDENCE; test branch disabled by stock config |
| AirPlayInfoArrayAddScreenDisplay 0x578f0 | UNUSED/DORMANT | Exported complete builder; no stock caller/import/reference found in scanned ELFs | STRONG EVIDENCE within scope |
| CFUtilsTest 0x8d120 using same UUID literal at 0x8ff54 | TEST/DIAGNOSTIC, not a display producer | CF utility test string/accessor exercise, not /info display construction | PROVEN (function bounds/use) |
| AirPlayInfoArrayAddHIDDevice 0x57420 and SendHIDReport 0x572b0 | Different schema, not a display producer | Shared `uuid` key but HID/report fields and paths | PROVEN (call/insertion facts) |

## Multiplicity result

DIO constructs one array and appends one dictionary. No alternative active
producer of two display dictionaries, cluster-sized display, second UUID or
secondary role was recovered. Its conditional omission of primaryInputDevice
does exist, but does not identify a secondary display. Feature values are
calculated from input-feature state, not a recovered screen-count test.

The dormant libairplay helper can append to an existing array repeatedly. This
establishes structural multiple-entry construction support, not an active
two-display path or client acceptance. No stock call sequence using it twice
was found.

Negative findings are **UNKNOWN** for an absolute firmware-wide absence:
non-ELF resources, dynamically generated data and client behavior were not
universally reverse engineered. There is no evidence to label the one-entry
producer a safe minimum second-screen template. See
[helper](airplay-add-screen-display.md), [UUID](display-uuid-contract.md) and
[gates](secondary-display-gates.md).
