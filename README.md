<p align="center">
  <img src="docs/assets/hero.svg" width="100%" alt="Vacuo — native cache control for Linux">
</p>

<p align="center">
  <a href="https://github.com/Trendorin/Vacuo/actions/workflows/ci.yml"><img alt="CI" src="https://img.shields.io/github/actions/workflow/status/Trendorin/Vacuo/ci.yml?branch=main&amp;style=flat-square&amp;label=build&amp;labelColor=111315&amp;color=555b60"></a>
  <a href="https://github.com/Trendorin/Vacuo/security/code-scanning"><img alt="CodeQL" src="https://img.shields.io/github/actions/workflow/status/Trendorin/Vacuo/codeql.yml?branch=main&amp;style=flat-square&amp;label=CodeQL&amp;labelColor=111315&amp;color=555b60"></a>
  <a href="https://github.com/Trendorin/Vacuo/releases/latest"><img alt="Release" src="https://img.shields.io/github/v/release/Trendorin/Vacuo?style=flat-square&amp;label=release&amp;labelColor=111315&amp;color=555b60"></a>
  <a href="LICENSE"><img alt="GPL-3.0-or-later" src="https://img.shields.io/badge/license-GPL--3.0--or--later-555b60?style=flat-square&amp;labelColor=111315"></a>
</p>

<p align="center">
  <a href="#install"><img alt="Install" src="https://img.shields.io/badge/INSTALL-2d3135?style=for-the-badge"></a>
  <a href="#safety"><img alt="Safety" src="https://img.shields.io/badge/SAFETY-2d3135?style=for-the-badge"></a>
  <a href="https://github.com/Trendorin/Vacuo/releases"><img alt="Releases" src="https://img.shields.io/badge/RELEASES-2d3135?style=for-the-badge"></a>
  <a href="README_RU.md"><img alt="Русская версия" src="https://img.shields.io/badge/РУССКАЯ_ВЕРСИЯ-2d3135?style=for-the-badge"></a>
</p>

Vacuo is a native Linux cache cleaner with a C++20 backend and a Qt 6 Widgets interface. It scans first, explains every category, and changes only what the user explicitly selects. There is no Electron runtime, telemetry, account, cloud service, or background daemon.

## What makes it different

| Native by default | Explicit cleanup | Small privilege boundary |
|---|---|---|
| Standard Qt Widgets, normal system window frame, desktop icon theme and palette. | Read-only scan, per-category sizes, risk labels, warnings and a final confirmation. | The GUI never runs as root. A PolicyKit helper accepts fixed action IDs—not paths or shell commands. |

Vacuo does not install a custom visual theme. KDE Plasma, GNOME, Xfce, Cinnamon, LXQt and Wayland compositors keep control through the system Qt style and palette.

## Capabilities

- Detects `/etc/os-release`, distribution family, package manager, desktop session, Wayland/X11 and the filesystem mounted for the home directory.
- Scans allocated disk usage without following symbolic links.
- Separates low-risk rebuildable cache from browser, developer, gaming, Trash and privileged system categories.
- Supports DNF/DNF5, pacman through `paccache`, APT, zypper and APK cache cleanup.
- Vacuums archived systemd journal data older than 14 days; active journal files stay intact.
- Provides `vacuoctl` with stable JSON schemas for audit and automation.
- Builds without third-party C++ dependencies beyond Qt for the GUI.

### Cleanup catalog

| Category | Scope | Default | Behavior |
|---|---:|:---:|---|
| Application cache | User | On | Clears general `$XDG_CACHE_HOME` content, excluding separately listed groups. |
| Thumbnails | User | On | Clears Freedesktop thumbnail previews. |
| Browser cache | User | Off | Clears cache directories only; profiles, cookies, history, passwords and extensions are not targets. |
| Developer cache | User | Off | npm, Gradle, Cargo, pip, uv, Go, Yarn and supported IDE caches. |
| Flatpak app cache | User | Off | Only `~/.var/app/<app-id>/cache`; settings and application data remain. |
| Steam cache | User | Off | Shader and embedded web caches; game data and saves remain. |
| Trash | User | Off | Permanently removes Trash files and metadata. |
| Package cache | System | Off | Uses the detected package manager through the restricted helper. |
| Journal / crash dumps / Snap cache | System | Off | Fixed system actions, each selected separately. |

Unknown distributions receive only the safe XDG/user rules. Vacuo does not guess a package-manager command.

<a id="safety"></a>
## Safety

Vacuo treats deletion as a security boundary, not a convenience function.

- The GUI and CLI refuse to clean while running as root.
- User targets come from a compiled, exact allow-list. There is no arbitrary-path field.
- `/`, the home directory, configuration roots and broad data roots are hard-denied.
- Every target must be a real directory owned by the current user; symlinked path components are rejected.
- Traversal uses descriptor-relative `openat`, `fstatat` and `unlinkat` with no-follow flags, limiting symlink swaps and path traversal.
- The helper is not setuid. PolicyKit starts it only when a selected system action needs authorization.
- The helper accepts four fixed identifiers and launches absolute, allow-listed executables without a shell.
- Concurrent privileged runs are blocked by an exclusive lock.
- Arch cleanup requires `paccache`; Vacuo deliberately refuses the destructive `pacman -Scc` fallback.

See the complete [security model](docs/SECURITY_MODEL.md), [privacy statement](docs/PRIVACY.md), and [vulnerability policy](SECURITY.md).

Vacuo permanently deletes selected cache. Run the scan, read warnings, close affected applications, and keep backups of data that is not reproducible.

<a id="install"></a>
## Install

Download the package for your distribution from [GitHub Releases](https://github.com/Trendorin/Vacuo/releases/latest). Every release includes `SHA256SUMS`, an SPDX SBOM, source archive and build-provenance attestation.

### Fedora / RHEL family

```bash
sudo dnf install ./vacuo-0.1.0-*.rpm
```

### Debian / Ubuntu family

```bash
sudo apt install ./vacuo_0.1.0_*.deb
```

### Arch family

Download `PKGBUILD` and the source archive from the same release, then inspect and build it:

```bash
makepkg -si
```

Install `pacman-contrib` to enable the safe package-cache adapter.

### Build from source

Requirements: a C++20 compiler, CMake 3.24+, Ninja or Make, Qt 6.4+ (`Widgets` and `Concurrent`), and PolicyKit at runtime for system actions.

```bash
git clone https://github.com/Trendorin/Vacuo.git
cd Vacuo
cmake -S . -B build -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX=/usr
cmake --build build --parallel
ctest --test-dir build --output-on-failure
sudo cmake --install build
```

The installed helper is a normal root-owned `0755` executable. Do not add setuid/setgid bits.

## CLI

```bash
vacuoctl system
vacuoctl list
vacuoctl scan
vacuoctl scan --json
vacuoctl clean --category thumbnails --category application-cache --yes
```

There is intentionally no implicit `clean all`. System categories use the same PolicyKit helper as the GUI.

## Distribution adapters

| Family | Detection | Package-cache action |
|---|---|---|
| Fedora, RHEL, Rocky, Alma | `ID` / `ID_LIKE` | `dnf5 clean all` or `dnf clean all` |
| Arch, CachyOS, Manjaro, EndeavourOS | `ID` / `ID_LIKE` | `paccache -rk2` |
| Debian, Ubuntu, Mint, Pop!_OS, Kali | `ID` / `ID_LIKE` | `apt-get clean` |
| openSUSE / SLES | `ID` / `ID_LIKE` | `zypper --non-interactive clean --all` |
| Alpine | `ID` / `ID_LIKE` | `apk cache clean` |
| NixOS / unknown | detected | package-cache cleanup disabled |

Filesystem detection is informational and mount-aware. Deletion remains filesystem-agnostic and uses Linux VFS APIs; Vacuo does not run filesystem repair or snapshot commands.

## Development

The core has no Qt dependency and can be tested on a minimal Linux system:

```bash
./tests/run-core-tests.sh
```

The full build is checked on Ubuntu, Fedora and Arch. A separate Clang job runs AddressSanitizer and UndefinedBehaviorSanitizer; CodeQL analyzes every main-branch change.

Before contributing a rule, read [CONTRIBUTING.md](CONTRIBUTING.md). New privileged behavior must use a fixed action identifier, an absolute command allow-list, negative tests, and documentation of what is preserved.

## Project policy

- [Changelog](CHANGELOG.md)
- [Security policy](SECURITY.md)
- [Privacy](docs/PRIVACY.md)
- [Contributors](CONTRIBUTORS.md)
- [Support](SUPPORT.md)

Vacuo is maintained by [Trendorin](https://github.com/Trendorin). Community contributors are listed by Git history and the [contributors graph](https://github.com/Trendorin/Vacuo/graphs/contributors); no fabricated contributor list is kept.

## License

Copyright © 2026 Trendorin. Licensed under [GNU GPL v3 or later](LICENSE). Contributions are accepted under the same license.
