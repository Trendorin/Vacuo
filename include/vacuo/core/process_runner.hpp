#pragma once

#include <chrono>
#include <filesystem>
#include <string>
#include <vector>

namespace vacuo {

struct ProcessResult {
    int exitCode{-1};
    bool timedOut{false};
    std::string output;
    std::string error;
};

class ProcessRunner final {
public:
    [[nodiscard]] static ProcessResult run(const std::filesystem::path& executable,
                                           const std::vector<std::string>& arguments,
                                           std::chrono::seconds timeout = std::chrono::seconds{300});
};

} // namespace vacuo
