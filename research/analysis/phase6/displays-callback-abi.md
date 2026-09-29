# displays callback ABI

`dio_manager:0x1db6e0` is approximately:

```cpp
const void *CDIOManager::infoRequestDisplays(int &error);
```

x0 is `this`, x1 is an error/status pointer, and the return value is the CFArray
from `getDisplays`. At `0x1db710` it calls `getDisplays`; when `error == 0`,
`CFRetain` is called at `0x1db720` before return. This strongly supports a
retained CFArray callback result, though exact CoreFoundation ownership on all
error paths is UNKNOWN.

`getDisplays` calls `createDisplaysDictionary` and returns one CFArray entry.
The best advertisement seam is therefore the callback result: retain/copy the
stock descriptor(s), append a separately constructed descriptor, and return a
new retained array. The exact delegate slot and replacement mechanics remain
UNKNOWN.
