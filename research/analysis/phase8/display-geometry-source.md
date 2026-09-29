# Active display geometry source contract

Normal advertised geometry: **RUNTIME_STATE**. Including the disabled test-HMI
configuration branch, overall source classification is **MIXED**. Actual live
values remain UNKNOWN; 1540x720 and 235x110 are not promoted to active geometry.

startService 0x2170c0 receives a ServiceConfiguration smart-pointer reference,
loads its object at reference+8 and copies fields into CDIOManager loaded from
implementation+0x208. The recovered source layout is:

| Source ServiceConfiguration | Manager destination | Meaning from metadata and use | Confidence |
|---|---|---|---|
| +0x30 / +0x34, uint32 | +0x454 / +0x458 | videoResolutionX / videoResolutionY | STRONG EVIDENCE |
| +0x38 / +0x3c, uint32 | +0x45c / +0x460 | windowResolutionX / windowResolutionY | STRONG EVIDENCE |
| +0x40 / +0x44, uint32 | +0x474 / +0x478 | windowOffsetX / windowOffsetY | STRONG EVIDENCE |
| +0x58 / +0x5c, uint32 | +0x470 / +0x46c | physicalDisplayHeight / physicalDisplayWidth | STRONG EVIDENCE |
| +0x60, uint32 on DSI wire | low byte → +0x158 | inputFeatures | STRONG EVIDENCE |
| +0x6c, uint32 | +0x484 | primaryInputFeature | STRONG EVIDENCE |

Field names are corroborated by the actual `/esofw/share/ems_tables.zip`
`dsi.carplay.ems_table.json` startService variants, notably variants with the
six video/window integer fields, inputFeatures and primaryInputFeature.
Earlier variant signatures coexist and must not be assumed to have the same
layout. The generated current deserializer is 0x2e5500; see
[DSI provenance](dsi-display-state-provenance.md).

## Derived advertised resolution

startService validates window dimensions as nonzero and not exceeding video
dimensions at 0x217310–0x21733c. It calls setFrameResolution at 0x217344.
setFrameResolution 0x215930 copies **window** dimensions:
manager+0x45c → +0x464 at 0x215950 and +0x460 → +0x468 at 0x21595c.
It also invokes the service operation with that frame size. getDisplays passes
manager+0x464/+0x468 as widthPixels/heightPixels. Thus advertised pixel geometry
is derived from configured window resolution, not necessarily full video/screen
resolution. No scaling calculation is present in this setter.

Physical dimensions are copied from +0x5c/+0x58 and validated nonzero during
startService (0x217390–0x21739c). That is a stock service policy, not proof that
iOS requires physical dimensions in every second descriptor.

Service startup supplies these fields before the plugin startup path around
0x2174b4. It makes the source available before the ordinary /info callback.
The setter and startup path support replacement on a later startService; an
in-session hot geometry update/concurrency guarantee was not recovered.

EKey182 selects the test-HMI branch. E183/184 supply video dimensions,
E187/188 window dimensions, E189/190 physical dimensions and E199 primary input.
This variant is configuration-derived. maxFPS comes separately from E152
(default60), not a recovered DSI frame-rate field. No active service values,
vehicle coding choice or safe cluster geometry can be read from these static
stores alone.
