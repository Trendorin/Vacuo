#include "vacuo/core/rule_catalog.hpp"

#include <algorithm>
#include <system_error>

namespace vacuo {
namespace {

PathTarget target(const fs::path& path, std::vector<fs::path> exclusions = {}) {
    PathTarget result;
    result.path = path.lexically_normal();
    result.exclusions.reserve(exclusions.size());
    for (auto& exclusion : exclusions) {
        result.exclusions.push_back(exclusion.lexically_normal());
    }
    return result;
}

CleanRule userRule(std::string id,
                   std::string title,
                   std::string description,
                   std::string warning,
                   const RiskLevel risk,
                   const bool selected,
                   std::vector<PathTarget> targets) {
    return CleanRule{std::move(id),
                     std::move(title),
                     std::move(description),
                     std::move(warning),
                     RuleScope::User,
                     risk,
                     selected,
                     std::move(targets),
                     {}};
}

CleanRule systemRule(std::string id,
                     std::string title,
                     std::string description,
                     std::string warning,
                     std::string helperAction,
                     std::vector<PathTarget> targets) {
    return CleanRule{std::move(id),
                     std::move(title),
                     std::move(description),
                     std::move(warning),
                     RuleScope::System,
                     RiskLevel::Elevated,
                     false,
                     std::move(targets),
                     std::move(helperAction)};
}

std::vector<PathTarget> flatpakTargets(const fs::path& home) {
    std::vector<PathTarget> result;
    const auto applications = home / ".var/app";
    std::error_code error;
    fs::directory_iterator iterator(applications, fs::directory_options::skip_permission_denied, error);
    for (; !error && iterator != fs::directory_iterator(); iterator.increment(error)) {
        const auto status = iterator->symlink_status(error);
        if (error) {
            error.clear();
            continue;
        }
        if (!fs::is_directory(status) || fs::is_symlink(status)) {
            continue;
        }
        result.push_back(target(iterator->path() / "cache"));
    }
    return result;
}

} // namespace

std::vector<CleanRule> RuleCatalog::build(const SystemInfo& system) {
    const auto& home = system.home;
    const auto& cache = system.cacheHome;
    const auto& data = system.dataHome;

    const std::vector<fs::path> browserPaths{
        cache / "mozilla/firefox",
        cache / "chromium",
        cache / "google-chrome",
        cache / "BraveSoftware/Brave-Browser",
        cache / "vivaldi",
        cache / "opera",
        cache / "microsoft-edge",
    };
    const std::vector<fs::path> developerPaths{
        cache / "pip",
        cache / "uv",
        cache / "pypoetry",
        cache / "go-build",
        cache / "yarn",
        cache / "JetBrains",
        home / ".npm/_cacache",
        home / ".gradle/caches",
        home / ".cargo/registry/cache",
    };
    const std::vector<fs::path> gamingPaths{
        data / "Steam/steamapps/shadercache",
        data / "Steam/config/htmlcache",
        home / ".steam/steam/steamapps/shadercache",
        home / ".steam/steam/config/htmlcache",
    };

    std::vector<fs::path> genericExclusions{
        cache / "thumbnails",
    };
    genericExclusions.insert(genericExclusions.end(), browserPaths.begin(), browserPaths.end());
    genericExclusions.insert(genericExclusions.end(), developerPaths.begin(), developerPaths.end());
    for (const auto& path : gamingPaths) {
        if (path.native().rfind(cache.native(), 0) == 0) {
            genericExclusions.push_back(path);
        }
    }

    std::vector<PathTarget> browserTargets;
    std::transform(browserPaths.begin(), browserPaths.end(), std::back_inserter(browserTargets),
                   [](const fs::path& path) { return target(path); });
    std::vector<PathTarget> developerTargets;
    std::transform(developerPaths.begin(), developerPaths.end(), std::back_inserter(developerTargets),
                   [](const fs::path& path) { return target(path); });
    std::vector<PathTarget> gamingTargets;
    std::transform(gamingPaths.begin(), gamingPaths.end(), std::back_inserter(gamingTargets),
                   [](const fs::path& path) { return target(path); });

    std::vector<CleanRule> rules;
    rules.push_back(userRule(
        "application-cache",
        "Application cache",
        "General XDG cache data, excluding the separately listed browser, developer and thumbnail groups.",
        "Applications may start more slowly once while their caches are rebuilt.",
        RiskLevel::Low,
        true,
        {target(cache, std::move(genericExclusions))}));
    rules.push_back(userRule(
        "thumbnails",
        "Thumbnail cache",
        "File-manager and desktop thumbnail previews.",
        "Previews are recreated automatically when folders are opened.",
        RiskLevel::Low,
        true,
        {target(cache / "thumbnails")}));
    rules.push_back(userRule(
        "browser-cache",
        "Browser cache",
        "Rebuildable HTTP and media caches for supported Firefox- and Chromium-based browsers.",
        "Close browsers first. This does not remove profiles, history, cookies, passwords or extensions.",
        RiskLevel::Moderate,
        false,
        std::move(browserTargets)));
    rules.push_back(userRule(
        "developer-cache",
        "Developer cache",
        "Package downloads and build caches from npm, Gradle, Cargo, pip, uv, Go and supported IDEs.",
        "The next dependency install or build can take longer and may download data again.",
        RiskLevel::Moderate,
        false,
        std::move(developerTargets)));
    rules.push_back(userRule(
        "flatpak-cache",
        "Flatpak application cache",
        "Only each application's cache directory under ~/.var/app; application data and settings stay intact.",
        "Close Flatpak applications before cleaning their caches.",
        RiskLevel::Moderate,
        false,
        flatpakTargets(home)));
    rules.push_back(userRule(
        "gaming-cache",
        "Steam shader and web cache",
        "Rebuildable Steam shader cache and embedded web-interface cache.",
        "Shaders can be compiled again and a game may stutter on its next launch.",
        RiskLevel::Moderate,
        false,
        std::move(gamingTargets)));
    rules.push_back(userRule(
        "trash",
        "Trash",
        "Freedesktop Trash files and their metadata.",
        "This permanently removes every item currently in Trash.",
        RiskLevel::Elevated,
        false,
        {target(data / "Trash/files"), target(data / "Trash/info")}));

    switch (system.family) {
    case DistroFamily::Arch:
        rules.push_back(systemRule(
            "package-cache", "Package cache", "Cached pacman packages; two installed versions are retained.",
            "Uses paccache through the restricted Vacuo helper.", "package-cache",
            {target("/var/cache/pacman/pkg")}));
        break;
    case DistroFamily::Debian:
        rules.push_back(systemRule(
            "package-cache", "Package cache", "Downloaded APT package archives.",
            "Uses apt-get clean through the restricted Vacuo helper.", "package-cache",
            {target("/var/cache/apt/archives")}));
        break;
    case DistroFamily::Fedora:
        rules.push_back(systemRule(
            "package-cache", "Package cache", "DNF metadata and downloaded packages.",
            "Uses dnf/dnf5 clean all through the restricted Vacuo helper.", "package-cache",
            {target("/var/cache/dnf"), target("/var/cache/libdnf5"), target("/var/cache/yum")}));
        break;
    case DistroFamily::OpenSuse:
        rules.push_back(systemRule(
            "package-cache", "Package cache", "zypper repository metadata and downloaded packages.",
            "Uses zypper clean --all through the restricted Vacuo helper.", "package-cache",
            {target("/var/cache/zypp"), target("/var/cache/zypper")}));
        break;
    case DistroFamily::Alpine:
        rules.push_back(systemRule(
            "package-cache", "Package cache", "Downloaded APK packages.",
            "Uses apk cache clean through the restricted Vacuo helper.", "package-cache",
            {target("/var/cache/apk")}));
        break;
    case DistroFamily::Nix:
    case DistroFamily::Unknown:
        break;
    }

    rules.push_back(systemRule(
        "system-journal",
        "Archived system journal",
        "Archived systemd journal entries older than 14 days.",
        "Active journal files are retained; journalctl decides the exact eligible set.",
        "system-journal",
        {target("/var/log/journal")}));
    rules.push_back(systemRule(
        "crash-dumps",
        "System crash dumps",
        "Stored systemd-coredump files.",
        "Crash dumps used for debugging cannot be restored after cleaning.",
        "crash-dumps",
        {target("/var/lib/systemd/coredump")}));
    rules.push_back(systemRule(
        "snap-cache",
        "Snap download cache",
        "Downloaded Snap package cache. Installed snaps and revisions are not touched.",
        "The same packages may need to be downloaded again.",
        "snap-cache",
        {target("/var/lib/snapd/cache")}));
    return rules;
}

const CleanRule* RuleCatalog::find(const std::vector<CleanRule>& rules, const std::string& id) {
    const auto found = std::find_if(rules.begin(), rules.end(), [&id](const CleanRule& rule) {
        return rule.id == id;
    });
    return found == rules.end() ? nullptr : &*found;
}

} // namespace vacuo
