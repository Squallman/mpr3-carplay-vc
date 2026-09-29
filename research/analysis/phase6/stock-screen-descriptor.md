# Stock P3695 screen descriptor

`dio_manager:0x1db510` calls `createDisplaysDictionary` once and returns an
array containing exactly one dictionary (`0x1d9d30-0x1d9d38`). The configured
local target is:

```text
display=Driver_Display
displayable=Displayable_External_Smarthphone
displayableID=93
layer=Layer_Media_Base
width=1540 height=720
physicalWidth=235 physicalHeight=110
displayName=Audi MMI
```

`createDisplaysDictionary:0x1d9be0` writes the following recovered fields:

| Key | Value/source | Confidence |
|---|---|---|
| features | boolean-derived mask containing 0x1/0x2/0x4/0x8/0x10 | numeric PROVEN; semantics UNKNOWN |
| widthPixels | width argument, config-derived path | STRONG |
| heightPixels | height argument, config-derived path | STRONG |
| widthPhysical | physical-width argument | STRONG |
| heightPhysical | physical-height argument | STRONG |
| displayUUID | no write recovered in this helper | UNKNOWN |
| modes | not written by this helper | UNKNOWN |
| display name/role | CFString write at `0x1d9d1c-0x1d9d2c`; config correlates it to Audi MMI | STRONG |

No UUID, primary flag, rotation, overscan, or mode dictionary is proven here.
A second descriptor must be a CFDictionary containing whatever mandatory fields
libairplay/iOS require; that minimum is UNKNOWN and must not be invented.
