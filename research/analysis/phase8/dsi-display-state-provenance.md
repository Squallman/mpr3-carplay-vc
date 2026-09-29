# DSI display/input state provenance

## Resolved local provenance

**STRONG EVIDENCE:** ServiceConfiguration+0x60 is the DSI **inputFeatures**
field, not a recovered secondary-display state. Its wire/storage field is
32-bit; startService reads only its low byte into manager+0x158.

| Stage | Writer / reader | Width and lifetime | Evidence |
|---|---|---|---|
| Construct received configuration | Generated DSI dispatcher zeroes +0x60/+0x64 at 0x2daba4 | uint32 default zero in newly allocated configuration | PROVEN (stores) |
| Populate field | deserialize 0x2e5500; field address +0x60 at 0x2e5a0c, virtual integer read 0x2e5a18 | uint32 from DSI input stream | PROVEN (arguments) |
| Dispatch received config | 0x2dabd8 calls deserializer; 0x2dabfc calls service listener | Owned smart-pointer config passed through DSI registration path | STRONG EVIDENCE |
| Manager default | Constructor store 0x1e3104 | Byte zero; manager lifetime | PROVEN |
| Normal manager update | startService 0x217268 ldrb config+0x60; 0x21726c strb manager+0x158 | Truncated to low eight bits; service configuration snapshot | PROVEN |
| Test override | EKey182 branch; E198 read at 0x2175c8, uxtb 0x2175d0 → same store | Config-selected test byte; not observed runtime state | PROVEN |
| Display consumer | getDisplays 0x1db55c, extraction 0x1db568–0x1db574 | Low bits0..3 passed as four booleans | PROVEN |
| HID consumer | getHIDDevices 0x1dab70, byte read 0x1dac00 | Same manager input-feature byte used by input-device construction | STRONG EVIDENCE |
| Descriptor mapping | createDisplaysDictionary 0x1d9c58–0x1d9c94 | Boolean inputs produce descriptor masks2,8,4,16 | PROVEN (bit operations) |

The stock start-service diagnostic at dio 0x338e10 calls the field
`HID interface` and prints “1 - Knob, 2 - low fidelity, 4 - high fidelity,
8 - touch pad”. The actual EMS table's startService variants name the field
`inputFeatures`, followed by physical touchpad sizes and `primaryInputFeature`.
Together with the four-bit extraction and HID readers, this is **STRONG
EVIDENCE** of input capability semantics. It does not identify any of the
separate AirPlayGetFeatures mask bits as AltScreen.

## Store coverage and false positives

The direct byte stores at manager+0x158 recovered in dio are constructor zero
and the shared normal/test startService store. Other matches were checked by
base-object provenance: 0x145690 writes a shutdown job object, 0x172c0c clears
a communications call-state object, and stack fields are unrelated. The apparent iAP2 reads at 0x1732f4,
0x178590, 0x1ecb98 and 0x1ed198 belong to CCallStateUpdates objects; the
last two load from the incoming call-state argument, not CDIOManager. Matching
the offset alone would have produced a false iAP2 capability bridge.

The current serializer at 0x2e3950 and deserializer establish an IPC field
contract, not the original HMI computation. The full-image ELF inventory found
the ServiceConfiguration type-name use only in dio. EMS metadata confirms DSI
method variants but includes older signatures; it is not an implementation of
the sender.

## Precise remaining unknown

The original HMI/DSI client writer and coding/variant selection are **UNKNOWN**.
`/hmi/bl/product.cds` exists as non-ELF compiled data, and earlier-version EMS
variants are present. That upstream resource has not been exhaustively
decompiled. Further static analysis could refine this local provenance; no
STATIC_LIMIT_REACHED claim applies to that writer. No observed iAP2 acceptance,
rejection or identification producer writes this manager byte. Absence of that
edge in inspected code is not a firmware-wide proof of absence.

Even recovering the sender would describe local input configuration, not
necessarily the phone's type111 decision. See [causality scope](static-re-limit.md).
