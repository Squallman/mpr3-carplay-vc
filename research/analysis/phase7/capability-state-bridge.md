# Search for an internal iAP2 → AirPlay capability bridge

## Direct result

**UNKNOWN**: no P3695 writer/reader chain connecting iAP2 identification output
to AirPlay feature-mask or displays generation was established.

| Candidate state | Writer | Reader | Lifetime / semantic evidence | Result |
|---|---|---|---|---|
| Manager display feature byte +0x158 | Constructor zero 0x1e3104; CDSICarplayImpl::startService 0x21726c copies ServiceConfiguration+0x60 (unless test configuration branch) | getDisplays 0x1db55c supplies four descriptor bools; related HID/service paths | Manager state persists across /info calls; service configuration supplies value | STRONG EVIDENCE for DSI→display path; iAP2 source UNKNOWN |
| Display metadata manager+0x454 | startService 0x2172ac–0x21730c and following stores | getDisplays reads widths/heights/physical/input metadata | ServiceConfiguration-derived state; disabled test-HMI override also exists | STRONG EVIDENCE; not identification-derived proof |
| Identification Wireless field +0x380 | addIdentificationInformation 0x2637a0–0x26386c | libesoiap2 named formatter and selector-3 composer | Owned identification object, configuration-derived ID/name and two flags | STRONG EVIDENCE; no AirPlay reader found |
| AirPlay server features property | Getter in AirPlayGetFeatures 0x49d40; full writer provenance UNKNOWN | /info at 0x4ad44 / 0x4aea8 | Property plus fixed OR floor; independent display property lookup | No closed iAP2 dependency |
| `extendedFeatures` array | infoRequestExtendedFeatures 0x1d9060 | Generic /info property list | Contains vocoderInfo in inspected construction | No secondary capability bridge |

The DSI source matters: startService receives
`CIPtr<dsi::carplay::ServiceConfiguration> const&`, accesses its underlying
object at x21+8, then copies byte+0x60 to the proven manager pointer loaded
from implementation+0x208. This is a real control/data path, not a name match.
It does not prove who originally sets ServiceConfiguration+0x60 or that this
byte means secondary screen. No bit semantics are assigned from proximity.

## Search coverage and precise missing edge

Static symbol/string inventories covered iap2connectionmanager, libesoiap2,
dio_manager and extdevconfig.json; disassembly followed named identification
producer/serializer, DSI startService, getDisplays, generic AirPlay callbacks
and feature getter. Terms display/screen/secondary/route/feature/capability/
theme/asset/cluster/ui-context were used to locate candidates, not prove them.
No message ID or IPC serializer/deserializer was shown carrying a ThemeAssets
or secondary-screen identification state into that service configuration.

Missing facts: originating writer of ServiceConfiguration+0x60; its IPC/event
contract and per-bit semantics; any writer of the server features property
derived from identification; target/client acceptance of parent24+child17.
Unexamined indirect IPC/event paths prevent concluding that no bridge exists.
The shared process/library topology is not causality. The historical gate
primarily describes **state on iOS**, not this missing head-unit-internal edge.
