#pragma once

#include "vacuo/core/types.hpp"

#include <vector>

namespace vacuo {

class RuleCatalog final {
public:
    [[nodiscard]] static std::vector<CleanRule> build(const SystemInfo& system);
    [[nodiscard]] static const CleanRule* find(const std::vector<CleanRule>& rules,
                                               const std::string& id);
};

} // namespace vacuo
