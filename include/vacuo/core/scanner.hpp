#pragma once

#include "vacuo/core/types.hpp"

#include <atomic>
#include <cstddef>
#include <vector>

namespace vacuo {

class Scanner final {
public:
    explicit Scanner(std::size_t entryLimit = 2'000'000);

    [[nodiscard]] ScanReport scan(const SystemInfo& system,
                                  const std::vector<CleanRule>& rules,
                                  const std::atomic_bool* cancelled = nullptr) const;

    [[nodiscard]] ScanStats scanTarget(const PathTarget& target,
                                       const std::atomic_bool* cancelled = nullptr) const;

private:
    std::size_t entryLimit_;
};

} // namespace vacuo
