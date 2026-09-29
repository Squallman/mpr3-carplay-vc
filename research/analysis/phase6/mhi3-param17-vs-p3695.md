# Historical MHI3 parameter 17 versus P3695

Historical commit `e9c2b9a6c231b192420be21fdea78a8565068c47`,
`docs/altscreen_gate_analysis.md`, documents MHI3 top-level parameter 21
(`WirelessCarplayTransportComponent`) with subparameters 0 (2-byte ID), 1
(32-byte UUID), 17 (zero-byte void marker), 18 (void), and 20 (void). The
documented wire header for void ID 17 is a four-byte header followed by
big-endian ID `0x0011`. The same historical implementation also emitted ID 17
under parameter 20.

P3695 scans found no named ThemeAssets class/string or proven child-17 emit
site. P3695 does contain generic iAP2 identification serialization, but no
proof that its type tables accept arbitrary nested TLVs.

Classification: **UNKNOWN between GENERICALLY EXPRESSIBLE and NEW SERIALIZER
REQUIRED**. Native support is not proven; the historical gate remains an
input-side blocker.
