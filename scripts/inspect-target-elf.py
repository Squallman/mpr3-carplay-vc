#!/usr/bin/env python3
"""Read ELF headers/symbol tables only. No loading or executing objects.

Portable fallback for hosts without an ELF-aware nm/readelf.
"""
import pathlib
import struct
import sys


def inspect(path):
    data = pathlib.Path(path).read_bytes()
    if data[:6] != b"\x7fELF\x02\x01":
        raise ValueError("expected ELF64 little-endian object")
    header = struct.unpack_from("<16sHHIQQQIHHHHHH", data)
    if header[1] != 1 or header[2] != 183:
        raise ValueError("expected relocatable AArch64 ELF (ET_REL, EM_AARCH64)")
    sections = [struct.unpack_from("<IIQQQQIIQQ", data, header[6] + i * header[11])
                for i in range(header[12])]
    symbols = []
    for section in sections:
        if section[1] != 2:  # SHT_SYMTAB
            continue
        if section[9] != 24:
            raise ValueError("unexpected ELF64 symbol size")
        strings = sections[section[6]]
        table = data[strings[4]:strings[4] + strings[5]]
        for offset in range(section[4], section[4] + section[5], section[9]):
            name, info, other, index, value, size = struct.unpack_from("<IBBHQQ", data, offset)
            end = table.index(b"\0", name)
            symbols.append((table[name:end].decode(), info >> 4, info & 15,
                            other & 3, index, value, size))
    matches = [s for s in symbols if s[0] == "AirPlayReceiverSessionSetup"]
    if len(matches) != 1 or matches[0][1:4] != (1, 2, 0) or matches[0][4] == 0:
        raise ValueError("missing unique GLOBAL FUNC DEFAULT defined unmangled SETUP symbol")
    if any("SecondaryController" in s[0] for s in symbols):
        raise ValueError("entry object unexpectedly references SecondaryController")
    print("ELF64 little-endian / REL / AArch64")
    print("Value             Size Type Bind   Vis     Name")
    symbol = matches[0]
    print(f"{symbol[5]:016x} {symbol[6]:5d} FUNC GLOBAL DEFAULT AirPlayReceiverSessionSetup")
    for symbol in symbols:
        if symbol[1] == 1 and symbol[4] == 0:
            print(f"UND {symbol[0]}")


if __name__ == "__main__":
    try:
        inspect(sys.argv[1])
    except (IndexError, OSError, ValueError, struct.error) as error:
        sys.exit(f"ELF inspection failed: {error}")
