#pragma once

#include "vacuo/core/types.hpp"

#include <map>
#include <string>
#include <string_view>

namespace vacuo {

class SystemDetector final {
public:
    [[nodiscard]] static SystemInfo detect();
    [[nodiscard]] static std::map<std::string, std::string> parseOsRelease(std::string_view content);
    [[nodiscard]] static DistroFamily classify(std::string_view id, std::string_view idLike);
    [[nodiscard]] static std::string fileSystemForPath(const fs::path& path,
                                                       const fs::path& mountInfo = "/proc/self/mountinfo");

private:
    [[nodiscard]] static fs::path resolveHome();
    [[nodiscard]] static fs::path resolveXdgPath(const char* variable,
                                                 const fs::path& fallback,
                                                 const fs::path& home);
};

} // namespace vacuo
