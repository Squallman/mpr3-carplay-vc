# SETUP request escape analysis

## Result and scope

**STRONG EVIDENCE: YES**, a temporary retaining shallow-copy request may be
released immediately after an ordinary return from the recovered P3695 stock
SETUP path. Neither the temporary dictionary nor its replacement streams array
needs to survive that call. This is a contract for the inspected stock binary,
not an assertion about arbitrary replacement delegates or concurrent mutation.
See [request origin](setup-request-origin.md) and [cleanup](setup-cleanup-matrix.md).

Image-relative addresses below identify evidence in `libairplay.so`; they are
not runtime function addresses. The source fingerprint is in the
[main report](phase8-ownership-secondary-display.md#evidence-scope).

| Object/value | Consumer | Escape? | Retained/copied? | Lifetime | Confidence |
|---|---|---|---|---|---|
| Request dictionary | SETUP 0x58dc0, saved at stack+0xc8, 0x58df0 | No persistent request store found | READS_ONLY_SYNCHRONOUSLY | Through original call | STRONG EVIDENCE |
| Streams array | Typed lookup 0x58f94, stack+0xe0; enumeration 0x59018 | No array store in session/screen/worker found | READS_ONLY_SYNCHRONOUSLY | Borrowed while request remains alive | STRONG EVIDENCE |
| Stream dictionaries | Typed array lookup, descriptor in x19 | No whole descriptor escape found | READS_ONLY_SYNCHRONOUSLY | Enumeration and type handler | STRONG EVIDENCE |
| Type110 descriptor | Screen_Setup 0x5cf10, call 0x590a8 | No whole descriptor store | READS_ONLY_SYNCHRONOUSLY | Synchronous helper | STRONG EVIDENCE |
| `latencyMs` | Screen_Setup extraction; screen+0 store 0x5cf58 | Scalar persists | COPIES_SCALARS | Screen lifetime; absent/zero extraction uses configured fallback | PROVEN (store) |
| `streamConnectionID` | SETUP extraction 0x590c8, crypto derivation 0x51b90 / 0x59c28 | Scalar and derived bytes persist | COPIES_SCALARS | Screen/session state | STRONG EVIDENCE |
| Derived key material | ChaCha setter 0x5cdf0; AES setter 0x5ce40 | Owned bytes, not stack pointer | COPIES_SCALARS | Screen crypto context | STRONG EVIDENCE |
| Socket/port state | SETUP socket path around 0x59138 | File descriptor/scalars persist | COPIES_SCALARS | Session/screen teardown | STRONG EVIDENCE |
| Platform `setUpStreams` params | PlatformControl 0x48410; match 0x48600, parse 0x4863c onward | No generic asynchronous request dispatch on this command | READS_ONLY_SYNCHRONOUSLY | Local platform handling | STRONG EVIDENCE |
| Audio descriptor `audioType` | PlatformControl 0x487c0–0x487e8 | Yes, audio state+0x18 | STORES_RETAINED_POINTER; retain 0x487d4 before store 0x487dc; old value released | Independent of request/container | PROVEN (retain/store) |
| Audio `vocoderInfo` dictionary | Lookup 0x48800, numeric conversion 0x48814 | Dictionary does not escape on inspected path | COPIES_SCALARS | Audio state receives scalar | STRONG EVIDENCE |
| Request on failure teardown | SETUP 0x5a060 → TearDown 0x58930 → local platform 0x47f20 | No request store found | READS_ONLY_SYNCHRONOUSLY | Cleanup before return | STRONG EVIDENCE |
| Screen worker arguments | Worker construction 0x57a70, binding 0x57abc, start 0x57b9c | Session persists, not request | No request/descriptor-derived CF pointer in work item | Session worker lifetime | STRONG EVIDENCE |

## Downstream screen state

Screen_Create 0x5c950 allocates the independent 0x340-byte context without a
request argument. Screen_Setup reads the descriptor's integer and stores the
converted value. Screen_StartSession 0x5e0b0, called at 0x4efd8, consumes
already copied session/server fields and screen delegate data. It does not
receive a request or stream dictionary.

ChaCha key storage copies 32 bytes at 0x5ce1c–0x5ce28. AES_CTR_Init 0x6f260
expands the key through `aes_encrypt_key128` and copies the IV at
0x6f294–0x6f298. SETUP wipes its temporary key buffers at 0x591b0 and
0x59c50/0x59c68. Those buffers are not borrowed worker inputs. Screen setup
failure cleanup at 0x59c80 onward releases the response descriptor, invokes
session screen cleanup and wipes temporary key bytes.

The failure callback at 0x59210 receives server/status/context, not the request.
The os-build notification uses a newly constructed CFString from copied text
(0x58f40–0x58f58), rather than a borrowed parsed CFString.

## Important negative control: other commands do queue borrowed params

PlatformControl's generic fallback at 0x48b18 can pass params to the session
delegate. The DIO callback 0x1e26e0 reaches asyncSessionControl 0x1e25e0:
proxy 0x1651f0 stores the borrowed CF params at event+0x40 (0x165230), queues
the event, then waits at 0x1e2658. Event processing 0x162940 and manager
onEvent 0x1dfa60 dispatch and signal completion at 0x1dfad8. The event
destructor 0x163220 destroys the command string, not that CF pointer.

This prevents generalizing the result to every AirPlay control operation.
The exact `setUpStreams` and `tearDownStreams` comparisons select local
handlers before that fallback. Audio objects that persist are explicitly
retained; type110 is skipped by the audio platform loop. No claim of general
callback thread safety or safety after installing another delegate follows.

## Exceptions and boundary of the conclusion

Cold SETUP landing pads, including 0x5b198–0x5b470, clean up C++ worker and
function objects and resume unwinding. They do not establish universally
balanced stock response cleanup. A future wrapper must scope its own CF
references so they are released on unwinding and must preserve the exception.
It must not catch an unknown exception and invent an AirPlay status. Stock
response leaks on an exceptional path, if present, are independent of temporary
request lifetime. Runtime rebinding and service concurrency remain UNKNOWN.
