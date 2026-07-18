#pragma once

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

namespace vacuo {

namespace fs = std::filesystem;

enum class DistroFamily {
    Arch,
    Debian,
    Fedora,
    OpenSuse,
    Alpine,
    Nix,
    Unknown,
};

enum class RiskLevel {
    Low,
    Moderate,
    Elevated,
};

enum class RuleScope {
    User,
    System,
};

struct SystemInfo {
    std::string id{"unknown"};
    std::string idLike;
    std::string prettyName{"Unknown Linux"};
    std::string version;
    DistroFamily family{DistroFamily::Unknown};
    std::string packageManager;
    std::string desktop{"Unknown"};
    std::string sessionType{"unknown"};
    std::string homeFileSystem{"unknown"};
    fs::path home;
    fs::path cacheHome;
    fs::path dataHome;
};

struct PathTarget {
    fs::path path;
    std::vector<fs::path> exclusions;
};

struct CleanRule {
    std::string id;
    std::string title;
    std::string description;
    std::string warning;
    RuleScope scope{RuleScope::User};
    RiskLevel risk{RiskLevel::Low};
    bool selectedByDefault{false};
    std::vector<PathTarget> targets;
    std::string helperAction;
};

struct ScanStats {
    std::uint64_t bytes{0};
    std::uint64_t items{0};
    std::uint64_t inaccessible{0};
    bool truncated{false};
};

struct ScanResult {
    CleanRule rule;
    ScanStats stats;
    bool available{false};
    std::string note;
};

struct ScanReport {
    SystemInfo system;
    std::vector<ScanResult> results;
    std::uint64_t totalBytes{0};
    std::uint64_t totalItems{0};
};

struct ActionResult {
    std::string ruleId;
    bool success{false};
    std::uint64_t bytesFreed{0};
    std::uint64_t itemsRemoved{0};
    std::string message;
};

struct CleanReport {
    std::vector<ActionResult> actions;
    std::uint64_t bytesFreed{0};
    std::uint64_t itemsRemoved{0};

    [[nodiscard]] bool allSucceeded() const;
};

[[nodiscard]] std::string toString(DistroFamily family);
[[nodiscard]] std::string toString(RiskLevel risk);
[[nodiscard]] std::string toString(RuleScope scope);
[[nodiscard]] std::string formatBytes(std::uint64_t bytes);

} // namespace vacuo
