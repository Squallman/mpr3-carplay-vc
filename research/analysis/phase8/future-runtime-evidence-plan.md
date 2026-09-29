# Future runtime evidence plan — document only

This plan exists because client validation/trigger questions reached
STATIC_LIMIT_REACHED. It contains no scripts, target commands, deployment,
configuration edits or authorization to run a bench or vehicle experiment.
A later phase requires separate authorization and a concrete observation
method. Prefer existing passive/read-only diagnostics or captures.

| Observation | Question answered | Minimum data | Passive/read-only preference | Why static P3695 cannot answer |
|---|---|---|---|---|
| Stock identification exchange | What parent24 bytes actually reach the client, and are they accepted? | Complete IdentificationInformation0x1d01 with nested lengths/IDs, acceptance/rejection and timing; client/version | Passive capture if available | Static serialization does not show actual runtime variant, delivery or client reaction |
| Stock /info response | What live displays/features does the client see? | Full decoded dictionary plus original bytes; geometry, UUID, primary input, feature mask, maxFPS and timing | Passive capture/read-only log | DSI fields and runtime property values are not present as live constants |
| Stock SETUP exchange | What screen requests and response fields occur without intervention? | Full request/response dictionaries and exact types/streamConnectionIDs; timestamps linked to /info | Passive capture | No observed phone choice is in a head-unit parser |
| Active geometry/configuration snapshot | Which DSI window/video/physical/input fields are active? | ServiceConfiguration values, manager frame/window values, variant and session phase | Existing read-only state/diagnostic source if available | Only source offsets/copy rules are known statically |
| Displays reference lifetime | Does the active binding have the inferred extra reference? | Actual bound helper identity and create/retain/set/release/destruction sequence over repeated /info | Existing refcount/diagnostic observation if available | Static accounting cannot prove runtime binding or observed leak |
| Controlled comparison, only if separately approved | Is a known supported capability change related to /info and type111? | Same client/version; paired identification, /info and SETUP records; one documented changed variable | Observe an existing supported configuration choice where possible | Necessity/sufficiency requires contrasting client decisions; no change is proposed here |
| Authorized known-valid two-display reference, if one becomes available | What schema/UUID/role does this client accept? | Advertisement and actual type111 exchange from a documented compatible reference, with provenance | Capture/reference data only | Helper emission lacks a client validator or known-valid P3695 second descriptor |

First collect a stock baseline and prove correlation by transaction/session
timing. An accepted identification message does not establish child17 meaning;
two dictionary entries do not establish type111. An observed111 exchange should
be tied to a complete preceding advertisement and identification record before
any causal claim. A one-variable comparison can discriminate hypotheses only
when that variable's supported meaning is already known.

Active display endpoint/occupancy and output restoration may be observed later
for separate output-adapter work, but they are not minimum evidence for closing
the phone's input trigger. This plan does not add a child17 experiment, invent
a UUID/descriptor, or request vehicle connectivity. If passive observations
cannot establish sufficiency, the remaining question stays UNKNOWN until a
separately justified client analysis/experiment is authorized.
