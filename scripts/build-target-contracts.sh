#!/bin/sh
# Opt-in HOST tests and unlinked objects only.
set -eu
root=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
build="$root/build-target-contracts"
posix=${MPR3_ENABLE_POSIX_TARGET_RESOLVER:-0}
case "$posix" in
  0) cmake_posix=OFF ;;
  1) cmake_posix=ON ;;
  *) echo "MPR3_ENABLE_POSIX_TARGET_RESOLVER must be 0 or 1" >&2; exit 1 ;;
esac
cflite_posix=${MPR3_ENABLE_POSIX_CFLITE_RESOLVER:-0}
case "$cflite_posix" in
  0) cmake_cflite_posix=OFF ;;
  1) cmake_cflite_posix=ON ;;
  *) echo "MPR3_ENABLE_POSIX_CFLITE_RESOLVER must be 0 or 1" >&2; exit 1 ;;
esac
if command -v cmake >/dev/null 2>&1; then
  cmake -S "$root" -B "$build" -DMPR3_BUILD_TARGET_CONTRACTS=ON \
    -DMPR3_ENABLE_POSIX_TARGET_RESOLVER="$cmake_posix" \
    -DMPR3_ENABLE_POSIX_CFLITE_RESOLVER="$cmake_cflite_posix"
  cmake --build "$build"
  ctest --test-dir "$build" --output-on-failure -V
else
  echo "CMake unavailable; compiling opt-in target contract HOST tests and objects."
  mkdir -p "$build"
  set --
  if [ "$posix" = 1 ]; then
    set -- -DMPR3_ENABLE_POSIX_TARGET_RESOLVER=1 \
      "$root/target/airplay/posix_setup_symbol_lookup.cpp"
    case $(uname -s) in Linux) set -- "$@" -ldl ;; esac
  fi
  "${CXX:-clang++}" -std=c++17 -Wall -Wextra -Wpedantic -Werror \
    -I"$root/include" -I"$root/target/include" -I"$root/target/tests" \
    "$root/target/airplay/original_setup_resolver.cpp" \
    "$root/target/airplay/setup_bridge.cpp" "$root/sidecar/runtime_config.cpp" \
    "$root/target/tests/test_original_setup_resolver.cpp" \
    "$root/target/tests/test_setup_bridge.cpp" "$root/target/tests/test_target_contracts.cpp" \
    "$@" -o "$build/mpr3_target_tests"
  for source in airplay_setup_entry abi_header_consumer cflite_abi_header_consumer; do
    case "$source" in
      airplay_setup_entry) input="$root/target/abi/$source.cpp" ;;
      *header_consumer) input="$root/target/tests/$source.cpp" ;;
    esac
    "${CXX:-clang++}" -std=c++17 -Wall -Wextra -Wpedantic -Werror \
      -I"$root/include" -I"$root/target/include" -c "$input" -o "$build/$source.o"
  done
  "${CXX:-clang++}" -std=c++17 -Wall -Wextra -Wpedantic -Werror \
    -I"$root/include" -I"$root/target/include" \
    -c "$root/target/airplay/setup_bridge.cpp" -o "$build/setup_bridge.o"
  "$root/scripts/check-target-dependencies.sh" "$build/setup_bridge.o"
  "$build/mpr3_target_tests"
  set --
  for source in cflite_api cf_setup_context cf_setup_adapter target_setup_filter; do
    "${CXX:-clang++}" -std=c++17 -Wall -Wextra -Wpedantic -Werror \
      -I"$root/include" -I"$root/target/include" \
      -c "$root/target/cflite/$source.cpp" -o "$build/$source.o"
    "$root/scripts/check-cf-adapter-dependencies.sh" "$build/$source.o"
    set -- "$@" "$build/$source.o"
  done
  if [ "$cflite_posix" = 1 ]; then
    "${CXX:-clang++}" -std=c++17 -Wall -Wextra -Wpedantic -Werror \
      -DMPR3_ENABLE_POSIX_CFLITE_RESOLVER=1 -I"$root/target/include" \
      -c "$root/target/cflite/posix_cflite_symbol_lookup.cpp" -o "$build/posix_cflite_symbol_lookup.o"
    "$root/scripts/check-cf-adapter-dependencies.sh" "$build/posix_cflite_symbol_lookup.o"
    set -- "$@" "$build/posix_cflite_symbol_lookup.o"
    case $(uname -s) in Linux) set -- "$@" -ldl ;; esac
  fi
  "${CXX:-clang++}" -std=c++17 -Wall -Wextra -Wpedantic -Werror \
    -I"$root/include" -I"$root/target/include" -I"$root/target/tests" \
    "$root/hook/airplay_setup_hook.cpp" "$root/target/tests/fake_cflite.cpp" \
    "$root/target/tests/host_allocation_failure.cpp" "$root/target/tests/test_cflite_api.cpp" \
    "$root/target/tests/test_cf_setup_adapter.cpp" "$@" -o "$build/mpr3_target_cf_tests"
  "$build/mpr3_target_cf_tests"
fi
