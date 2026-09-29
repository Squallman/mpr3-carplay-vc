#!/bin/sh
set -eu
root=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
if command -v cmake >/dev/null 2>&1; then
  cmake -S "$root" -B "$root/build"
  cmake --build "$root/build"
  ctest --test-dir "$root/build" --output-on-failure
else
  echo "CMake unavailable; building offline tests directly with the host C++ compiler."
  mkdir -p "$root/build"
  "${CXX:-clang++}" -std=c++17 -Wall -Wextra -Wpedantic -Werror \
    -DMPR3_ENABLE_TARGET_LOADER=0 -I"$root/include" -I"$root/mocks/include" -I"$root/tests" \
    "$root"/hook/*.cpp "$root"/sidecar/*.cpp "$root"/mocks/*.cpp \
    "$root"/tests/*.cpp -o "$root/build/mpr3_tests"
  "$root/build/mpr3_tests"
fi
