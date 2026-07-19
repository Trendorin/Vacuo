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
(cd "$build" && cpack -G DEB && cpack -G TGZ)
find "$build" -maxdepth 1 -type f \( -name '*.deb' -o -name '*.tar.gz' \) \
  -exec cp --target-directory="$dist" {} +

version="$(sed -n 's/.*kVersion = "\([0-9][0-9.]*\)".*/\1/p' "$root/include/vacuo/version.hpp")"
test -n "$version"
git -C "$root" archive --format=tar.gz \
  --prefix="vacuo-$version-source/" \
  --output="$dist/vacuo-$version-source.tar.gz" HEAD
source_hash="$(sha256sum "$dist/vacuo-$version-source.tar.gz" | cut -d' ' -f1)"
sed -e "s/@VERSION@/$version/g" -e "s/@SOURCE_SHA256@/$source_hash/g" \
  "$root/packaging/arch/PKGBUILD.in" > "$dist/PKGBUILD"
"$root/packaging/scripts/generate-sbom.sh" "$version" > "$dist/vacuo-$version.spdx"
(cd "$dist" && sha256sum -- * > SHA256SUMS)

printf 'Portable release artifacts: %s\n' "$dist"
printf 'Fedora RPM and Arch packages are built only in their native CI jobs.\n'
