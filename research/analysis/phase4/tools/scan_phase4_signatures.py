#!/usr/bin/env python3
import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[5] / ".local-research/mpr3/P3695/tffsmount"))
from tffsmount.interface import TFFS
from tffsmount.stream import Stream

SIGS = [
    b"setActiveDisplayable", b"setDisplayable", b"requestVideoConnection",
    b"releaseVideoConnection", b"createDisplayable", b"registerDisplayable",
    b"unregisterDisplayable", b"getDisplayable", b"IpTeConnection",
    b"IpteConnection", b"Displayable_Cluster_Map",
    b"Displayable_Cluster_Map_Route_Guidance", b"Displayable_Hud_Map",
    b"Displayable_External_Smarthphone", b"WirelessCarPlayTransportComponent",
    b"ThemeAssets", b"altScreen", b"enabledFeatures", b"displayUUID",
    b"routeGuidance", b"cluster", b"navigation", b"fpk",
]

def walk(fs, entry, path="/", seen=None):
    seen = set() if seen is None else seen
    if entry.inode_number in seen:
        return
    seen.add(entry.inode_number)
    try:
        children = fs.read_dir(entry)
    except Exception:
        return
    for child in children:
        if not child.name or child.name == "$TFFS_Indirect_Inodes" or child.inode_number == 2:
            continue
        p = "/" + child.name if path == "/" else path.rstrip("/") + "/" + child.name
        try:
            e = fs.get_dir_entry_from_path(Path(p))
        except Exception:
            continue
        if e is None:
            continue
        if e.is_dir:
            yield from walk(fs, e, p, seen)
        else:
            yield p, e

for image in map(Path, sys.argv[1:]):
    with Stream(str(image), 0) as stream:
        fs = TFFS(stream)
        for p, e in walk(fs, fs.root):
            try:
                data = fs.read_file(e)
            except Exception:
                continue
            for sig in SIGS:
                if sig in data:
                    print(f"{image}\t{p}\t{sig.decode()}\t{e.size}")
