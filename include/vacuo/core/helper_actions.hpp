#pragma once

#include "vacuo/core/types.hpp"

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace vacuo {

struct FixedCommand {
    std::filesystem::path executable;
    std::vector<std::string> arguments;
};

class HelperActions final {
public:
    [[nodiscard]] static bool isKnown(std::string_view action);
    [[nodiscard]] static std::vector<std::string> knownActions();
    [[nodiscard]] static std::optional<FixedCommand> packageCacheCommand(const SystemInfo& system);
    [[nodiscard]] static ActionResult execute(std::string_view action, const SystemInfo& system);
};

} // namespace vacuo
