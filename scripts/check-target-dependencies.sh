#!/bin/sh
# Inspect host contract symbols to guard pass-through isolation.
set -eu
if [ "$#" != 1 ]; then
  echo "Usage: $0 <bridge-object-or-contract-archive>" >&2
  exit 1
fi
symbols=$(nm -u "$1")
if printf '%s\n' "$symbols" | rg 'SecondaryController|ISecondaryController|ScreenStream|dint_|setActiveDisplayable|Mock|(^|[[:space:]])_?CF(Retain|Release|Dictionary[A-Za-z0-9_]+|Array[A-Za-z0-9_]+)([[:space:]]|$)'; then
  echo "FAIL: pass-through references a secondary controller or unresolved service." >&2
  exit 1
fi
echo "PASS: pass-through symbols contain no secondary, CF, display or COMM dependency."
