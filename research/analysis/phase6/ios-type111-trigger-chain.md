# Causal chain for iOS type 111

| Arrow | Classification | Evidence |
|---|---|---|
| iAP2 identification -> CarPlay capability state | STRONG EVIDENCE | historical MHI3 gate; P3695 standard transport component |
| iAP2 child 17 -> P3695 capability state | UNKNOWN | no P3695 child-17 case proven |
| capability state -> AirPlay `/info` displays/features | UNKNOWN | stock callback and feature mask proven; causality not |
| `/info` displays/features -> iOS proposes type 111 | HISTORICAL ONLY | MHI3/MHI2Q comparative work |
| P3695 SETUP -> type 111 | DISPROVEN for stock | dispatcher accepts 100..110 only |

The strongest defensible claim is that iOS must be made to believe a secondary
screen is available through one or more capability layers. The exact required
combination is UNKNOWN; historical parameter 17 is not proof of P3695 behavior.
