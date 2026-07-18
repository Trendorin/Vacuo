# Vacuo 0.1.0

The first public release of Vacuo establishes a conservative Linux cache-cleaning foundation: native Qt 6 Widgets, a dependency-light C++20 core, explicit cleanup plans and a narrow PolicyKit boundary.

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
- No AppImage is published in 0.1.0: the privileged helper and PolicyKit policy require a native system installation, and shipping a misleading partially functional bundle would be worse than providing correct native packages.
