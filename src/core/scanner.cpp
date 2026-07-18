#include "vacuo/core/scanner.hpp"

#include <algorithm>
#include <cerrno>
#include <filesystem>
#include <sys/stat.h>
#include <system_error>

namespace vacuo {
namespace {

bool isWithin(const fs::path& parent, const fs::path& child) {
    const auto normalizedParent = parent.lexically_normal();
    const auto normalizedChild = child.lexically_normal();
    auto parentPart = normalizedParent.begin();
    auto childPart = normalizedChild.begin();
    while (parentPart != normalizedParent.end()) {
        if (childPart == normalizedChild.end() || *parentPart != *childPart) {
            return false;
        }
        ++parentPart;
        ++childPart;
    }
    return true;
}

bool excluded(const fs::path& path, const std::vector<fs::path>& exclusions) {
    return std::any_of(exclusions.begin(), exclusions.end(), [&path](const fs::path& exclusion) {
        return isWithin(exclusion, path);
    });
}

std::uint64_t allocatedBytes(const fs::path& path, bool* accessible) {
    struct stat status {};
    if (::lstat(path.c_str(), &status) != 0) {
        *accessible = false;
        return 0;
    }
    *accessible = true;
    return static_cast<std::uint64_t>(status.st_blocks) * 512ULL;
}

bool usableDirectory(const fs::path& path) {
    struct stat status {};
    return ::lstat(path.c_str(), &status) == 0 && S_ISDIR(status.st_mode) != 0;
}

} // namespace

Scanner::Scanner(const std::size_t entryLimit) : entryLimit_(entryLimit) {}

ScanStats Scanner::scanTarget(const PathTarget& target, const std::atomic_bool* cancelled) const {
    ScanStats result;
    if (!usableDirectory(target.path)) {
        return result;
    }

    std::error_code error;
    fs::recursive_directory_iterator iterator(target.path,
                                              fs::directory_options::skip_permission_denied,
                                              error);
    const fs::recursive_directory_iterator end;
    while (iterator != end) {
        if (cancelled != nullptr && cancelled->load()) {
            result.truncated = true;
            break;
        }
        if (result.items >= entryLimit_) {
            result.truncated = true;
            break;
        }
        if (error) {
            ++result.inaccessible;
            error.clear();
            iterator.increment(error);
            continue;
        }

        const auto path = iterator->path().lexically_normal();
        if (excluded(path, target.exclusions)) {
            if (iterator->is_directory(error) && !error) {
                iterator.disable_recursion_pending();
            }
            error.clear();
            iterator.increment(error);
            continue;
        }

        bool accessible = false;
        result.bytes += allocatedBytes(path, &accessible);
        if (accessible) {
            ++result.items;
        } else {
            ++result.inaccessible;
        }

        const auto status = iterator->symlink_status(error);
        if (!error && fs::is_symlink(status)) {
            iterator.disable_recursion_pending();
        }
        error.clear();
        iterator.increment(error);
    }
    if (error) {
        ++result.inaccessible;
    }
    return result;
}

ScanReport Scanner::scan(const SystemInfo& system,
                         const std::vector<CleanRule>& rules,
                         const std::atomic_bool* cancelled) const {
    ScanReport report;
    report.system = system;
    report.results.reserve(rules.size());

    for (const auto& rule : rules) {
        if (cancelled != nullptr && cancelled->load()) {
            break;
        }
        ScanResult result;
        result.rule = rule;
        for (const auto& target : rule.targets) {
            if (usableDirectory(target.path)) {
                result.available = true;
            }
            const auto stats = scanTarget(target, cancelled);
            result.stats.bytes += stats.bytes;
            result.stats.items += stats.items;
            result.stats.inaccessible += stats.inaccessible;
            result.stats.truncated = result.stats.truncated || stats.truncated;
        }
        if (result.stats.truncated) {
            result.note = "Scan limit reached; displayed size is a lower bound.";
        } else if (result.stats.inaccessible > 0) {
            result.note = "Some entries could not be read.";
        } else if (!result.available) {
            result.note = "Not present on this system.";
        }
        report.totalBytes += result.stats.bytes;
        report.totalItems += result.stats.items;
        report.results.push_back(std::move(result));
    }
    return report;
}

} // namespace vacuo
