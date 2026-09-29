# vesdt set_displayable call chain

Artifact: phase3/extracted/usr/bin/vesdt, AArch64 ELF, not executed.

## Proven parser path

The main command path begins at the stripped entry region around 0x4c20.
It starts the COMM agent, constructs the generated VideoEncoding proxy state,
then compares argv[1] with the set_displayable command string. In the
set_displayable branch:

  argv[2] -> strtol(base 10) at 0x4de4-0x4df0
  argv[3] -> strtol(base 10) at 0x4df4-0x4e04
  w1 = first parsed value
  w2 = second parsed value
  x0 = proxy object + 0x10
  call wrapper at 0x6970

The wrapper at 0x6970 calls comm::Proxy::getImpl() and dispatches through the
proxy implementation vtable slot at offset +0x8:

  0x6970: x0 += 0x10
          save w1/w2
          comm::Proxy::getImpl()
          x3 = [impl]
          x3 = [x3 + 0x8]
          branch x3 with w1,w2

The sibling wrapper at 0x6930 dispatches through vtable offset 0 and is used by
the set_rate branch. Thus set_displayable is a generated proxy call, not a
direct socket write in vesdt. Exact C++ method spelling for the vtable slot is
not present in the stripped client, but the EMS table identifies the relevant
setActiveDisplayable methods and the command is named set_displayable.

## EMS method identities

esofw/share/ems_tables.zip contains asi.VideoEncoding/IVideoEncoding:

  method 0: setActiveDisplayable, DisplayID enum CLUSTER=4, HUD=5, displayable=i
  method 3: setActiveDisplayable, PRIMARY_DEFAULT=0, CLUSTER=4, HUD=5,
            PARKPILO=-16, displayable=i
  method 6: setActiveDisplayable, CLUSTER_MOST=0, HUD_MOST=1,
            CLUSTER_ETH=2, HUD_ETH=3, displayable=i
  method 9: setActiveDisplayable, MOST_CLUSTER=0, ETHERNET_CLUSTER=1,
            MOST_HUD=2, ETHERNET_HUD=3, MOST_CLUSTER_MINOR=4,
            ETHERNET_CLUSTER_MINOR=5, displayable=i

Methods 0, 3 and 6 are marked true in the table; method 9 is present but marked
false. The table is stronger evidence than names in the stripped binary for
method numbering, but it does not prove which variant the Audi runtime selects.

## Proxy construction and discovery

The constructor path around 0x4d14-0x4d5c constructs two IdentityArgs strings,
then calls comm::AgentStarter::AgentStarter and AgentStarter::start. The binary
contains the service identity string:

  asi.VideoEncoding.IVideoEncoding

It also contains generated proxy/stub type names and
comm_idl_calls::call_struct_DisplayID_I32IVideoEncodingRequest. The connection
is therefore COMM/ASI service discovery, not a raw Unix socket opened by
vesdt. The exact broker socket is hidden inside libcomm and was not recovered
from this client alone.

## Result handling and errors

The command sleeps for 500 ms after issuing the request and exits. The generated
call returns through the proxy ABI; no user-visible reply parsing is performed
in the set_displayable command path. Service startup failure prints “Could not
establish comm connection”. The AgentStarter/proxy path contains alive/dead
wait and exception/error handling. Invalid numeric strings are handed to
strtol; no explicit range validation is visible before the RPC call.

Displayable existence and dimensions are validated service-side, not by vesdt.
videoencoderservice later calls IpTeConnection::getDisplayable(int), checks
width/height, and only then creates/feeds the encoder. Thus nonexistent or
zero-dimension displayables fail after the RPC reaches the service.

## Approximate pseudocode

  int command_set_displayable(argv) {
      int displayId = (int)strtol(argv[2], 0, 10);
      int displayable = (int)strtol(argv[3], 0, 10);
      // generated proxy implementation, vtable slot +8
      return videoEncodingProxy.set_displayable(displayId, displayable);
  }

  proxy startup() {
      osal_init();
      util_init();
      AgentStarter agent(identity_args, ...);
      agent.start(...);
      ServiceRegistration registration(...);
      comm::Proxy proxy(identity=asi.VideoEncoding.IVideoEncoding,
                        proxy_stub_factory, registration, ...);
      proxy.connect()/waitUntilAlive();
      return proxy;
  }

The pseudocode deliberately leaves generated factory/identity fields abstract;
their ABI is present but not semantically named in the stripped artifact.

## Libraries

vesdt NEEDED libraries:
  libcomm.so
  libiplutil.so
  libosal.so
  libiplcommon.so
  libstdc++.so.6
  libgcc_s.so.1
  libc.so.6
  libdl.so.2
  ld-linux-aarch64.so.1

No libipte or libdisplayinit dependency is required merely to call the encoder
RPC. Those are needed by a sidecar that also creates/writes a displayable.
