# Vacuo 0.1.1

Vacuo 0.1.1 is the verified public baseline: native Qt 6 Widgets, a dependency-light C++20 core, explicit cleanup plans and a narrow PolicyKit boundary.

## Changes since 0.1.0

- Correct AppStream localization that passes strict Ubuntu validation.
- Clean Clang 18 build with `-Wconversion -Werror` and ASan/UBSan.
- Current `actions/checkout@v7`, `github/codeql-action@v4` and provenance attestation action.
- No cleanup boundary or category behavior changed.

## Included

- DEB and RPM packages, an installable TGZ tree, source archive and generated Arch `PKGBUILD`.
- Distribution adapters for Fedora/RHEL, Arch, Debian/Ubuntu, openSUSE and Alpine.
- User cache, thumbnails, browsers, developer tools, Flatpak app cache, Steam cache and Trash categories.
- Separately authorized package-cache, journal, coredump and Snap actions.
- `vacuoctl` with stable scan/clean JSON schemas.
- `SHA256SUMS`, SPDX 2.3 SBOM and GitHub build-provenance attestations.

## Important behavior

- Vacuo permanently removes selected cache.
- Browser profiles, cookies, history, passwords, application settings, downloads, projects, games and saves are outside the target catalog.
- The GUI must run as a normal user. System categories prompt through PolicyKit.
- Arch package-cache cleanup needs `pacman-contrib`; Vacuo does not fall back to `pacman -Scc`.
- No AppImage is published: the privileged helper and PolicyKit policy require a native system installation, and shipping a misleading partially functional bundle would be worse than providing correct native packages.
