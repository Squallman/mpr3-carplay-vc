# WirelessCarPlay parent24 lifecycle and feedback

## Client-facing message

**STRONG EVIDENCE:** the parent24 component is included in control-session
**IdentificationInformation, message ID0x1d01**. libesoiap2 composer 0x43f60
constructs the control message, sets that ID at 0x441f4–0x441f8, serializes
parameters and deploys the result at 0x45b0c. It is not an AirPlay /info response.

| Stage | Evidence | Contract / confidence |
|---|---|---|
| Prepare Wireless fields | dio addIdentificationInformation 0x2631f0, Wireless branch 0x2637a0–0x26386c | Parent24 object fields0/1/2/4; STRONG EVIDENCE |
| Start callback | Wireless::startIdentification 0x2623d0 | Obtains msg handler/deployer at 0x262464/0x262470; STRONG EVIDENCE |
| Compose message | 0x262488 calls identificationInformation with this+0x60 and transaction argument zero | Full prepared identification object, not a standalone child injection; PROVEN (arguments) |
| Serialize parent24 | Selector3 helper, Wireless branch 0x430d8–0x43530 | Nested views copied into parent, then outer parameter list; STRONG EVIDENCE |
| Final handoff | libesoiap2 0x45afc builds outer params, 0x45b0c → 0x40930 | Deployer virtual slot+0x10; STRONG EVIDENCE |
| DIO deploy implementation | 0x258020; getBuffer 0x258078, connection proxy slot+0x18 at 0x258088 | Raw control message buffer plus connection handle +0xdc passed to IPC path; STRONG EVIDENCE |

This establishes the control-session route to the iAP2 connection proxy. Exact
wire scheduling/retransmission by lower transport is not an observation of
when the phone receives/accepts every message. Composition is synchronous;
local parameter backing buffers remain owned until copying and message handoff.

## Feedback exists, but does not expose secondary-screen interpretation

The identification parser dispatches IDs at 0x48abc–0x48adc:

| ID | Consumer | Recovered behavior |
|---|---|---|
| 0x1d00 | 0x48b48–0x48b60, listener slot+0x10 | Start identification, transaction context forwarded |
| 0x1d02 | 0x48b80–0x48b98, listener slot+0x18 | Identification accepted; no accepted-capability payload parsed on this branch |
| 0x1d03 | 0x48b68 → rejection parser 0x484c0 | Rejection parameter data parsed, then listener notification |

DIO identificationAccepted 0x2644e0 iterates module listeners, invoking
slot+0x18. identificationRejected 0x264c50 examines rejected fields, logs them,
and can clear retry state at this+0x410 (0x264f50), delay via nanosleep and
invoke startIdentification again at 0x264ff0; terminal rejection notifies module
slot+0x20 at 0x26507c. Thus “no client feedback at all” is **DISPROVEN**.
The accepted/rejected feedback is not a reported second-screen decision.

The inspected Wireless start callback ignores its incoming transaction argument
and passes zero to the composer. The rejection retry also passes zero. An
acceptance callback alone neither identifies child17 support nor proves the
phone interpreted parent24 as an alternate-screen component. No identification
response branch recovering such a state was found. How iOS interprets the
component and combines it with AirPlay remains UNKNOWN and external to the
recovered feedback payload.
