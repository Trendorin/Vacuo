#pragma once

#include "vacuo/core/types.hpp"

#include <string>
#include <vector>

namespace vacuo {

class Cleaner final {
public:
    Cleaner(SystemInfo system, std::vector<CleanRule> rules);

    [[nodiscard]] CleanReport execute(const std::vector<std::string>& ruleIds) const;
    [[nodiscard]] static bool isRunningAsRoot();

private:
    [[nodiscard]] std::vector<ActionResult> executeSystemActions(
        const std::vector<const CleanRule*>& rules) const;

    SystemInfo system_;
    std::vector<CleanRule> rules_;
};

} // namespace vacuo
