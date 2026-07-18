#include "vacuo/core/helper_actions.hpp"

#include "vacuo/core/process_runner.hpp"
#include "vacuo/core/safe_delete.hpp"
#include "vacuo/core/scanner.hpp"

#include <algorithm>
#include <array>
#include <filesystem>
#include <unistd.h>

namespace vacuo {
namespace {

std::optional<fs::path> firstExecutable(const std::initializer_list<fs::path> candidates) {
    for (const auto& candidate : candidates) {
        if (::access(candidate.c_str(), X_OK) == 0) {
            return candidate;
        }
    }
    return std::nullopt;
}

std::vector<PathTarget> targetsFor(const std::string_view action, const SystemInfo& system) {
    if (action == "crash-dumps") {
        return {{"/var/lib/systemd/coredump", {}}};
    }
    if (action == "snap-cache") {
        return {{"/var/lib/snapd/cache", {}}};
    }
    if (action == "system-journal") {
        return {{"/var/log/journal", {}}};
    }
    if (action != "package-cache") {
        return {};
    }
    switch (system.family) {
    case DistroFamily::Arch:
        return {{"/var/cache/pacman/pkg", {}}};
    case DistroFamily::Debian:
        return {{"/var/cache/apt/archives", {}}};
    case DistroFamily::Fedora:
        return {{"/var/cache/dnf", {}}, {"/var/cache/libdnf5", {}}, {"/var/cache/yum", {}}};
    case DistroFamily::OpenSuse:
        return {{"/var/cache/zypp", {}}, {"/var/cache/zypper", {}}};
    case DistroFamily::Alpine:
        return {{"/var/cache/apk", {}}};
    case DistroFamily::Nix:
    case DistroFamily::Unknown:
        return {};
    }
    return {};
}

ScanStats measure(const std::vector<PathTarget>& targets) {
    Scanner scanner;
    ScanStats total;
    for (const auto& target : targets) {
        const auto stats = scanner.scanTarget(target);
        total.bytes += stats.bytes;
        total.items += stats.items;
        total.inaccessible += stats.inaccessible;
        total.truncated = total.truncated || stats.truncated;
    }
    return total;
}

ActionResult clearFixedDirectory(const std::string& id, const fs::path& path) {
    ActionResult result;
    result.ruleId = id;
    std::error_code error;
    if (!fs::exists(path, error)) {
        result.success = true;
        result.message = "Directory is already empty or not installed.";
        return result;
    }
    PathGuard guard("/", {path});
    const auto deleted = SafeDelete::clearContents({path, {}}, guard);
    result.success = deleted.success;
    result.bytesFreed = deleted.bytes;
    result.itemsRemoved = deleted.items;
    result.message = deleted.message;
    return result;
}

} // namespace

bool HelperActions::isKnown(const std::string_view action) {
    static constexpr std::array<std::string_view, 4> actions{
        "package-cache", "system-journal", "crash-dumps", "snap-cache"};
    return std::find(actions.begin(), actions.end(), action) != actions.end();
}

std::vector<std::string> HelperActions::knownActions() {
    return {"package-cache", "system-journal", "crash-dumps", "snap-cache"};
}

std::optional<FixedCommand> HelperActions::packageCacheCommand(const SystemInfo& system) {
    switch (system.family) {
    case DistroFamily::Arch:
        if (const auto command = firstExecutable({"/usr/bin/paccache"})) {
            return FixedCommand{*command, {"-rk2"}};
        }
        break;
    case DistroFamily::Debian:
        if (const auto command = firstExecutable({"/usr/bin/apt-get"})) {
            return FixedCommand{*command, {"clean"}};
        }
        break;
    case DistroFamily::Fedora:
        if (const auto command = firstExecutable({"/usr/bin/dnf5", "/usr/bin/dnf"})) {
            return FixedCommand{*command, {"clean", "all"}};
        }
        break;
    case DistroFamily::OpenSuse:
        if (const auto command = firstExecutable({"/usr/bin/zypper"})) {
            return FixedCommand{*command, {"--non-interactive", "clean", "--all"}};
        }
        break;
    case DistroFamily::Alpine:
        if (const auto command = firstExecutable({"/sbin/apk", "/usr/sbin/apk"})) {
            return FixedCommand{*command, {"cache", "clean"}};
        }
        break;
    case DistroFamily::Nix:
    case DistroFamily::Unknown:
        break;
    }
    return std::nullopt;
}

ActionResult HelperActions::execute(const std::string_view action, const SystemInfo& system) {
    ActionResult result;
    result.ruleId = std::string(action);
    if (!isKnown(action)) {
        result.message = "Unknown privileged action.";
        return result;
    }
    if (action == "crash-dumps") {
        return clearFixedDirectory(result.ruleId, "/var/lib/systemd/coredump");
    }
    if (action == "snap-cache") {
        return clearFixedDirectory(result.ruleId, "/var/lib/snapd/cache");
    }

    std::optional<FixedCommand> command;
    if (action == "package-cache") {
        command = packageCacheCommand(system);
        if (!command) {
            result.message = system.family == DistroFamily::Arch
                ? "paccache is unavailable; install pacman-contrib. Vacuo will not fall back to pacman -Scc."
                : "No allow-listed package-cache command is available for this distribution.";
            return result;
        }
    } else if (action == "system-journal") {
        if (const auto journalctl = firstExecutable({"/usr/bin/journalctl", "/bin/journalctl"})) {
            command = FixedCommand{*journalctl, {"--vacuum-time=14d"}};
        } else {
            result.message = "journalctl is not installed.";
            return result;
        }
    }

    const auto targets = targetsFor(action, system);
    const auto before = measure(targets);
    const auto process = ProcessRunner::run(command->executable, command->arguments);
    const auto after = measure(targets);
    result.success = process.exitCode == 0 && !process.timedOut;
    result.bytesFreed = before.bytes > after.bytes ? before.bytes - after.bytes : 0;
    result.itemsRemoved = before.items > after.items ? before.items - after.items : 0;
    if (process.timedOut) {
        result.message = "Allow-listed cleanup command timed out.";
    } else if (result.success) {
        result.message = process.output.empty() ? "Cleanup completed." : process.output;
    } else {
        result.message = process.error.empty() ? process.output : process.error;
    }
    return result;
}

} // namespace vacuo
