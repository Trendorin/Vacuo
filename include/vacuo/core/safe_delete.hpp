#pragma once

#include "vacuo/core/types.hpp"

#include <filesystem>
#include <string>
#include <vector>

namespace vacuo {

struct DeleteResult {
    bool success{false};
    std::uint64_t bytes{0};
    std::uint64_t items{0};
    std::uint64_t failures{0};
    std::string message;
};

class PathGuard final {
public:
    PathGuard(fs::path home, std::vector<fs::path> approvedRoots);

    [[nodiscard]] bool permits(const fs::path& candidate, std::string* reason = nullptr) const;
    [[nodiscard]] int openApprovedDirectory(const fs::path& candidate,
                                            std::string* reason = nullptr) const;

private:
    fs::path home_;
    std::vector<fs::path> approvedRoots_;
};

class SafeDelete final {
public:
    [[nodiscard]] static DeleteResult clearContents(const PathTarget& target,
                                                    const PathGuard& guard);

private:
    [[nodiscard]] static DeleteResult clearDirectoryFd(int directoryFd,
                                                       const fs::path& logicalPath,
                                                       const std::vector<fs::path>& exclusions,
                                                       unsigned depth);
};

} // namespace vacuo
