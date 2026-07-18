#!/usr/bin/env bash
set -euo pipefail

root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
compiler="${CXX:-c++}"
output="${TMPDIR:-/tmp}/vacuo-core-tests"

sources=(
  "$root/src/core/types.cpp"
  "$root/src/core/system_detector.cpp"
  "$root/src/core/rule_catalog.cpp"
  "$root/src/core/scanner.cpp"
  "$root/src/core/safe_delete.cpp"
  "$root/src/core/process_runner.cpp"
  "$root/src/core/helper_actions.cpp"
  "$root/src/core/cleaner.cpp"
  "$root/src/core/json.cpp"
  "$root/tests/test_main.cpp"
)

"$compiler" -std=c++20 -O2 -g -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror \
  -I"$root/include" "${sources[@]}" -pthread -o "$output"
"$output"
