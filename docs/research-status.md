# Research status

## Proven

- P3695 is Linux/AArch64.
- Stock CarPlay screen is stream type 110.
- Stock SETUP rejects stream type 111.
- A stock screen context is separately allocated and owns socket/ScreenStream state.
- VideoEncoding directly resolves an integer IPTE displayable and feeds it.
- `setActiveDisplayable(displayID, displayable:i32)` exists.
- `AirPlayReceiverSessionSetup` has strong evidence for `(session, request, responseOut)`.
- The symbol is exported and internally called through PLT/JUMP_SLOT.
- Configured displayable creation resolves a LayerConfig name.

## Strong evidence

- ScreenStream/decoder factories are suitable for separate instances.
- `infoRequestDisplays(int&)` returns a retained display array.
- The stock display helper emits one descriptor with width/height/physical fields and a feature field.

## Plausible

- A preload or equivalent in-process hook could preserve 110 while privately handling 111.
- A separate configured displayable could feed the existing encoder.
- `Displayable_Debug_1` is a possible test candidate, not a safe production choice.

## Unknown

- Exact iOS type-111 trigger.
- P3695 support for historical iAP2 parameter/subparameter 17.
- Exact advertisement delegate slot and complete second descriptor.
- AppArmor/runtime loader authorization.
- Active cluster endpoint.
- Safe displayable occupancy and target concurrent lifecycle.

## Disproven

- Merely changing the 100..110 range check is sufficient.
- Stock P3695 already handles type 111.
- MHI2Q/MHI3 similarly named objects prove MPR3 behavior.

Detailed evidence is indexed in [`research/analysis/README.md`](../research/analysis/README.md).
