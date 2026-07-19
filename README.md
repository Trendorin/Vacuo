<p align="center">
  <img src="data/icons/hicolor/512x512/apps/io.github.trendorin.Vacuo.png" width="144" alt="Vacuo application icon">
</p>

<h1 align="center">Vacuo</h1>
<p align="center">Controlled cache cleanup for Linux.</p>

<p align="center">
  <a href="README.md"><img alt="English" src="https://img.shields.io/badge/EN-English-d7dade?style=flat-square&labelColor=15181b"></a>
  <a href="README.ru.md"><img alt="Русский" src="https://img.shields.io/badge/RU-Русский-d7dade?style=flat-square&labelColor=15181b"></a>
  <a href="README.uk.md"><img alt="Українська" src="https://img.shields.io/badge/UK-Українська-d7dade?style=flat-square&labelColor=15181b"></a>
  <a href="README.de.md"><img alt="Deutsch" src="https://img.shields.io/badge/DE-Deutsch-d7dade?style=flat-square&labelColor=15181b"></a>
</p>

<p align="center">
  <a href="https://github.com/Trendorin/Vacuo/actions/workflows/ci.yml"><img alt="CI" src="https://img.shields.io/github/actions/workflow/status/Trendorin/Vacuo/ci.yml?branch=main&style=flat-square&label=build&labelColor=15181b&color=5d666d"></a>
  <a href="https://github.com/Trendorin/Vacuo/releases/latest"><img alt="Release" src="https://img.shields.io/github/v/release/Trendorin/Vacuo?style=flat-square&label=release&labelColor=15181b&color=5d666d"></a>
  <img alt="C++20" src="https://img.shields.io/badge/C%2B%2B-20-5d666d?style=flat-square&labelColor=15181b">
  <img alt="Qt 6 Widgets" src="https://img.shields.io/badge/Qt-6_Widgets-5d666d?style=flat-square&labelColor=15181b">
  <a href="LICENSE"><img alt="GPL-3.0-or-later" src="https://img.shields.io/badge/license-GPL--3.0--or--later-5d666d?style=flat-square&labelColor=15181b"></a>
</p>

<p align="center">
  <a href="#install">Install</a> ·
  <a href="#build-from-source">Source</a> ·
  <a href="#uninstall">Uninstall</a> ·
  <a href="docs/SECURITY_MODEL.md">Security</a> ·
  <a href="https://github.com/Trendorin/Vacuo/releases">Releases</a>
</p>

Vacuo is a native Linux utility that measures known cache locations, shows exactly what can be removed, and cleans only selected categories. It uses a C++20 core and a Qt 6 Widgets interface that follows the active desktop theme. No Electron, telemetry, account, cloud service, or background daemon.

The interface follows the system language or switches immediately between English, Russian, Ukrainian and German in Settings.

## What it does

| Scope | Included | Default |
|---|---|:---:|
| Rebuildable user data | Application cache and thumbnails | On |
| Optional user data | Browser, developer, Flatpak and Steam caches | Off |
| Permanent user data | Trash files and Trash metadata | Off |
| System data | Package cache, archived journal, coredumps and Snap cache | Off + PolicyKit |

Browser profiles, cookies, history, passwords, application settings, projects, games and saves are not targets. Unknown distributions receive only conservative XDG user rules; Vacuo never guesses a package-manager command.

### Supported systems

| Distribution family | Package-cache adapter |
|---|---|
| Fedora / RHEL | `dnf5 clean all` or `dnf clean all` |
| Arch / CachyOS / Manjaro | `paccache -rk2` |
| Debian / Ubuntu | `apt-get clean` |
| openSUSE / SLES | `zypper --non-interactive clean --all` |
| Alpine | `apk cache clean` |

KDE Plasma, GNOME, Xfce, Cinnamon, LXQt, Wayland and X11 keep control of the window frame, palette and Qt style.

<a id="install"></a>
## Install

Download the matching asset from the [latest release](https://github.com/Trendorin/Vacuo/releases/latest).

| System | Asset | Command |
|---|---|---|
| Fedora 44 | `vacuo-*.rpm` | `sudo dnf install ./vacuo-*.rpm` |
| Ubuntu 24.04 | `vacuo_*.deb` | `sudo apt install ./vacuo_*.deb` |
| Arch Linux | `vacuo-*-x86_64.pkg.tar.zst` | `sudo pacman -U ./vacuo-*-x86_64.pkg.tar.zst` |

Each binary package is built and install-tested in its native target. `PKGBUILD` is included for a source rebuild with `makepkg -si`. On Arch, `pacman-contrib` enables safe package-cache cleanup; Vacuo has no `pacman -Scc` fallback.

Verify downloaded files with the release manifest:

```bash
sha256sum --ignore-missing --check SHA256SUMS
```

<a id="build-from-source"></a>
## Build from source

Requirements: CMake 3.24+, a C++20 compiler, Ninja or Make, Qt 6.4+ (`Widgets`, `Concurrent`, `LinguistTools`) and PolicyKit for system actions.

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

The installed helper must remain a regular root-owned `0755` executable. Never add setuid or setgid bits.

<a id="uninstall"></a>
## Uninstall

| Installation | Command |
|---|---|
| Fedora 44 | `sudo dnf remove vacuo` |
| Ubuntu 24.04 | `sudo apt remove vacuo` |
| Arch Linux | `sudo pacman -Rns vacuo` |
| Source build | `sudo xargs -r -d '\n' rm -- < build/install_manifest.txt` |

Optional: remove saved window geometry with `rm -f "$HOME/.config/Trendorin/Vacuo.conf"`.

## Safety model

- Scan is read-only; cleanup always requires an explicit selection and confirmation.
- GUI and CLI refuse cleanup when run as root.
- User targets come from an exact compiled allow-list; arbitrary paths are not accepted.
- Symlinked components and protected roots are rejected before descriptor-relative deletion.
- The non-setuid PolicyKit helper accepts four fixed system action IDs and never executes a shell command supplied by the user.

Read the [security model](docs/SECURITY_MODEL.md), [privacy statement](docs/PRIVACY.md), and [vulnerability policy](SECURITY.md).

## CLI

```bash
vacuoctl system
vacuoctl list
vacuoctl scan --json
vacuoctl clean --category thumbnails --category application-cache --yes
```

There is intentionally no implicit `clean all` command.

## Project

[Changelog](CHANGELOG.md) · [Contributing](CONTRIBUTING.md) · [Contributors](CONTRIBUTORS.md) · [Support](SUPPORT.md)

Maintained by [Trendorin](https://github.com/Trendorin). Licensed under [GPL-3.0-or-later](LICENSE).
