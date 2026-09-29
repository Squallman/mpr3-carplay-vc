# AirPlay server delegate map

`dio::CAirPlayServer::registerDelegates` at `dio_manager:0x1fd340` copies
exactly `0xa0` bytes from its `AirPlayReceiverServerDelegate` argument and then
calls `AirPlayReceiverServerSetDelegate`. The libairplay setter at `0x4b6b0`
copies those bytes into the server object at `server+0x18`.

The delegate is assembled by `CAirPlayHostThread::registerAirPlayServerDelegates`
at `0x1f6250` and registered at `0x1f6400`.

| Delegate fact | Result |
|---|---|
| copied size | 0xa0 bytes, PROVEN |
| destination in libairplay server | private +0x18, PROVEN |
| registration API | AirPlayReceiverServerSetDelegate, PROVEN |
| display callback member | `CDIOManager::infoRequestDisplays(int&)`, STRONG EVIDENCE |
| exact byte slot | UNKNOWN from stripped consumer-side mapping |

The dynamic-info response iterates callback/property entries and inserts the
returned CF object into the response dictionary. The exact byte slot cannot be
asserted without a complete field-to-callback map.
