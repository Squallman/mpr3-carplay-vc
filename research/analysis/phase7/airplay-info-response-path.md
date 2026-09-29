# AirPlay /info displays insertion path

## Observed control and data flow

STRONG EVIDENCE from libairplay.so and dio_manager call/argument chains:

```text
ServerControl /info URI suffix dispatch
  -> request helper 0x4af50
  -> AirPlayCopyServerInfo 0x4ac10
  -> CDynamicInfoResponse::serverInfoResponse 0x4dd40
  -> setServerPropertyFromCallback 0x4dd20
  -> setPropertyFromCallback 0x4dcb0
  -> AirPlayReceiverServerCopyProperty 0x48d70
  -> PlatformCopyProperty 0x481c0
  -> generic delegate+0x18 / server+0x30
  -> dio s_serverCopyPropertyCallback 0x1df750
  -> infoResponse 0x1df610 -> property-map callback 0x1df410
  -> bound infoRequestDisplays 0x1db6e0
  -> getDisplays 0x1db510 -> createDisplaysDictionary 0x1d9be0
  -> returned array -> dictionary SetValue 0x4dcfc
  -> CFBinaryPlistV0Create -> HTTP body
```

Exact URI literal `/info` @ 0x105400 is passed to strnicmp_suffix at 0x4cb44;
matching branch calls the request helper 0x4cb5c. Helper invokes
AirPlayCopyServerInfo at 0x4b098. The factory CInfoResponse::create @ 0x4e170
unconditionally builds the dynamic response implementation in this image
(GOT 0x15ce40 resolves its vtable). No static variant selection was observed.

AirPlayCopyServerInfo creates a mutable response at 0x4aca0 and passes 16
property keys at 0x4ace0, from pointer list @ 0x158ea0. Exact `displays`
key object is **0x104938**, inline text at +8. Adjacent keys include
audioFormats, audioLatencies, bluetoothIDs, extendedFeatures, hidDevices,
limitedUIElements, limitedUI, manufacturer, model, nightMode and OEM icon
properties. This demonstrates why the delegate slot is generic.

Property helper calls the getter at 0x4dce0 with synchronous flag w1=1,
qualifier=null and errorOut=null. If returned object is nonnull, SetValue
0x4dcfc stores it under the same requested key and CFRelease 0x4dd04 removes
one returned reference. Null skips insertion. There is no array-count rewrite,
descriptor cloning or feature-mask update in this insertion helper.

The same response construction calls AirPlayGetFeatures at 0x4ad44 and inserts
a number under `features` object 0x103ac8 at 0x4aea8. This is a separate path
in the same request; no count-dependent mask change was found.

Serialization helper @ 0x49950 calls CFBinaryPlistV0Create at 0x4999c then
HTTPMessageSetBodyPtr at 0x499b8 with `application/x-apple-binary-plist`
(literal @ 0x104cc0). Request helper calls serialization at 0x4b1b4 and
releases response dictionary at 0x4b1c4. Full HTTP body-buffer transfer/error
semantics were not reconstructed and are not part of an adapter declaration.

## Ownership, caching and scope

The response dictionary's retaining callbacks own the array after successful
insertion; its reference ends with response release after serialization. One
callback-result reference is released immediately after insertion. The extra
retain observed in dio is unresolved; see [callback ABI](info-displays-callback-abi.md).

Fresh dynamic response creation, property enumeration and display-array
construction occur on the traced /info path. No result cache is visible in
these functions. Cached manager metadata, other info callers and concurrent
updates remain UNKNOWN. Named thread identity is not established by this
call chain alone.

PLAUSIBLE: selectively augmenting the returned displays array can preserve all
other /info properties. The stock array/descriptor must be preserved; a valid
second descriptor and fully balanced callback ownership are still required.
This does not establish a safe advertisement implementation or iOS trigger.
