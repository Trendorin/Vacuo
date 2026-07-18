#include "vacuo/core/types.hpp"

#include <array>
#include <iomanip>
#include <sstream>

namespace vacuo {

bool CleanReport::allSucceeded() const {
    for (const auto& action : actions) {
        if (!action.success) {
            return false;
        }
    }
    return true;
}

std::string toString(const DistroFamily family) {
    switch (family) {
    case DistroFamily::Arch:
        return "Arch";
    case DistroFamily::Debian:
        return "Debian";
    case DistroFamily::Fedora:
        return "Fedora";
    case DistroFamily::OpenSuse:
        return "openSUSE";
    case DistroFamily::Alpine:
        return "Alpine";
    case DistroFamily::Nix:
        return "Nix";
    case DistroFamily::Unknown:
        return "Unknown";
    }
    return "Unknown";
}

std::string toString(const RiskLevel risk) {
    switch (risk) {
    case RiskLevel::Low:
        return "Low";
    case RiskLevel::Moderate:
        return "Moderate";
    case RiskLevel::Elevated:
        return "Elevated";
    }
    return "Unknown";
}

std::string toString(const RuleScope scope) {
    return scope == RuleScope::User ? "User" : "System";
}

std::string formatBytes(const std::uint64_t bytes) {
    static constexpr std::array<const char*, 5> units{"B", "KiB", "MiB", "GiB", "TiB"};
    auto value = static_cast<double>(bytes);
    std::size_t unit = 0;
    while (value >= 1024.0 && unit + 1 < units.size()) {
        value /= 1024.0;
        ++unit;
    }

    std::ostringstream output;
    output << std::fixed << std::setprecision(unit == 0 ? 0 : 1) << value << ' ' << units[unit];
    return output.str();
}

} // namespace vacuo
