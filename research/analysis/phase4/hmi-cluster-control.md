# HMI cluster control investigation

The extracted Audi HMI artifacts contain the displayable names and display
manager lookup calls, but no recovered literal setActiveDisplayable or
setDisplayable invocation.

main.17a1455a44197c5710246855...js contains enum-like values:
  Displayable_Cluster_Map = 40
  Displayable_Cluster_Map_Route_Guidance = 42
  Displayable_Hud_Map = 60
  Displayable_External_Smarthphone = 93

It also contains display names including Cluster_Display (1),
Cluster_Display_Subframe (80), Virtual_Display_Cluster_MOST (90),
Virtual_Display_HUD_MOST (91), Virtual_Display_Cluster_ETH (92), and
Virtual_Display_HUD_ETH (93). These are lookup/configuration values, not proof
of an active endpoint.

The recovered getDisplayable occurrences are display-manager service/debug
lookups such as displayManager.getDisplayables, displayables.getDisplayables,
and getDisplayableId. They do not establish an active-display switch.

navi-fpk configures MapDisplayService and MapGuidanceDisplayService with a
display name, displayable name, layer name, and source/target rectangles. This
is evidence that the HMI navigation control plane chooses presentation targets,
but the final call into videoencoderservice is not present in the inspected JS.

navStartup has no recovered videoencoderservice/IVideoEncoding import and no
setActiveDisplayable literal. Consequently the stock native-navigation caller
of setActiveDisplayable is UNKNOWN. It may be a display manager, generated RPC
client, or an unextracted service path; no claim is made that HMI itself calls
the encoder.

Result: HMI controls view/displayable configuration, but a direct proof of the
sentence “show native map in Virtual Cockpit” reaching setActiveDisplayable is
not yet available.
