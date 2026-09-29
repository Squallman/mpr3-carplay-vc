#!/bin/sh
# AArch64 compile/inspection only. Never links or executes target objects.
set -eu
root=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
build="$root/build-target-abi"
mkdir -p "$build"
# This dedicated directory must contain only the named inspection artifacts.
check_artifacts() {
  for artifact in "$build"/* "$build"/.[!.]* "$build"/..?*; do
    [ -e "$artifact" ] || continue
    case "$artifact" in
      "$build/airplay_setup_entry.o"|"$build/abi_header_consumer.o"|"$build/compiler.txt"|"$build/symbols.txt") ;;
      *) echo "Unexpected artifact in ABI inspection directory: $artifact" >&2; exit 1 ;;
    esac
  done
}
check_artifacts
if command -v aarch64-linux-gnu-g++ >/dev/null 2>&1; then
  compiler=aarch64-linux-gnu-g++
  set --
elif command -v clang++ >/dev/null 2>&1; then
  compiler=clang++
  set -- --target=aarch64-linux-gnu
else
  echo "SKIP: no installed AArch64 compiler or Clang cross-target capability."
  exit 0
fi
if ! "$compiler" "$@" -std=c++17 -Wall -Wextra -Wpedantic -Werror \
    -I"$root/include" -I"$root/target/include" -c "$root/target/abi/airplay_setup_entry.cpp" \
    -o "$build/airplay_setup_entry.o" >"$build/compiler.txt" 2>&1; then
  cat "$build/compiler.txt" >&2
  # Capability absence is a clean skip; source/ABI failures remain failures.
  if rg -q 'unable to create target|unknown target triple|No available targets|unsupported.*target' "$build/compiler.txt"; then
    echo "SKIP: installed compiler cannot generate aarch64-linux-gnu objects."
    exit 0
  fi
  echo "FAIL: entrypoint compilation failed." >&2
  exit 1
fi
"$compiler" "$@" -std=c++17 -Wall -Wextra -Wpedantic -Werror \
  -I"$root/include" -I"$root/target/include" -c "$root/target/tests/abi_header_consumer.cpp" \
  -o "$build/abi_header_consumer.o"
file "$build/airplay_setup_entry.o"
if command -v python3 >/dev/null 2>&1; then
  python3 "$root/scripts/inspect-target-elf.py" "$build/airplay_setup_entry.o" >"$build/symbols.txt"
  cat "$build/symbols.txt"
else
  echo "FAIL: python3 is required for portable ELF validation." >&2
  exit 1
fi
if command -v llvm-readelf >/dev/null 2>&1; then
  llvm-readelf -Ws "$build/airplay_setup_entry.o"
elif command -v readelf >/dev/null 2>&1; then
  readelf -Ws "$build/airplay_setup_entry.o"
elif command -v llvm-nm >/dev/null 2>&1; then
  llvm-nm -g "$build/airplay_setup_entry.o"
elif command -v nm >/dev/null 2>&1; then
  if ! nm -g "$build/airplay_setup_entry.o"; then
    echo "Host nm lacks ELF support; portable ELF symbol validation above passed."
  fi
else
  echo "nm/readelf unavailable; portable ELF symbol validation above passed."
fi
check_artifacts
echo "PASS: AArch64 ABI object and isolated header compiled; no linked target artifact."
