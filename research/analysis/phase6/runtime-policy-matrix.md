# Runtime policy matrix

The extracted tree contains the Enforcer JSON files but not the referenced
AppArmor profile contents. No policy conclusion is inferred from filenames.

| Operation | dio_manager hook | standalone sidecar | Evidence | Status |
|---|---|---|---|---|
| map/execute shared object | child has explicit env construction; profile missing | own profile/startup not found | dio_manager.json, smartphone_integrator.json | UNKNOWN |
| access COMM/EMS VideoEncoding | dio_manager already uses stock COMM/EMS libraries | vesdt proves a standalone client shape | vesdt, EMS tables | STRONG EVIDENCE for API; policy UNKNOWN |
| access IPTE/display libraries | stock nav/sample binaries do so | no sidecar policy | libdisplayinit/libipte dependencies | UNKNOWN |
| read LayerConfig/config | stock paths read configured names | path/profile permissions absent | config and binaries | UNKNOWN |
| arbitrary executable path | Enforcer/AppArmor profile unavailable | no launch definition | missing `/etc/enforcer/apparmor/*` | UNKNOWN |
| UID/GID | 1122/1122 | no proposed identity | dio_manager.json | PROVEN for dio_manager only |

No evidence proves a policy block, but no evidence proves authorization either.
