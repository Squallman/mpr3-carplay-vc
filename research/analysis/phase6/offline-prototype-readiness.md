# Offline prototype readiness

## Decision

**READY FOR OFFLINE PROTOTYPE**, with vehicle I/O mocked and no deployment
claim.

Enough is proven to encode host-side mocks for the three-argument
`AirPlayReceiverSessionSetup` wrapper, type-110-preserving stream filtering,
independent secondary state, the VideoEncoding proxy integer contract, direct
displayable-to-encoder semantics, and configured displayable-name creation.

Actual AArch64 calls, CF runtime, AppArmor, service discovery, decoder/rendering
I/O, exact second descriptor construction, safe displayable occupancy, and the
active endpoint must remain mocked or unresolved.
