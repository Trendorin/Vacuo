#include "translation_utils.hpp"

#include <QCoreApplication>

namespace {

[[maybe_unused]] constexpr const char* kVacuoDataSources[] = {
    QT_TRANSLATE_NOOP("VacuoData", "Application cache"),
    QT_TRANSLATE_NOOP("VacuoData", "General XDG cache data, excluding the separately listed browser, developer and thumbnail groups."),
    QT_TRANSLATE_NOOP("VacuoData", "Applications may start more slowly once while their caches are rebuilt."),
    QT_TRANSLATE_NOOP("VacuoData", "Thumbnail cache"),
    QT_TRANSLATE_NOOP("VacuoData", "File-manager and desktop thumbnail previews."),
    QT_TRANSLATE_NOOP("VacuoData", "Previews are recreated automatically when folders are opened."),
    QT_TRANSLATE_NOOP("VacuoData", "Browser cache"),
    QT_TRANSLATE_NOOP("VacuoData", "Rebuildable HTTP and media caches for supported Firefox- and Chromium-based browsers."),
    QT_TRANSLATE_NOOP("VacuoData", "Close browsers first. This does not remove profiles, history, cookies, passwords or extensions."),
    QT_TRANSLATE_NOOP("VacuoData", "Developer cache"),
    QT_TRANSLATE_NOOP("VacuoData", "Package downloads and build caches from npm, Gradle, Cargo, pip, uv, Go and supported IDEs."),
    QT_TRANSLATE_NOOP("VacuoData", "The next dependency install or build can take longer and may download data again."),
    QT_TRANSLATE_NOOP("VacuoData", "Flatpak application cache"),
    QT_TRANSLATE_NOOP("VacuoData", "Only each application's cache directory under ~/.var/app; application data and settings stay intact."),
    QT_TRANSLATE_NOOP("VacuoData", "Close Flatpak applications before cleaning their caches."),
    QT_TRANSLATE_NOOP("VacuoData", "Steam shader and web cache"),
    QT_TRANSLATE_NOOP("VacuoData", "Rebuildable Steam shader cache and embedded web-interface cache."),
    QT_TRANSLATE_NOOP("VacuoData", "Shaders can be compiled again and a game may stutter on its next launch."),
    QT_TRANSLATE_NOOP("VacuoData", "Trash"),
    QT_TRANSLATE_NOOP("VacuoData", "Freedesktop Trash files and their metadata."),
    QT_TRANSLATE_NOOP("VacuoData", "This permanently removes every item currently in Trash."),
    QT_TRANSLATE_NOOP("VacuoData", "Package cache"),
    QT_TRANSLATE_NOOP("VacuoData", "Cached pacman packages; two installed versions are retained."),
    QT_TRANSLATE_NOOP("VacuoData", "Uses paccache through the restricted Vacuo helper."),
    QT_TRANSLATE_NOOP("VacuoData", "Downloaded APT package archives."),
    QT_TRANSLATE_NOOP("VacuoData", "Uses apt-get clean through the restricted Vacuo helper."),
    QT_TRANSLATE_NOOP("VacuoData", "DNF metadata and downloaded packages."),
    QT_TRANSLATE_NOOP("VacuoData", "Uses dnf/dnf5 clean all through the restricted Vacuo helper."),
    QT_TRANSLATE_NOOP("VacuoData", "zypper repository metadata and downloaded packages."),
    QT_TRANSLATE_NOOP("VacuoData", "Uses zypper clean --all through the restricted Vacuo helper."),
    QT_TRANSLATE_NOOP("VacuoData", "Downloaded APK packages."),
    QT_TRANSLATE_NOOP("VacuoData", "Uses apk cache clean through the restricted Vacuo helper."),
    QT_TRANSLATE_NOOP("VacuoData", "Archived system journal"),
    QT_TRANSLATE_NOOP("VacuoData", "Archived systemd journal entries older than 14 days."),
    QT_TRANSLATE_NOOP("VacuoData", "Active journal files are retained; journalctl decides the exact eligible set."),
    QT_TRANSLATE_NOOP("VacuoData", "System crash dumps"),
    QT_TRANSLATE_NOOP("VacuoData", "Stored systemd-coredump files."),
    QT_TRANSLATE_NOOP("VacuoData", "Crash dumps used for debugging cannot be restored after cleaning."),
    QT_TRANSLATE_NOOP("VacuoData", "Snap download cache"),
    QT_TRANSLATE_NOOP("VacuoData", "Downloaded Snap package cache. Installed snaps and revisions are not touched."),
    QT_TRANSLATE_NOOP("VacuoData", "The same packages may need to be downloaded again."),
    QT_TRANSLATE_NOOP("VacuoData", "Scan limit reached; displayed size is a lower bound."),
    QT_TRANSLATE_NOOP("VacuoData", "Some entries could not be read."),
    QT_TRANSLATE_NOOP("VacuoData", "Not present on this system."),
    QT_TRANSLATE_NOOP("VacuoData", "Already empty."),
    QT_TRANSLATE_NOOP("VacuoData", "Cleanup completed."),
    QT_TRANSLATE_NOOP("VacuoData", "Unknown cleanup rule."),
    QT_TRANSLATE_NOOP("VacuoData", "Vacuo's installed PolicyKit helper is unavailable."),
    QT_TRANSLATE_NOOP("VacuoData", "PolicyKit helper timed out."),
    QT_TRANSLATE_NOOP("VacuoData", "PolicyKit authorization was cancelled or the helper failed."),
    QT_TRANSLATE_NOOP("VacuoData", "paccache is unavailable; install pacman-contrib. Vacuo will not fall back to pacman -Scc."),
    QT_TRANSLATE_NOOP("VacuoData", "No allow-listed package-cache command is available for this distribution."),
    QT_TRANSLATE_NOOP("VacuoData", "journalctl is not installed."),
};

} // namespace

QString translatedVacuoText(const std::string& source) {
    return QCoreApplication::translate("VacuoData", source.c_str());
}
