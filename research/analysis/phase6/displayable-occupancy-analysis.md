# Displayable occupancy analysis

P3695 defines debug slots 137–145 and generic video slots 88–100. Absence of
production string references is not proof of runtime availability. `dmdt-ivi`
explicitly warns that every configured displayable may already be occupied.

| Candidate | Evidence | Rank |
|---|---|---|
| 137 Debug_1 | configured, no production string reference found, diagnostic naming | BEST TEST CANDIDATE / POSSIBLE |
| 138–145 Debug_2..9 | same weak evidence | POSSIBLE |
| 88–92 Digital video | media ownership unresolved | CONFLICT RISK |
| 94–100 Digital video/StreamOnly | media/diagnostic ownership unresolved | CONFLICT RISK |
| 40/42/60 native cluster/HUD | native navigation ownership | UNSUITABLE |
| 93 external smartphone | stock CarPlay owner | UNSUITABLE |

No candidate reaches SAFE. Debug_1 is the best offline/test hypothesis with
only PLAUSIBLE occupancy confidence.
