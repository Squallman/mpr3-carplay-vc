# Agent instructions

1. Read `README.md`, `docs/architecture.md`, `docs/research-status.md`,
   `docs/ai-context.md`, and relevant `research/analysis/` reports before work.
2. Never convert `UNKNOWN` into fact. Do not transfer MHI2Q/MHI3 findings to
   MPR3 without direct evidence.
3. Preserve stock stream type 110 unless an explicitly justified task changes
   that boundary.
4. Secondary failure must fail open and never break normal CarPlay.
5. `.local-research/mpr3/P3695/` is optional, local, ignored firmware data;
   never commit it.
6. This repository is currently an offline-only prototype. Do not add flashing,
   installation, vehicle connection, or deployment tooling unless explicitly
   requested.
7. Do not modify stock ELF files unless explicitly requested in a future task.
8. Use interfaces and mocks for unresolved contracts.
9. Do not hard-code a cluster endpoint, safe displayable ID, secondary
   descriptor fields, or ThemeAssets/parameter-17 behavior without proof.
10. Do not commit or push unless explicitly requested.
