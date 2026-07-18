#include "vacuo/core/safe_delete.hpp"

#include <algorithm>
#include <cerrno>
#include <cstring>
#include <dirent.h>
#include <fcntl.h>
#include <memory>
#include <sstream>
#include <sys/stat.h>
#include <unistd.h>

namespace vacuo {
namespace {

struct DirectoryCloser {
    void operator()(DIR* directory) const noexcept {
        if (directory != nullptr) {
            (void)::closedir(directory);
        }
    }
};

fs::path normalize(const fs::path& path) {
    return path.lexically_normal();
}

bool isWithin(const fs::path& parent, const fs::path& child) {
    const auto normalizedParent = normalize(parent);
    const auto normalizedChild = normalize(child);
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

bool isExcluded(const fs::path& path, const std::vector<fs::path>& exclusions) {
    return std::any_of(exclusions.begin(), exclusions.end(), [&path](const fs::path& exclusion) {
        return isWithin(exclusion, path);
    });
}

std::string errorText(const char* operation, const fs::path& path) {
    std::ostringstream output;
    output << operation << " failed for " << path << ": " << std::strerror(errno);
    return output.str();
}

bool hasUnsafeSymlinkComponent(const fs::path& home, const fs::path& candidate) {
    if (!isWithin(home, candidate)) {
        return true;
    }
    fs::path current = home;
    auto part = candidate.begin();
    for (const auto& homePart : home) {
        if (part == candidate.end() || *part != homePart) {
            return true;
        }
        ++part;
    }
    for (; part != candidate.end(); ++part) {
        current /= *part;
        struct stat status {};
        if (::lstat(current.c_str(), &status) != 0) {
            return errno != ENOENT;
        }
        if (S_ISLNK(status.st_mode) != 0) {
            return true;
        }
    }
    return false;
}

void merge(DeleteResult& destination, const DeleteResult& source) {
    destination.success = destination.success && source.success;
    destination.bytes += source.bytes;
    destination.items += source.items;
    destination.failures += source.failures;
    if (!source.message.empty()) {
        if (!destination.message.empty()) {
            destination.message += '\n';
        }
        destination.message += source.message;
    }
}

} // namespace

PathGuard::PathGuard(fs::path home, std::vector<fs::path> approvedRoots)
    : home_(normalize(std::move(home))) {
    approvedRoots_.reserve(approvedRoots.size());
    for (auto& root : approvedRoots) {
        approvedRoots_.push_back(normalize(std::move(root)));
    }
}

bool PathGuard::permits(const fs::path& candidate, std::string* reason) const {
    const auto normalized = normalize(candidate);
    const auto deny = [reason](const std::string& message) {
        if (reason != nullptr) {
            *reason = message;
        }
        return false;
    };

    if (normalized.empty() || !normalized.is_absolute()) {
        return deny("Cleanup target is not an absolute path.");
    }
    if (normalized == "/" || normalized == home_ || normalized == home_ / ".config"
        || normalized == home_ / ".local" || normalized == home_ / ".local/share") {
        return deny("Cleanup target is a protected directory.");
    }
    if (!isWithin(home_, normalized)) {
        return deny("User cleanup target is outside the current home directory.");
    }
    if (std::find(approvedRoots_.begin(), approvedRoots_.end(), normalized) == approvedRoots_.end()) {
        return deny("Cleanup target is not in the immutable rule allow-list.");
    }
    if (hasUnsafeSymlinkComponent(home_, normalized)) {
        return deny("Cleanup target contains a symbolic-link path component.");
    }

    struct stat status {};
    if (::lstat(normalized.c_str(), &status) != 0) {
        return deny(errno == ENOENT ? "Cleanup target does not exist." : errorText("lstat", normalized));
    }
    if (S_ISDIR(status.st_mode) == 0 || S_ISLNK(status.st_mode) != 0) {
        return deny("Cleanup target is not a real directory.");
    }
    if (status.st_uid != getuid()) {
        return deny("Cleanup target is not owned by the current user.");
    }
    return true;
}

int PathGuard::openApprovedDirectory(const fs::path& candidate, std::string* reason) const {
    if (!permits(candidate, reason)) {
        return -1;
    }

    const auto normalized = normalize(candidate);
    int descriptor = ::open("/", O_RDONLY | O_DIRECTORY | O_CLOEXEC | O_NOFOLLOW);
    if (descriptor < 0) {
        if (reason != nullptr) {
            *reason = errorText("open", "/");
        }
        return -1;
    }

    for (const auto& component : normalized.relative_path()) {
        const auto name = component.string();
        const int next = ::openat(descriptor,
                                  name.c_str(),
                                  O_RDONLY | O_DIRECTORY | O_CLOEXEC | O_NOFOLLOW);
        ::close(descriptor);
        if (next < 0) {
            if (reason != nullptr) {
                *reason = errorText("openat", normalized);
            }
            return -1;
        }
        descriptor = next;
    }

    struct stat status {};
    if (::fstat(descriptor, &status) != 0 || S_ISDIR(status.st_mode) == 0
        || status.st_uid != getuid()) {
        if (reason != nullptr) {
            *reason = "Opened cleanup boundary failed the final owner/type check.";
        }
        ::close(descriptor);
        return -1;
    }
    return descriptor;
}

DeleteResult SafeDelete::clearDirectoryFd(const int directoryFd,
                                          const fs::path& logicalPath,
                                          const std::vector<fs::path>& exclusions,
                                          const unsigned depth) {
    DeleteResult result;
    result.success = true;
    if (depth > 128) {
        result.success = false;
        result.failures = 1;
        result.message = "Maximum directory depth exceeded at " + logicalPath.string();
        return result;
    }

    const int duplicate = ::dup(directoryFd);
    if (duplicate < 0) {
        result.success = false;
        result.failures = 1;
        result.message = errorText("dup", logicalPath);
        return result;
    }
    DIR* rawDirectory = ::fdopendir(duplicate);
    if (rawDirectory == nullptr) {
        ::close(duplicate);
        result.success = false;
        result.failures = 1;
        result.message = errorText("fdopendir", logicalPath);
        return result;
    }
    const std::unique_ptr<DIR, DirectoryCloser> directory(rawDirectory);

    while (true) {
        errno = 0;
        dirent* entry = ::readdir(directory.get());
        if (entry == nullptr) {
            if (errno != 0) {
                result.success = false;
                ++result.failures;
                result.message += errorText("readdir", logicalPath);
            }
            break;
        }
        const std::string name(entry->d_name);
        if (name == "." || name == "..") {
            continue;
        }
        const auto childPath = normalize(logicalPath / name);
        if (isExcluded(childPath, exclusions)) {
            continue;
        }

        struct stat status {};
        if (::fstatat(directoryFd, name.c_str(), &status, AT_SYMLINK_NOFOLLOW) != 0) {
            result.success = false;
            ++result.failures;
            result.message += errorText("fstatat", childPath) + '\n';
            continue;
        }
        const auto allocated = static_cast<std::uint64_t>(status.st_blocks) * 512ULL;

        if (S_ISDIR(status.st_mode) != 0) {
            const int childFd = ::openat(directoryFd,
                                         name.c_str(),
                                         O_RDONLY | O_DIRECTORY | O_CLOEXEC | O_NOFOLLOW);
            if (childFd < 0) {
                result.success = false;
                ++result.failures;
                result.message += errorText("openat", childPath) + '\n';
                continue;
            }
            const auto nested = clearDirectoryFd(childFd, childPath, exclusions, depth + 1);
            ::close(childFd);
            merge(result, nested);
            if (::unlinkat(directoryFd, name.c_str(), AT_REMOVEDIR) != 0) {
                result.success = false;
                ++result.failures;
                if (errno != ENOTEMPTY) {
                    result.message += errorText("unlinkat", childPath) + '\n';
                }
            } else {
                result.bytes += allocated;
                ++result.items;
            }
        } else if (::unlinkat(directoryFd, name.c_str(), 0) != 0) {
            result.success = false;
            ++result.failures;
            result.message += errorText("unlinkat", childPath) + '\n';
        } else {
            result.bytes += allocated;
            ++result.items;
        }
    }
    if (result.success && result.message.empty()) {
        result.message = "Cleanup completed.";
    }
    return result;
}

DeleteResult SafeDelete::clearContents(const PathTarget& target, const PathGuard& guard) {
    std::string reason;
    const int directoryFd = guard.openApprovedDirectory(target.path, &reason);
    if (directoryFd < 0) {
        return DeleteResult{false, 0, 0, 1, std::move(reason)};
    }
    auto result = clearDirectoryFd(directoryFd, normalize(target.path), target.exclusions, 0);
    ::close(directoryFd);
    return result;
}

} // namespace vacuo
