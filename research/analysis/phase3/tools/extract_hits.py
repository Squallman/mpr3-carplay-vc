import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[5] / '.local-research/mpr3/P3695/tffsmount'))
from tffsmount.interface import TFFS
from tffsmount.stream import Stream

image, outdir, *paths = sys.argv[1:]
outdir = Path(outdir)
with Stream(image, 0) as stream:
    fs = TFFS(stream)
    for source in paths:
        e = fs.get_dir_entry_from_path(Path(source))
        if e is None or e.is_dir:
            print(f'NOT FOUND OR DIR: {source}', file=sys.stderr)
            continue
        data = fs.read_file(e)
        target = outdir / source.lstrip('/')
        target.parent.mkdir(parents=True, exist_ok=True)
        target.write_bytes(data)
        print(f'{len(data):10d}  {source} -> {target}')
