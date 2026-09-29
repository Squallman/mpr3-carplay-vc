# Current architecture

```text
                         iPhone
                            |
                            v
                 AirPlay SETUP / negotiation
                            |
                   runtime interception
                    /                  \
             type 110                 type 111
                |                         |
          stock CarPlay          secondary owner
                                      |
                              ScreenStream / decoder
                                      |
                           configured IPTE displayable
                                      |
                         setActiveDisplayable(displayID, X)
                                      |
                         stock videoencoderservice
                                      |
                                  MOST / ETH
                                      |
                              Virtual Cockpit
```

**Proven:** type 110 is the stock screen; type 111 is rejected by stock SETUP;
screen contexts/ScreenStream factories are instance-oriented; displayable
creation resolves configured names; the VideoEncoding RPC exists; the encoder
directly resolves and feeds an IPTE displayable; the SETUP symbol has a PLT /
JUMP_SLOT seam.

**Implementation proposal:** filter type 111 before the stock handler, retain it
for an independent owner, render into a configured displayable, and select that
displayable through stock VideoEncoding.

**Unknown:** the iOS advertisement gate, second descriptor, policy/loader
authorization, safe displayable, active endpoint, and real target lifecycle.

Secondary failure MUST NOT break stock stream 110. The prototype enforces this
fail-open rule.
