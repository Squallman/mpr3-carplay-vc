# SETUP response ownership

Source: libairplay.so, Setup @ 0x58dc0 and ServerControl @ 0x4b890.
All conclusions concern static paths, not execution.

| Path | Exact binary fact | Reconstructed contract | Confidence |
|---|---|---|---|
| Caller initialization | ServerControl 0x4c5a0 stores zero to stack+0xd0; 0x4c7b0 passes its address as x2 | Stock caller starts responseOut null | PROVEN |
| Internal construction | Setup 0x58e38 creates mutable dictionary; owned local saved at stack+0xc0 | Setup constructs response independently of incoming *responseOut | STRONG EVIDENCE |
| Successful transfer | 0x597ac–0x597b8 tests responseOut and stores local dictionary if nonnull; skips local release | One created reference transferred to caller on ordinary success | STRONG EVIDENCE |
| Ordinary failure | 0x591dc–0x591e0 loads/releases local response; early null-session / allocation failures 0x5a088 / 0x5a090 | Does not generally initialize or overwrite caller's output on error | STRONG EVIDENCE |
| Caller success | Serialize response 0x4c824; release request 0x4c83c; guarded response release 0x4c850 | Caller consumes returned dictionary reference after serialization | STRONG EVIDENCE |
| Caller Setup error | 0x4cd70 sets HTTP status 400 then branches 0x4c830 to same guarded releases | Cleanup covers ordinary Setup failure and null response | PROVEN |
| Null responseOut success | 0x597b0 skips store, successful exit still bypasses local response release | Apparent unreleased local reference on this path; not a safe wrapper convention | STRONG EVIDENCE |

PROVEN: Setup has no entry store clearing *responseOut. A wrapper must preserve
the caller's pointer and value before calling stock; it cannot rely on an error
always producing null. Stock's own null initialization makes its guarded
cleanup safe. Success/error behavior must remain distinct from resolver failure.

The request is borrowed by Setup; its stock caller releases it on ordinary
success and failure. Response mutation uses a separate dictionary and newly
created response stream dictionaries. A temporary request copy need not change
response ownership: forwarding the original responseOut pointer leaves transfer
and caller release with stock. This separation is STRONG EVIDENCE, conditional
on confirming all request consumers obey that borrowed lifetime.

Setup invokes platform control and failure/session delegates and starts worker
state. The normal cleanup edges were traced, including null session and
allocation failure. C++ cold exception regions through approximately 0x5b470
were inspected as candidates but not fully reconstructed. Universal guarantees
for exception propagation, callback retention and concurrent teardown remain
UNKNOWN. Do not turn this table into an unconditional post-return release rule
for every future target path. See [shallow-copy contract](cf-shallow-copy-contract.md).
