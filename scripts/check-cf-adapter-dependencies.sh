#!/bin/sh
# Inspect production adapter objects/static archives, never execute target code.
set -eu
if [ "$#" != 1 ]; then echo "Usage: $0 object-or-static-archive" >&2; exit 1; fi
symbols=$(nm "$1")
if printf '%s\n' "$symbols" | rg 'Mock|FakeCFLite|cflite_test|SecondaryController|ScreenStream|display.?init|dint_|setActiveDisplayable'; then
  echo "FAIL: unexpected CF adapter production dependency" >&2
  exit 1
fi
echo "PASS: CF adapter production dependency boundary ($1)"
