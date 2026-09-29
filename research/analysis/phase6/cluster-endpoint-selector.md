# Active cluster endpoint selector

EMS tables expose endpoint families:

```text
method 6: CLUSTER_MOST=0, HUD_MOST=1, CLUSTER_ETH=2, HUD_ETH=3
method 9: MOST_CLUSTER=0, ETHERNET_CLUSTER=1, MOST_HUD=2,
          ETHERNET_HUD=3, MOST_CLUSTER_MINOR=4, ETHERNET_CLUSTER_MINOR=5
```

P3695 displaymanager data defines corresponding virtual displays and coding /
terminal variant tables. `getDisplays` selects display variants from coding
fields, but the extracted artifacts do not contain the runtime values needed to
select the active encoder endpoint.

Result: **UNKNOWN**. The endpoint cannot be called MOST merely because MOST is
configured. The deciding runtime values are vehicle coding/terminal state and
the service branch selecting the EMS DisplayID family.
