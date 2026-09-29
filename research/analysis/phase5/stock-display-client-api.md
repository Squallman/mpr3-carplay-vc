# Stock display client API examples

## dmdt-ivi

dmdt-ivi is the clearest display-manager client example. It links against:

  libdisplayinit.so
  librudi_runtime.so
  liblayerconfig.so
  librudi_core.so
  libiplutil.so
  libosal.so
  libiplcommon.so

Its strings prove RUDI service-tracker/resolver setup and DisplayablesClient
methods GetDisplayables, GetDisplayable and Screenshot. It exposes a command
to enable a hidden displayable with three parameters:

  DISPLAYABLE DISPLAY LAYER

It also reports displayable ID, display, active state, size and layers. The
binary is a DisplayManager/RUDI client, not proof that arbitrary processes may
write any displayable.

## navStartup display-init sequence

navStartup imports dint_create_displayable, dint_get_surface,
dint_create_context, dint_choose_config, dint_init_display,
dint_make_current, dint_swap_buffers and dint_destroy_displayable.

Recovered sequence at the call site around 0x69e2bc:

  displayableName = navigationObject + 0x38 (indirect string)
  createDisplayable(..., displayableName, ..., &object + 0x290)
  obtain surface through dint_get_surface
  create/select EGL context/config
  make current and swap buffers during rendering
  destroy displayable during teardown at 0x69dd0c

The exact C ABI argument types are not recoverable from stripped code, but
libdisplayinit disassembly proves that the name is resolved with
mcp::CLayerConfig::getDisplayableId(string const&). Unresolved names fail.

## Minimum supported shape

  initialize display/graphics library
  name = existing LayerConfig displayable name
  d = dint_create_displayable(role, name, dimensions, config/context, &out)
  surface = dint_get_surface(d, ...)
  create EGL context/config and make current
  render or copy frames to the surface
  eglSwapBuffers / dint_swap_buffers
  dint_destroy_displayable(d)

This is recovered API shape, not implementation advice. It does not establish
that a sidecar can safely write a production displayable concurrently.

## encoding_poc / encoderal_spec

Both samples use libipte and expose a separate direct encoder path:

  gfx::IpteConnection::create(string)
  getDisplayable(int, shared_ptr<IpteDisplayable>)
  inspect width/height/stride/size
  create gfx::CEncoder
  feed(IpteDisplayable)

They demonstrate direct IPTE displayable consumption, but not creation of a
new displayable surface. No compositor call is present in their relevant
imports.
