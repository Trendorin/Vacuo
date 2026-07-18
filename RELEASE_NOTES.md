# Vacuo 0.1.4

Vacuo 0.1.4 aligns the repository presentation with MetaDrop. The README now uses only the centered application icon, while the existing native package and security boundaries remain unchanged.

## Included

| Asset | Purpose |
|---|---|
| `vacuo_0.1.4_amd64.deb` | Debian / Ubuntu package |
| `vacuo-0.1.4-1.x86_64.rpm` | Fedora / RHEL package |
| `PKGBUILD` | Arch-family package recipe |
| `vacuo-0.1.4-x86_64.tar.gz` | Installable Linux tree |
| `vacuo-0.1.4-source.tar.gz` | Release source |
| `vacuo-0.1.4.spdx` | SPDX 2.3 software bill of materials |
| `SHA256SUMS` | Integrity manifest for every asset |

## Changes

- Removed the wide README banner; the application icon is centered without a surrounding hero background.
- Kept the same concise EN, RU, UK and DE documentation structure as MetaDrop.
- Standardized installed project documents and release presentation.

## Safety

- Run the GUI as a normal user; system actions request PolicyKit authorization separately.
- Selected cache and Trash contents are deleted permanently.
- Browser profiles, cookies, history, passwords, settings, projects, games and saves remain outside the cleanup catalog.
- Arch package-cache cleanup requires `pacman-contrib`; Vacuo does not use `pacman -Scc`.

Verify downloads with `sha256sum --ignore-missing --check SHA256SUMS`.
