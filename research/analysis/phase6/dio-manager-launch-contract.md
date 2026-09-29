# dio_manager launch contract

`smartphone_integrator` contains a configured child entry:

```json
"carplay": {
  "exec": "dio_manager",
  "path": "/opt/smartphone_integration/bin",
  "useEnforcer": true,
  "params": "",
  "envs": [
    "LD_LIBRARY_PATH=/opt/smartphone_integration/lib:/opt/iap2/lib:/opt/cinemo/lib:/opt/opus/lib:/gpl/lib",
    "IPL_CONFIG_DIR_DIO_MANAGER=/opt/smartphone_integration/etc",
    "IPL_CONFIG_DIR_EXTDEVCONFIG=/opt/iap2/etc",
    "LIBIMG_CFGFILE=/etc/config/img.conf"
  ]
}
```

The launcher uses `posix_spawn` (imports/strings at `0x11f80` and process
handler diagnostics) and starts the child through `/usr/bin/enforcer` when
`useEnforcer` is true. `dio_manager` is configured by
`/etc/enforcer/dio_manager.json` to run as UID/GID 1122 with AppArmor profile
`/etc/enforcer/apparmor/carplay`; capabilities, namespaces, and seccomp are
otherwise restricted/disabled as shown in that JSON.

**PROVEN:** the environment is explicitly constructed from the `envs` list;
the stock CarPlay list does not contain LD_PRELOAD. The mirrorlink entry does
contain an explicit `LD_PRELOAD`, proving the launcher can pass one in a child
environment. **UNKNOWN:** whether an altered carplay environment would be
accepted by the production configuration path and whether Enforcer/AppArmor
permits the chosen library path.

The preload path cannot be called “available under stock policy” without the
missing AppArmor profile. The strongest safe classification is **UNKNOWN / not
proven**.
