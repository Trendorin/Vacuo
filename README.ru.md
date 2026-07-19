<p align="center">
  <img src="data/icons/hicolor/512x512/apps/io.github.trendorin.Vacuo.png" width="144" alt="Иконка приложения Vacuo">
</p>

<h1 align="center">Vacuo</h1>
<p align="center">Контролируемая очистка кэша Linux.</p>

<p align="center">
  <a href="README.md"><img alt="English" src="https://img.shields.io/badge/EN-English-d7dade?style=flat-square&labelColor=15181b"></a>
  <a href="README.ru.md"><img alt="Русский" src="https://img.shields.io/badge/RU-Русский-d7dade?style=flat-square&labelColor=15181b"></a>
  <a href="README.uk.md"><img alt="Українська" src="https://img.shields.io/badge/UK-Українська-d7dade?style=flat-square&labelColor=15181b"></a>
  <a href="README.de.md"><img alt="Deutsch" src="https://img.shields.io/badge/DE-Deutsch-d7dade?style=flat-square&labelColor=15181b"></a>
</p>

<p align="center">
  <a href="https://github.com/Trendorin/Vacuo/actions/workflows/ci.yml"><img alt="CI" src="https://img.shields.io/github/actions/workflow/status/Trendorin/Vacuo/ci.yml?branch=main&style=flat-square&label=build&labelColor=15181b&color=5d666d"></a>
  <a href="https://github.com/Trendorin/Vacuo/releases/latest"><img alt="Релиз" src="https://img.shields.io/github/v/release/Trendorin/Vacuo?style=flat-square&label=release&labelColor=15181b&color=5d666d"></a>
  <img alt="C++20" src="https://img.shields.io/badge/C%2B%2B-20-5d666d?style=flat-square&labelColor=15181b">
  <img alt="Qt 6 Widgets" src="https://img.shields.io/badge/Qt-6_Widgets-5d666d?style=flat-square&labelColor=15181b">
  <a href="LICENSE"><img alt="GPL-3.0-or-later" src="https://img.shields.io/badge/license-GPL--3.0--or--later-5d666d?style=flat-square&labelColor=15181b"></a>
</p>

<p align="center">
  <a href="#install">Установка</a> ·
  <a href="#build-from-source">Исходники</a> ·
  <a href="#uninstall">Удаление</a> ·
  <a href="docs/SECURITY_MODEL.md">Безопасность</a> ·
  <a href="https://github.com/Trendorin/Vacuo/releases">Релизы</a>
</p>

Vacuo — нативная Linux-утилита, которая измеряет известные каталоги кэша, показывает точный план и очищает только выбранные категории. Основа написана на C++20, интерфейс — на Qt 6 Widgets и использует активную тему системы. Без Electron, телеметрии, аккаунта, облака и фонового демона.

## Что делает Vacuo

| Область | Что входит | По умолчанию |
|---|---|:---:|
| Восстанавливаемые данные | Кэш приложений и миниатюры | Вкл. |
| Дополнительные категории | Кэш браузеров, инструментов разработки, Flatpak и Steam | Выкл. |
| Необратимое удаление | Файлы и метаданные Корзины | Выкл. |
| Системные данные | Кэш пакетов, архивный journal, coredump и кэш Snap | Выкл. + PolicyKit |

Профили браузеров, cookies, история, пароли, настройки приложений, проекты, игры и сохранения не являются целями. На неизвестном дистрибутиве доступны только консервативные пользовательские XDG-правила — Vacuo не угадывает команды пакетного менеджера.

### Поддерживаемые системы

| Семейство | Очистка кэша пакетов |
|---|---|
| Fedora / RHEL | `dnf5 clean all` или `dnf clean all` |
| Arch / CachyOS / Manjaro | `paccache -rk2` |
| Debian / Ubuntu | `apt-get clean` |
| openSUSE / SLES | `zypper --non-interactive clean --all` |
| Alpine | `apk cache clean` |

В KDE Plasma, GNOME, Xfce, Cinnamon, LXQt, Wayland и X11 оформление окна, палитру и стиль Qt задаёт система.

<a id="install"></a>
## Установка

Скачайте подходящий файл из [последнего релиза](https://github.com/Trendorin/Vacuo/releases/latest).

| Система | Файл | Команда |
|---|---|---|
| Fedora 44 | `vacuo-*.rpm` | `sudo dnf install ./vacuo-*.rpm` |
| Ubuntu 24.04 | `vacuo_*.deb` | `sudo apt install ./vacuo_*.deb` |
| Arch Linux | `vacuo-*-x86_64.pkg.tar.zst` | `sudo pacman -U ./vacuo-*-x86_64.pkg.tar.zst` |

Каждый бинарный пакет собирается и проверяется установкой в целевой системе. Для сборки из исходников в релизе остаётся `PKGBUILD`: `makepkg -si`. На Arch пакет `pacman-contrib` включает безопасную очистку кэша пакетов; fallback на `pacman -Scc` отсутствует.

Проверка скачанных файлов:

```bash
sha256sum --ignore-missing --check SHA256SUMS
```

<a id="build-from-source"></a>
## Сборка из исходников

Требования: CMake 3.24+, компилятор C++20, Ninja или Make, Qt 6.4+ (`Widgets`, `Concurrent`) и PolicyKit для системных действий.

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

Helper должен оставаться обычным root-owned файлом с правами `0755`. Не добавляйте setuid или setgid.

<a id="uninstall"></a>
## Удаление

| Способ установки | Команда |
|---|---|
| Fedora 44 | `sudo dnf remove vacuo` |
| Ubuntu 24.04 | `sudo apt remove vacuo` |
| Arch Linux | `sudo pacman -Rns vacuo` |
| Сборка из исходников | `sudo xargs -r -d '\n' rm -- < build/install_manifest.txt` |

При необходимости удалите сохранённую геометрию окна: `rm -f "$HOME/.config/Trendorin/Vacuo.conf"`.

## Модель безопасности

- Сканирование работает только на чтение; очистка требует явного выбора и подтверждения.
- GUI и CLI отказываются очищать данные при запуске от root.
- Пользовательские цели заданы точным allow-list; произвольные пути не принимаются.
- Symlink-компоненты и защищённые корни блокируются до descriptor-relative удаления.
- PolicyKit helper не является setuid, принимает только четыре фиксированных системных действия и не запускает переданную пользователем shell-команду.

Подробнее: [модель безопасности](docs/SECURITY_MODEL.md), [обработка данных](docs/PRIVACY.md), [сообщение об уязвимостях](SECURITY.md).

## CLI

```bash
vacuoctl system
vacuoctl list
vacuoctl scan --json
vacuoctl clean --category thumbnails --category application-cache --yes
```

Неявной команды `clean all` намеренно нет.

## Проект

[Изменения](CHANGELOG.md) · [Участие](CONTRIBUTING.md) · [Авторы](CONTRIBUTORS.md) · [Поддержка](SUPPORT.md)

Проект поддерживает [Trendorin](https://github.com/Trendorin). Лицензия: [GPL-3.0-or-later](LICENSE).
