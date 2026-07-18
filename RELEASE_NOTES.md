# Vacuo 0.1.2

Vacuo 0.1.2 refreshes the application identity and makes installation, source builds and removal straightforward in English, Russian, Ukrainian and German. Cleanup behavior and the PolicyKit boundary are unchanged from 0.1.1.

## Included

| Asset | Purpose |
|---|---|
| `vacuo_0.1.2_amd64.deb` | Debian / Ubuntu package |
| `vacuo-0.1.2-1.x86_64.rpm` | Fedora / RHEL package |
| `PKGBUILD` | Arch-family package recipe |
| `vacuo-0.1.2-x86_64.tar.gz` | Installable Linux tree |
| `vacuo-0.1.2-source.tar.gz` | Release source |
| `vacuo-0.1.2.spdx` | SPDX 2.3 software bill of materials |
| `SHA256SUMS` | Integrity manifest for every asset |

## Changes

- New high-contrast application icon and compact repository header.
- English default README plus complete RU, UK and DE documentation.
- Exact package, source-build and uninstall commands.
- Clearer tables for cleanup scope, preserved data and distribution adapters.

## Safety

- Run the GUI as a normal user; system actions request PolicyKit authorization separately.
- Review selected categories before cleaning. Selected cache and Trash contents are deleted permanently.
- Browser profiles, cookies, history, passwords, settings, projects, games and saves remain outside the cleanup catalog.
- Arch package-cache cleanup requires `pacman-contrib`; Vacuo does not use `pacman -Scc`.

Verify downloads with `sha256sum --ignore-missing --check SHA256SUMS`.
