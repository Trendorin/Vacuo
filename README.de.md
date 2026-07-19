<p align="center">
  <img src="data/icons/hicolor/512x512/apps/io.github.trendorin.Vacuo.png" width="144" alt="Vacuo-Anwendungssymbol">
</p>

<h1 align="center">Vacuo</h1>
<p align="center">Kontrollierte Cache-Bereinigung für Linux.</p>

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
  <a href="#install">Installation</a> ·
  <a href="#build-from-source">Quellcode</a> ·
  <a href="#uninstall">Deinstallation</a> ·
  <a href="docs/SECURITY_MODEL.md">Sicherheit</a> ·
  <a href="https://github.com/Trendorin/Vacuo/releases">Releases</a>
</p>

Vacuo ist ein natives Linux-Werkzeug, das bekannte Cache-Verzeichnisse misst, einen genauen Bereinigungsplan anzeigt und nur ausgewählte Kategorien löscht. Der Kern ist in C++20 geschrieben; die Qt-6-Widgets-Oberfläche übernimmt das aktive Systemdesign. Kein Electron, keine Telemetrie, kein Konto, kein Cloud-Dienst und kein Hintergrunddienst.

Die Oberfläche folgt der Systemsprache oder wechselt in den Einstellungen sofort zwischen Englisch, Russisch, Ukrainisch und Deutsch.

## Funktionsumfang

| Bereich | Enthalten | Standard |
|---|---|:---:|
| Wiederherstellbare Daten | Anwendungs-Cache und Vorschaubilder | Ein |
| Optionale Kategorien | Browser-, Entwickler-, Flatpak- und Steam-Cache | Aus |
| Endgültiges Löschen | Dateien und Metadaten des Papierkorbs | Aus |
| Systemdaten | Paket-Cache, archiviertes Journal, Coredumps und Snap-Cache | Aus + PolicyKit |

Browserprofile, Cookies, Verlauf, Passwörter, Anwendungseinstellungen, Projekte, Spiele und Spielstände werden nicht verarbeitet. Auf unbekannten Distributionen aktiviert Vacuo nur konservative XDG-Regeln für Benutzerdaten und errät keine Paketmanager-Befehle.

### Unterstützte Systeme

| Distributionsfamilie | Paket-Cache-Befehl |
|---|---|
| Fedora / RHEL | `dnf5 clean all` oder `dnf clean all` |
| Arch / CachyOS / Manjaro | `paccache -rk2` |
| Debian / Ubuntu | `apt-get clean` |
| openSUSE / SLES | `zypper --non-interactive clean --all` |
| Alpine | `apk cache clean` |

Unter KDE Plasma, GNOME, Xfce, Cinnamon, LXQt, Wayland und X11 bestimmen das System und der aktive Qt-Stil Fensterrahmen und Farben.

<a id="install"></a>
## Installation

Lade das passende Paket aus dem [aktuellen Release](https://github.com/Trendorin/Vacuo/releases/latest) herunter.

| System | Datei | Befehl |
|---|---|---|
| Fedora 44 | `vacuo-*.rpm` | `sudo dnf install ./vacuo-*.rpm` |
| Ubuntu 24.04 | `vacuo_*.deb` | `sudo apt install ./vacuo_*.deb` |
| Arch Linux | `vacuo-*-x86_64.pkg.tar.zst` | `sudo pacman -U ./vacuo-*-x86_64.pkg.tar.zst` |

Jedes Binärpaket wird im jeweiligen Zielsystem gebaut und installiert geprüft. Für einen Quellcode-Build bleibt `PKGBUILD` mit `makepkg -si` enthalten. Unter Arch ermöglicht `pacman-contrib` die sichere Paket-Cache-Bereinigung; einen Fallback auf `pacman -Scc` gibt es nicht.

Heruntergeladene Dateien prüfen:

```bash
sha256sum --ignore-missing --check SHA256SUMS
```

<a id="build-from-source"></a>
## Aus dem Quellcode bauen

Benötigt werden CMake 3.24+, ein C++20-Compiler, Ninja oder Make, Qt 6.4+ (`Widgets`, `Concurrent`) und PolicyKit für Systemaktionen.

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

Der installierte Helper muss eine normale, root-eigene Datei mit Modus `0755` bleiben. Keine setuid- oder setgid-Bits hinzufügen.

<a id="uninstall"></a>
## Deinstallation

| Installationsart | Befehl |
|---|---|
| Fedora 44 | `sudo dnf remove vacuo` |
| Ubuntu 24.04 | `sudo apt remove vacuo` |
| Arch Linux | `sudo pacman -Rns vacuo` |
| Quellcode-Build | `sudo xargs -r -d '\n' rm -- < build/install_manifest.txt` |

Optional gespeicherte Fenstergeometrie entfernen: `rm -f "$HOME/.config/Trendorin/Vacuo.conf"`.

## Sicherheitsmodell

- Der Scan ist schreibgeschützt; jede Bereinigung erfordert Auswahl und Bestätigung.
- GUI und CLI verweigern die Bereinigung bei Ausführung als root.
- Benutzerziele stammen aus einer festen Allowlist; beliebige Pfade werden nicht akzeptiert.
- Symlink-Komponenten und geschützte Wurzelverzeichnisse werden vor dem deskriptorrelativen Löschen abgewiesen.
- Der nicht-setuid PolicyKit-Helper akzeptiert vier feste Systemaktionen und führt keine vom Benutzer übergebene Shell-Anweisung aus.

Siehe [Sicherheitsmodell](docs/SECURITY_MODEL.md), [Datenschutzerklärung](docs/PRIVACY.md) und [Richtlinie für Schwachstellen](SECURITY.md).

## CLI

```bash
vacuoctl system
vacuoctl list
vacuoctl scan --json
vacuoctl clean --category thumbnails --category application-cache --yes
```

Es gibt bewusst keinen impliziten Befehl `clean all`.

## Projekt

[Änderungen](CHANGELOG.md) · [Mitwirken](CONTRIBUTING.md) · [Mitwirkende](CONTRIBUTORS.md) · [Support](SUPPORT.md)

Betreut von [Trendorin](https://github.com/Trendorin). Lizenz: [GPL-3.0-or-later](LICENSE).
