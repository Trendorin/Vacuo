#pragma once

#include "vacuo/core/types.hpp"

#include <string>
#include <string_view>

namespace vacuo {

[[nodiscard]] std::string jsonEscape(std::string_view value);
[[nodiscard]] std::string toJson(const ScanReport& report);
[[nodiscard]] std::string toJson(const CleanReport& report);

} // namespace vacuo
