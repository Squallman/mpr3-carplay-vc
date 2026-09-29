# Displays callback ABI and ownership

## Two different ABIs

STRONG EVIDENCE: the C-shaped delegate slot is the generic property callback
at delegate+0x18, **not** a direct CDIOManager method. Its observed registers:

```text
x0 = AirPlay server
x1 = CFLString property key
x2 = qualifier pointer (unused by this producer stub)
x3 = optional int32 status output
x4 = manager context
x0 return = CF object pointer or null
```

Producer symbol s_serverCopyPropertyCallback @ dio 0x1df750 agrees with
consumer AirPlayReceiverServerPlatformCopyProperty @ airplay 0x481c0,
indirect call 0x48234. Stub initializes local error to zero, passes context
as `this` into infoResponse @ 0x1df610, copies error to x3 when supplied,
and returns object. Null manager context produces -6740. Consumer on error
releases a nonnull result at 0x482b0 before returning null.

The bound method symbol is `CDIOManager::infoRequestDisplays(int&)` @
0x1db6e0. Observed method ABI:

```text
x0 = CDIOManager this
x1 = nonnull pointer to 32-bit int error
x0 return = CFL array pointer or null
```

The const/source return typedef cannot be recovered from symbol mangling;
array semantics follow constructor and consumer type. It is not a recovered
Apple CFArrayRef header declaration.

## Dispatch and lifetime

registerInfoRequestCallbacks @ 0x1dfe90 binds literal `displays` @ 0x32e1c8
to method 0x1db6e0. infoResponse converts key to std::string; the manager map
at manager+0x8f0 invokes the corresponding std::function at 0x1df49c.
infoRequestDisplays calls getDisplays @ 0x1db510, then if incoming *error is
zero, CFRetain at 0x1db720, returning the same array.

getDisplays calls createDisplaysDictionary at 0x1db608. That helper creates
a fresh owned array. **STRONG EVIDENCE of an extra reference**: no balancing
release of that initial array reference is visible between creation and the
method's retain. getDisplays uses a local error initialized -6714; the
incoming caller error reference is not updated in the inspected body. Do
not simplify this to a proven exactly-+1 result or copy its apparent leak.

The /info consumer inserts the returned object at 0x4dcfc, retaining it through
the dictionary callback, then releases one callback-result reference at
0x4dd04. On null result it omits the property. Allocation failure can return
null without propagated method error in the observed path. New callback
augmentation must resolve the reference imbalance and all failure conventions.

STRONG EVIDENCE: invocation is synchronous per property enumeration in the
observed /info path, with fresh array construction. No cache was found on
that path. Named execution thread, locking/serialization, other callers,
and races with service configuration changes remain UNKNOWN. No runtime
thread-safety claim is made. See [response path](airplay-info-response-path.md).
