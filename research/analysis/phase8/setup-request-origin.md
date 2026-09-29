# SETUP request creation and parser ownership

## Recovered origin

**STRONG EVIDENCE** closes the received-container callback-table uncertainty.
In libairplay's ServerControl, 0x4c63c loads the HTTP message body pointer/length
from message+0x518/+0x530. At 0x4c64c it calls
`CFBinaryPlistV0CreateWithData` with x0=bytes, x1=length and x2=int32 error-out.
The return is checked as a dictionary at 0x4c660/0x4c670 and is held in x27.
The parser result is owned by the control caller, which releases it at 0x4c83c
on ordinary success and error. This is the observed binary-plist control path,
not a declaration that all conceivable request entrypoints use that parser.

## Container construction

| Stage | Exact evidence | Ownership consequence | Confidence |
|---|---|---|---|
| Root parser | Export 0x81a80; recursive parse called at 0x81c44 | Returns one owned root reference or null/error | STRONG EVIDENCE |
| Parser object cache | Created at 0x81c24; released 0x81c74 | Cache owns its values while parsing, not after root return | STRONG EVIDENCE |
| Cached repeated object | Recursive helper 0x806c0, retain at 0x80724 | Reused object identity is returned with an acquired reference | PROVEN (retain) |
| Dictionary marker | Mutable dictionary construction 0x807d4–0x807ec | Standard retaining key/value tables, null allocator, zero capacity | PROVEN (arguments) |
| Dictionary children | Parse at 0x80868/0x808ac, set 0x808c8, releases 0x808d0/0x808d8 | Container keeps children; parser drops local child references | STRONG EVIDENCE |
| Array marker | Mutable array 0x80984–0x80994, callback pointer via GOT 0x15ca08 | Standard retaining array table | PROVEN (arguments) |
| Array child | Parse 0x809f4, append 0x80a0c, release 0x80a14 | Container retains the same child object | STRONG EVIDENCE |
| Parser temporary storage | Free 0x81c54; cache release 0x81c74 before root return 0x81c8c | Root does not depend on cache or parser allocation surviving | STRONG EVIDENCE |

The retaining tables are the same ones recovered in Phase7: dictionary keys
0x159318, values 0x1592f0 and array values 0x159348. Their retain/release
functions and dictionary/array finalizers were traced there; Phase8 connects
those tables to received SETUP containers.

## Packet-buffer independence

Parsed strings use CFStringCreateWithBytes at 0x80b0c/0x80c50 and parsed data
uses CFDataCreate at 0x80c88. CFLDataCreate 0x7b560 allocates owned byte storage
and memcpy-copies it at 0x7b5ec. CFStringCreateWithBytes 0x779d0 reaches
CFLStringCreateWithText 0x7d380; its text setter allocates a buffer and copies
text at 0x7d348. These are copying constructors, not NoCopy views into the HTTP
body. Numeric values are constructed as CF values, not pointers into packet
bytes. Shared plist references share CF objects with refcounts, not borrowed
packet storage.

Exact original HTTP-buffer destruction timing is not needed for the shallow
copy contract: the parsed graph owns its storage, and control keeps its root
through SETUP. No additional root owner outside the ordinary control call was
found on this path. Arbitrary manually supplied dictionaries, replaced parser
implementations and concurrent mutation are outside the inspected contract.
