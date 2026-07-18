<p align="center">
  <img src="docs/assets/hero.svg" width="100%" alt="Vacuo — контрольоване очищення кешу Linux">
</p>

<p align="center">
  <a href="README.md"><img alt="English" src="https://img.shields.io/badge/EN-English-d7dade?style=flat-square&labelColor=15181b"></a>
  <a href="README.ru.md"><img alt="Русский" src="https://img.shields.io/badge/RU-Русский-d7dade?style=flat-square&labelColor=15181b"></a>
  <a href="README.uk.md"><img alt="Українська" src="https://img.shields.io/badge/UK-Українська-d7dade?style=flat-square&labelColor=15181b"></a>
  <a href="README.de.md"><img alt="Deutsch" src="https://img.shields.io/badge/DE-Deutsch-d7dade?style=flat-square&labelColor=15181b"></a>
</p>

<p align="center">
  <a href="https://github.com/Trendorin/Vacuo/actions/workflows/ci.yml"><img alt="CI" src="https://img.shields.io/github/actions/workflow/status/Trendorin/Vacuo/ci.yml?branch=main&style=flat-square&label=build&labelColor=15181b&color=5d666d"></a>
  <a href="https://github.com/Trendorin/Vacuo/releases/latest"><img alt="Реліз" src="https://img.shields.io/github/v/release/Trendorin/Vacuo?style=flat-square&label=release&labelColor=15181b&color=5d666d"></a>
  <img alt="C++20" src="https://img.shields.io/badge/C%2B%2B-20-5d666d?style=flat-square&labelColor=15181b">
  <img alt="Qt 6 Widgets" src="https://img.shields.io/badge/Qt-6_Widgets-5d666d?style=flat-square&labelColor=15181b">
  <a href="LICENSE"><img alt="GPL-3.0-or-later" src="https://img.shields.io/badge/license-GPL--3.0--or--later-5d666d?style=flat-square&labelColor=15181b"></a>
</p>

<p align="center">
  <a href="#install">Встановлення</a> ·
  <a href="#build-from-source">Вихідний код</a> ·
  <a href="#uninstall">Видалення</a> ·
  <a href="docs/SECURITY_MODEL.md">Безпека</a> ·
  <a href="https://github.com/Trendorin/Vacuo/releases">Релізи</a>
</p>

Vacuo — нативна Linux-утиліта, що вимірює відомі каталоги кешу, показує точний план і очищає лише вибрані категорії. Основа написана на C++20, інтерфейс — на Qt 6 Widgets і використовує активну системну тему. Без Electron, телеметрії, облікового запису, хмари та фонового демона.

## Що робить Vacuo

| Область | Що входить | Типово |
|---|---|:---:|
| Відновлювані дані | Кеш застосунків і мініатюри | Увімк. |
| Додаткові категорії | Кеш браузерів, інструментів розробки, Flatpak і Steam | Вимк. |
| Незворотне видалення | Файли та метадані Смітника | Вимк. |
| Системні дані | Кеш пакетів, архівний journal, coredump і кеш Snap | Вимк. + PolicyKit |

Профілі браузерів, cookies, історія, паролі, налаштування застосунків, проєкти, ігри та збереження не є цілями. На невідомому дистрибутиві доступні лише консервативні користувацькі XDG-правила — Vacuo не вгадує команду пакетного менеджера.

### Підтримувані системи

| Сімейство | Очищення кешу пакетів |
|---|---|
| Fedora / RHEL | `dnf5 clean all` або `dnf clean all` |
| Arch / CachyOS / Manjaro | `paccache -rk2` |
| Debian / Ubuntu | `apt-get clean` |
| openSUSE / SLES | `zypper --non-interactive clean --all` |
| Alpine | `apk cache clean` |

У KDE Plasma, GNOME, Xfce, Cinnamon, LXQt, Wayland і X11 оформлення вікна, палітру та стиль Qt задає система.

<a id="install"></a>
## Встановлення

Завантажте відповідний файл з [останнього релізу](https://github.com/Trendorin/Vacuo/releases/latest).

| Система | Файл | Команда |
|---|---|---|
| Fedora / RHEL | `vacuo-*.rpm` | `sudo dnf install ./vacuo-*.rpm` |
| Debian / Ubuntu | `vacuo_*.deb` | `sudo apt install ./vacuo_*.deb` |
| Arch-based | `PKGBUILD` | `makepkg -si` |

На Arch встановіть `pacman-contrib`, щоб увімкнути безпечне очищення кешу пакетів. Fallback на `pacman -Scc` навмисно відсутній.

Перевірка завантажених файлів:

```bash
sha256sum --ignore-missing --check SHA256SUMS
```

<a id="build-from-source"></a>
## Збірка з вихідного коду

Потрібні CMake 3.24+, компілятор C++20, Ninja або Make, Qt 6.4+ (`Widgets`, `Concurrent`) і PolicyKit для системних дій.

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

Helper має залишатися звичайним root-owned файлом із правами `0755`. Не додавайте setuid або setgid.

<a id="uninstall"></a>
## Видалення

| Спосіб встановлення | Команда |
|---|---|
| Fedora / RHEL | `sudo dnf remove vacuo` |
| Debian / Ubuntu | `sudo apt remove vacuo` |
| Arch-based | `sudo pacman -Rns vacuo` |
| Збірка з вихідного коду | `sudo xargs -r -d '\n' rm -- < build/install_manifest.txt` |

За потреби видаліть збережену геометрію вікна: `rm -f "$HOME/.config/Trendorin/Vacuo.conf"`.

## Модель безпеки

- Сканування працює лише на читання; очищення потребує явного вибору та підтвердження.
- GUI і CLI відмовляються очищати дані під час запуску від root.
- Користувацькі цілі задані точним allow-list; довільні шляхи не приймаються.
- Symlink-компоненти та захищені корені блокуються до descriptor-relative видалення.
- PolicyKit helper не є setuid, приймає лише чотири фіксовані системні дії й не виконує передану користувачем shell-команду.

Докладніше: [модель безпеки](docs/SECURITY_MODEL.md), [обробка даних](docs/PRIVACY.md), [повідомлення про вразливості](SECURITY.md).

## CLI

```bash
vacuoctl system
vacuoctl list
vacuoctl scan --json
vacuoctl clean --category thumbnails --category application-cache --yes
```

Неявної команди `clean all` навмисно немає.

## Проєкт

[Зміни](CHANGELOG.md) · [Участь](CONTRIBUTING.md) · [Автори](CONTRIBUTORS.md) · [Підтримка](SUPPORT.md)

Проєкт підтримує [Trendorin](https://github.com/Trendorin). Ліцензія: [GPL-3.0-or-later](LICENSE).
