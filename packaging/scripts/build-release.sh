#!/usr/bin/env bash
set -euo pipefail

root="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
build="${BUILD_DIR:-$root/build-release}"
dist="${DIST_DIR:-$root/dist}"

cmake -S "$root" -B "$build" -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX=/usr \
  -DVACUO_WARNINGS_AS_ERRORS=ON
cmake --build "$build" --parallel
ctest --test-dir "$build" --output-on-failure

mkdir -p "$dist"
(cd "$build" && cpack -G DEB && cpack -G RPM && cpack -G TGZ && cpack --config CPackSourceConfig.cmake)
find "$build" -maxdepth 1 -type f \( -name '*.deb' -o -name '*.rpm' -o -name '*.tar.gz' \) \
  -exec cp --target-directory="$dist" {} +
(cd "$dist" && sha256sum -- * > SHA256SUMS)

printf 'Release artifacts: %s\n' "$dist"
