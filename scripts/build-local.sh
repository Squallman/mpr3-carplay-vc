#!/bin/sh
set -eu
root=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
if command -v cmake >/dev/null 2>&1; then
  cmake -S "$root" -B "$root/build"
  cmake --build "$root/build"
  ctest --test-dir "$root/build" --output-on-failure
else
  echo "CMake is not installed. Install CMake or use the documented direct Apple Clang command." >&2
  exit 2
fi
