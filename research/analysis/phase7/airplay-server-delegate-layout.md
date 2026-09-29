# AirPlayReceiverServerDelegate layout

## Construction and copy

PROVEN: CAirPlayHostThread::registerAirPlayServerDelegates @ dio_manager
0x1f6250 constructs 0xa0 bytes at stack x29+0x38. Initial pairs are zeroed,
named callbacks loaded via GOT relocations, optional callbacks selected by
configuration, context populated, then CAirPlayServer::registerDelegates @
0x1fd340 copies the record. AirPlayReceiverServerSetDelegate @ libairplay
0x4b6b0 copies 0xa0 bytes to server+0x18.

Offsets below are **delegate-relative**, not server-relative. The latter are
offset+0x18. Producer names come from symbols plus resolved GOT entries; role
and ABI are STRONG EVIDENCE unless the consumer is explicitly UNKNOWN.
`ctx` is the CDIOManager pointer supplied by the producer; `p` is a pointer.
Return types not established by register use remain UNKNOWN.

| Offset | Producer callback / value | Consumer path | Approx ABI | Semantic role | Confidence |
|---|---|---|---|---|---|
| 0x00 | Manager context; final store 0x1f6388 | server+0x18, property/control/session/audio callback context | p | Callback context | PROVEN |
| 0x08 | Zero | Complete invocation path UNKNOWN | UNKNOWN | Unused in this producer; no role assigned | UNKNOWN |
| 0x10 | Zero | ServerControl reads server+0x28 at 0x4b91c, calls 0x4b930 if nonnull | server, command, qualifier, parameters, responseOut, ctx | Generic control | STRONG EVIDENCE |
| 0x18 | s_serverCopyPropertyCallback @ 0x1df750; store 0x1f62d8 | PlatformCopyProperty server+0x30 at 0x48218 → call 0x48234 | x0 server, x1 CFString key, x2 qualifier, x3 int32 error-out, x4 ctx → x0 object | Generic property copy; includes displays | STRONG EVIDENCE |
| 0x20 | Zero | PlatformSetProperty reads server+0x38 at 0x48300, tail call 0x4830c | server, key, qualifier, value, ctx → w0 status | Generic set-property; disabled in this producer | STRONG EVIDENCE |
| 0x28 | s_sessionCreatedCallback @ 0x1e2710; store 0x1f62e4 | SessionCreate server+0x40 at 0x52ee4 → 0x52ef8 | server, session, ctx | Session created | STRONG EVIDENCE |
| 0x30 | s_sessionSetupFailedCallback @ 0x1e48c0; store 0x1f62e4 | Setup server+0x48 at 0x59200 → 0x59210 | server, int32 status, ctx | Setup failure | STRONG EVIDENCE |
| 0x38 | s_sessionScreenCreatedCallback @ 0x1dd5b0; store 0x1f62e8 | SessionCreate server+0x50 → call 0x52e18 | x0 pointer from session+0x1b08 | Screen context created | STRONG EVIDENCE |
| 0x40 | s_audioTypeCallback @ 0x1e21d0; store 0x1f62e8 | server+0x58 passed to AudioStreamSetAudioTypeCallback at 0x475e4 | int32, uint32, ctx | Audio type callback | STRONG EVIDENCE |
| 0x48 | s_waitAudioAvailableAckEvent @ 0x1dbc00; store 0x1f62cc | server+0x60 → AudioStreamSetAudioAvailabilityACKCallback 0x47618 | int32, ctx | Audio availability acknowledgment | STRONG EVIDENCE |
| 0x50 | s_initAudioDumpingCallback @ 0x1e5e00; store 0x1f6320 | server+0x68 → AudioStreamSetDumpingInitAudioCallback 0x4762c | enum32, int32, uint32, uint32, uint32, ctx | Audio dump initialization | STRONG EVIDENCE |
| 0x58 | s_txAudioDumpingCallback @ 0x1e6060; store 0x1f633c | server+0x70 → AudioStreamSetDataDumpCallback 0x47640 | bytes*, uint64 size, enum32, ctx | Transmit audio dump | STRONG EVIDENCE |
| 0x60 | s_rxAudioDumpingCallback @ 0x1e6110; store 0x1f6358 | server+0x78 → same setter | bytes*, uint64 size, enum32, ctx | Receive audio dump | STRONG EVIDENCE |
| 0x68 | s_txAudioFlushCallback @ 0x1e61b0; store 0x1f6374 | server+0x80 → AudioStreamSetDataFlushCallback 0x47654 | enum32, ctx | Transmit flush | STRONG EVIDENCE |
| 0x70 | s_rxAudioFlushCallback @ 0x1e6240; store 0x1f63a0 | server+0x88 → same setter | enum32, ctx | Receive flush | STRONG EVIDENCE |
| 0x78 | Manager context; store 0x1f63f0 | server+0x90 → Screen_StartSession x7 at 0x4efc4 → ScreenStreamCopyDelegates 0x5e180 | p | Screen callback context | PROVEN |
| 0x80 | s_screenBadFramesCallback @ 0x1dd540; store 0x1f63f0 | Copied to ScreenStream+0x50; ReportBadFrames invokes 0xf7e88 with context +0x48 | uint32 count, ctx | Bad frames | STRONG EVIDENCE |
| 0x88 | s_captureFramesCallback @ 0x1e5cb0; store 0x1f63d4 | Copied to ScreenStream+0x58; indirect call 0xf713c | const bytes*, uint64 length, ctx | Capture frame bytes | STRONG EVIDENCE |
| 0x90 | s_finalizeFramesCapturingCallback @ 0x1e5d50; store 0x1f63d4 | Copied to ScreenStream+0x60; ScreenStreamStop invokes 0xf78a0 | ctx | Finalize capture | STRONG EVIDENCE |
| 0x98 | s_videoDecoderErrorCallback @ 0x1e2ea0; store 0x1f63f4 | Copied to ScreenStream+0x68, passed by reference to video implementation at 0xf73c8; CESOGfxVideo stores reference +0x30; decoder error job dereferences and calls 0xfa0b8 | No arguments; job ignores result | Decoder error callback | STRONG EVIDENCE |

Offsets 0x50–0x70 are selected by EKey 122; 0x88/0x90 by EKey 156. Initial
zeros apply when disabled. The trace/logging guard around the latter has a
later branch; this table describes observed stores, not runtime option state.
The final record tail is initialized at 0x1f63a4–0x1f63ac before final stores.
Audio callbacks are handed to AudioStream, rather than directly invoked from
the server record. Their downstream invocation timing and complete returns
remain UNKNOWN.

## Displays offset result

**STRONG EVIDENCE: delegate+0x18 (server+0x30) is the relevant generic property
callback. There is no direct infoRequestDisplays member in this construction.**
The manager registers a std::function under exact key `displays` in
registerInfoRequestCallbacks @ 0x1dfe90. Construction at 0x1dff98 binds the
key; 0x1dffac–0x1dffb0 establishes method 0x1db6e0. Generic property callback
→ infoResponse → manager property map → bound infoRequestDisplays.

DISPROVEN for this producer: looking for a direct 0x1db6e0 pointer among the
0xa0 delegate bytes. Patching generic offset 0x18 would affect all properties
unless it dispatches and preserves them.

## Completeness limit

Every producer qword is accounted for. This is not a fully closed consumer
map: slot 0x08 and downstream AudioStream invocation timing/returns still
need work. Server runtime finalizer 0x495f0 and PlatformFinalize 0x48140
were checked; neither supplied an invocation for the unused slot 0x08.
Searches included direct/paired loads across
server creation/control/session paths, callback setter calls, and the five
qword ScreenStream copy @ 0xf8050. Numeric offset matches on session, decoder,
RTSP connection and stack objects were rejected without server provenance.
The exact displays path is closed independently of these remaining slots.
