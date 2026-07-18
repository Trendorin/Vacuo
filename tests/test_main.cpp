#include "vacuo/core/cleaner.hpp"
#include "vacuo/core/helper_actions.hpp"
#include "vacuo/core/json.hpp"
#include "vacuo/core/process_runner.hpp"
#include "vacuo/core/rule_catalog.hpp"
#include "vacuo/core/safe_delete.hpp"
#include "vacuo/core/scanner.hpp"
#include "vacuo/core/system_detector.hpp"
#include "vacuo/core/types.hpp"

#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>
#include <unistd.h>
#include <vector>

namespace fs = std::filesystem;

namespace {

class TempDirectory final {
public:
    TempDirectory() {
        std::string pattern = "/tmp/vacuo-tests-XXXXXX";
        std::vector<char> buffer(pattern.begin(), pattern.end());
        buffer.push_back('\0');
        const char* result = ::mkdtemp(buffer.data());
        if (result == nullptr) {
            throw std::runtime_error("mkdtemp failed");
        }
        path_ = result;
    }

    ~TempDirectory() {
        std::error_code error;
        fs::remove_all(path_, error);
    }

    [[nodiscard]] const fs::path& path() const { return path_; }

private:
    fs::path path_;
};

void write(const fs::path& path, const std::string& content) {
    fs::create_directories(path.parent_path());
    std::ofstream output(path, std::ios::binary);
    output << content;
    if (!output) {
        throw std::runtime_error("Failed to create test fixture");
    }
}

void require(const bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void osReleaseParsing() {
    const auto values = vacuo::SystemDetector::parseOsRelease(
        "# comment\nID=fedora\nPRETTY_NAME=\"Fedora Linux 44\"\nID_LIKE='rhel centos'\n"
        "ESCAPED=\"value\\\"quoted\"\nINVALID LINE\n");
    require(values.at("ID") == "fedora", "ID parsing failed");
    require(values.at("PRETTY_NAME") == "Fedora Linux 44", "quoted value parsing failed");
    require(values.at("ID_LIKE") == "rhel centos", "single-quoted value parsing failed");
    require(values.at("ESCAPED") == "value\"quoted", "escaped value parsing failed");
}

void distroClassification() {
    require(vacuo::SystemDetector::classify("cachyos", "arch") == vacuo::DistroFamily::Arch,
            "CachyOS classification failed");
    require(vacuo::SystemDetector::classify("ubuntu", "debian") == vacuo::DistroFamily::Debian,
            "Ubuntu classification failed");
    require(vacuo::SystemDetector::classify("rocky", "rhel centos fedora") == vacuo::DistroFamily::Fedora,
            "Rocky classification failed");
    require(vacuo::SystemDetector::classify("opensuse-tumbleweed", "suse opensuse")
                == vacuo::DistroFamily::OpenSuse,
            "openSUSE classification failed");
    require(vacuo::SystemDetector::classify("something", "") == vacuo::DistroFamily::Unknown,
            "unknown classification failed");
}

void mountInfoParsing() {
    TempDirectory temporary;
    const auto fixture = temporary.path() / "mountinfo";
    write(fixture,
          "30 20 8:1 / / rw,relatime - ext4 /dev/root rw\n"
          "31 30 0:42 / /home/test\\040user rw,nosuid - btrfs /dev/sda2 rw\n"
          "32 31 0:43 / /home/test\\040user/data rw,nosuid - xfs /dev/sdb1 rw\n");
    require(vacuo::SystemDetector::fileSystemForPath("/home/test user/file", fixture) == "btrfs",
            "escaped mount point parsing failed");
    require(vacuo::SystemDetector::fileSystemForPath("/home/test user/data/project", fixture) == "xfs",
            "longest mount point selection failed");
}

void ruleCatalogSafety() {
    vacuo::SystemInfo system;
    system.home = "/home/alice";
    system.cacheHome = "/home/alice/.cache";
    system.dataHome = "/home/alice/.local/share";
    system.family = vacuo::DistroFamily::Unknown;
    const auto unknownRules = vacuo::RuleCatalog::build(system);
    require(vacuo::RuleCatalog::find(unknownRules, "package-cache") == nullptr,
            "Unknown distributions must not receive a package-cache rule");
    require(vacuo::RuleCatalog::find(unknownRules, "application-cache") != nullptr,
            "Safe XDG fallback rule is missing");

    system.family = vacuo::DistroFamily::Fedora;
    const auto fedoraRules = vacuo::RuleCatalog::build(system);
    const auto* package = vacuo::RuleCatalog::find(fedoraRules, "package-cache");
    require(package != nullptr && package->helperAction == "package-cache",
            "Fedora package-cache adapter is missing");
    require(package->scope == vacuo::RuleScope::System, "Package cache must be privileged");
}

void scannerHonorsExclusions() {
    TempDirectory temporary;
    const auto cache = temporary.path() / ".cache";
    write(cache / "remove.bin", std::string(4096, 'x'));
    write(cache / "keep/secret.bin", std::string(8192, 'y'));

    const vacuo::PathTarget target{cache, {cache / "keep"}};
    const auto stats = vacuo::Scanner{}.scanTarget(target);
    require(stats.items == 1, "Scanner entered an excluded subtree");
    require(stats.bytes > 0, "Scanner did not measure an allocated file");
}

void safeDeleteDoesNotFollowLinks() {
    TempDirectory temporary;
    const auto home = temporary.path() / "home";
    const auto cache = home / ".cache";
    const auto outside = temporary.path() / "outside";
    write(cache / "remove/file.bin", "remove");
    write(cache / "keep/file.bin", "keep");
    write(outside / "must-survive.bin", "secret");
    fs::create_directory_symlink(outside, cache / "outside-link");

    const vacuo::PathTarget target{cache, {cache / "keep"}};
    const vacuo::PathGuard guard(home, {cache});
    const auto result = vacuo::SafeDelete::clearContents(target, guard);
    require(result.success, "Safe deletion unexpectedly failed: " + result.message);
    require(fs::exists(cache), "Cleanup boundary itself was deleted");
    require(fs::exists(cache / "keep/file.bin"), "Excluded data was deleted");
    require(!fs::exists(cache / "remove"), "Selected cache was not removed");
    std::error_code linkError;
    const auto linkStatus = fs::symlink_status(cache / "outside-link", linkError);
    require(linkStatus.type() == fs::file_type::not_found,
            "Symbolic link itself was not removed");
    require(fs::exists(outside / "must-survive.bin"), "Deletion followed a symbolic link");
}

void pathGuardRejectsBroadAndSymlinkedTargets() {
    TempDirectory temporary;
    const auto home = temporary.path() / "home";
    const auto cache = home / ".cache";
    fs::create_directories(cache);
    vacuo::PathGuard guard(home, {cache});
    require(!guard.permits(home), "Home directory was accepted as a cleanup target");
    require(!guard.permits(home / ".config"), "Configuration directory was accepted");
    require(!guard.permits(home / "Downloads"), "Non-allow-listed directory was accepted");

    const auto real = home / "real";
    fs::create_directories(real / "cache");
    fs::create_directory_symlink(real, home / "linked");
    vacuo::PathGuard symlinkGuard(home, {home / "linked/cache"});
    require(!symlinkGuard.permits(home / "linked/cache"), "Symlinked path component was accepted");
}

void pathGuardRejectsExternalTarget() {
    TempDirectory temporary;
    const auto home = temporary.path() / "home";
    const auto outside = temporary.path() / "outside";
    fs::create_directories(home);
    fs::create_directories(outside);
    vacuo::PathGuard guard(home, {outside});
    require(!guard.permits(outside), "An allow-listed but out-of-home user target was accepted");
}

void processRunnerUsesAbsoluteExecutables() {
    const auto result = vacuo::ProcessRunner::run("/usr/bin/printf", {"safe-output"});
    require(result.exitCode == 0, "Absolute allow-listed process failed");
    require(result.output == "safe-output", "Process output capture failed");
    const auto rejected = vacuo::ProcessRunner::run("printf", {"unsafe"});
    require(rejected.exitCode == -1 && !rejected.error.empty(), "Relative executable was accepted");
}

void processRunnerEnforcesTimeout() {
    const auto result = vacuo::ProcessRunner::run("/usr/bin/sleep", {"2"}, std::chrono::seconds{0});
    require(result.timedOut, "Process timeout was not enforced");
}

void jsonEncoding() {
    require(vacuo::jsonEscape("a\n\"b\\c") == "a\\n\\\"b\\\\c", "JSON escaping failed");
    vacuo::CleanReport report;
    report.actions.push_back({"test", true, 10, 1, "done"});
    report.bytesFreed = 10;
    report.itemsRemoved = 1;
    const auto json = vacuo::toJson(report);
    require(json.find("\"success\":true") != std::string::npos, "Clean JSON status is missing");
    require(json.find("io.github.trendorin.vacuo.clean/v1") != std::string::npos,
            "Clean JSON schema is missing");
}

void helperAllowList() {
    require(vacuo::HelperActions::isKnown("package-cache"), "Known helper action was rejected");
    require(!vacuo::HelperActions::isKnown("rm-rf"), "Unknown helper action was accepted");
}

void rootCleanerRefusesUserDeletion() {
    if (::geteuid() != 0) {
        return;
    }
    TempDirectory temporary;
    const auto home = temporary.path() / "home";
    const auto cache = home / ".cache";
    write(cache / "must-survive", "data");
    vacuo::SystemInfo system;
    system.home = home;
    system.cacheHome = cache;
    system.dataHome = home / ".local/share";
    const std::vector<vacuo::CleanRule> rules{{
        "test", "Test", "Test", "", vacuo::RuleScope::User, vacuo::RiskLevel::Low,
        false, {{cache, {}}}, ""}};
    const auto report = vacuo::Cleaner(system, rules).execute({"test"});
    require(!report.allSucceeded(), "Cleaner accepted a root session");
    require(fs::exists(cache / "must-survive"), "Root refusal happened after deletion");
}

} // namespace

int main() {
    const std::vector<std::pair<std::string, std::function<void()>>> tests{
        {"os-release parsing", osReleaseParsing},
        {"distribution classification", distroClassification},
        {"mountinfo parsing", mountInfoParsing},
        {"rule catalog safety", ruleCatalogSafety},
        {"scanner exclusions", scannerHonorsExclusions},
        {"symlink-safe deletion", safeDeleteDoesNotFollowLinks},
        {"path guard", pathGuardRejectsBroadAndSymlinkedTargets},
        {"external target rejection", pathGuardRejectsExternalTarget},
        {"process allow-list boundary", processRunnerUsesAbsoluteExecutables},
        {"process timeout", processRunnerEnforcesTimeout},
        {"JSON encoding", jsonEncoding},
        {"helper allow-list", helperAllowList},
        {"root cleaner refusal", rootCleanerRefusesUserDeletion},
    };

    std::size_t failures = 0;
    for (const auto& [name, test] : tests) {
        try {
            test();
            std::cout << "[PASS] " << name << '\n';
        } catch (const std::exception& error) {
            ++failures;
            std::cerr << "[FAIL] " << name << ": " << error.what() << '\n';
        }
    }
    std::cout << tests.size() - failures << '/' << tests.size() << " tests passed\n";
    return failures == 0 ? 0 : 1;
}
