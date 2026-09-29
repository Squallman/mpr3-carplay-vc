# Preload and original-call contract

## ELF result

`libairplay.so` exports `AirPlayReceiverSessionSetup` as a dynamic global text
symbol at `0x58dc0`. `objdump -R` shows:

```text
0x15ebc0 R_AARCH64_JUMP_SLOT AirPlayReceiverSessionSetup
```

The recovered internal call at `0x4c7bc` targets the PLT entry at `0x40480`.
This is the important distinction: an exported symbol alone is insufficient,
but this internal call has a dynamic PLT/JUMP_SLOT path.

The same pattern is present for Screen_Setup, Screen_Create, Screen_Start,
ProcessFrames, ScreenStreamCreate/ProcessData, and AirPlayGetFeatures. No
internal PLT call to `AirPlayInfoArrayAddScreenDisplay` was recovered.

No `DF_SYMBOLIC` or `BIND_NOW` flag was visible in the available `objdump -p`
dynamic-section output. Symbol version sections exist, but no target-specific
version label was recovered. `libairplay.so` does not list `libdl` as a NEEDED
dependency; that does not prevent the hosting process from providing dlsym.

## Classification

**RTLD_NEXT LIKELY VALID, runtime unverified.** A preload definition of
`AirPlayReceiverSessionSetup` should be able to resolve the next definition
with `dlsym(RTLD_NEXT, "AirPlayReceiverSessionSetup")` when the preloaded
object is in the same link-map scope and the target call uses the shown PLT.
The exact loader environment, namespace behavior, and symbol version lookup
remain **UNKNOWN**. No target binary was executed.

The wrapper must use the recovered three-argument ABI. Calling the original
with a guessed four/five-argument signature is not justified.

| Function | Exported | JUMP_SLOT / PLT evidence | Direct local calls observed | Preload result |
|---|---:|---:|---:|---|
| AirPlayReceiverSessionSetup | yes, 0x58dc0 | yes, 0x15ebc0; call 0x4c7bc | no direct call found | likely interceptable |
| AirPlayReceiverSessionScreen_Setup | yes | yes, 0x15da88; call 0x590a8 | no direct call found | likely interceptable |
| AirPlayReceiverSessionScreen_Create | yes | yes, 0x15e070; call 0x52e00 | no direct call found | likely interceptable |
| AirPlayReceiverSessionScreen_StartSession | yes | yes, 0x15d340; call 0x4efd8 | no direct call found | likely interceptable |
| AirPlayReceiverSessionScreen_ProcessFrames | yes | yes, 0x15f4d0; call 0x4f000 | no direct call found | likely interceptable |
| AirPlayGetFeatures | yes | yes, 0x15d210; calls 0x4a090/0x4ad44 | no direct call found | likely interceptable |
| AirPlayInfoArrayAddScreenDisplay | yes, 0x578f0 | no JUMP_SLOT found | not proven | not established |
| ScreenStreamCreate | yes | yes, 0x15ef08; call 0x5e124 | no direct call found | likely interceptable |
| ScreenStreamProcessData | yes | yes, 0x15d9c8; call 0x5d6e4 | no direct call found | likely interceptable |

Nearest stable seam for SETUP is therefore the exported internal PLT call to
`AirPlayReceiverSessionSetup`; a callback-level seam is preferable only if the
delegate ABI is recovered exactly.
