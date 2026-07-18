#include "vacuo/core/cleaner.hpp"
#include "vacuo/core/json.hpp"
#include "vacuo/core/rule_catalog.hpp"
#include "vacuo/core/scanner.hpp"
#include "vacuo/core/system_detector.hpp"
#include "vacuo/core/types.hpp"
#include "vacuo/version.hpp"

#include <algorithm>
#include <iostream>
#include <set>
#include <string>
#include <vector>

namespace {

void usage(std::ostream& output) {
    output << "Vacuo " << vacuo::kVersion << "\n"
           << "Usage:\n"
           << "  vacuoctl system [--json]\n"
           << "  vacuoctl list\n"
           << "  vacuoctl scan [--json]\n"
           << "  vacuoctl clean --category <id> [--category <id> ...] --yes [--json]\n"
           << "  vacuoctl --version\n\n"
           << "Cleaning never follows symbolic links. System categories are delegated to the\n"
           << "installed PolicyKit helper and may show an authentication prompt.\n";
}

bool hasArgument(const std::vector<std::string>& arguments, const std::string& value) {
    return std::find(arguments.begin(), arguments.end(), value) != arguments.end();
}

std::vector<std::string> categoryArguments(const std::vector<std::string>& arguments) {
    std::vector<std::string> categories;
    for (std::size_t index = 0; index + 1 < arguments.size(); ++index) {
        if (arguments[index] == "--category") {
            categories.push_back(arguments[index + 1]);
            ++index;
        }
    }
    return categories;
}

void printSystem(const vacuo::SystemInfo& system) {
    std::cout << "System:          " << system.prettyName << '\n'
              << "Family:          " << vacuo::toString(system.family) << '\n'
              << "Package manager: " << system.packageManager << '\n'
              << "Desktop:         " << system.desktop << '\n'
              << "Session:         " << system.sessionType << '\n'
              << "Home filesystem: " << system.homeFileSystem << '\n';
}

} // namespace

int main(int argc, char* argv[]) {
    std::vector<std::string> arguments;
    for (int index = 1; index < argc; ++index) {
        arguments.emplace_back(argv[index]);
    }
    if (arguments.empty() || hasArgument(arguments, "--help") || hasArgument(arguments, "-h")) {
        usage(std::cout);
        return arguments.empty() ? 2 : 0;
    }
    if (hasArgument(arguments, "--version")) {
        std::cout << "vacuoctl " << vacuo::kVersion << '\n';
        return 0;
    }

    const auto system = vacuo::SystemDetector::detect();
    const auto rules = vacuo::RuleCatalog::build(system);
    const bool json = hasArgument(arguments, "--json");
    const auto& command = arguments.front();

    if (command == "system") {
        if (json) {
            const vacuo::ScanReport minimal{system, {}, 0, 0};
            std::cout << vacuo::toJson(minimal) << '\n';
        } else {
            printSystem(system);
        }
        return 0;
    }
    if (command == "list") {
        for (const auto& rule : rules) {
            std::cout << rule.id << '\t' << vacuo::toString(rule.scope) << '\t'
                      << vacuo::toString(rule.risk) << '\t' << rule.title << '\n';
        }
        return 0;
    }
    if (command == "scan") {
        const auto report = vacuo::Scanner{}.scan(system, rules);
        if (json) {
            std::cout << vacuo::toJson(report) << '\n';
        } else {
            printSystem(system);
            std::cout << "\nCleanup candidates\n";
            for (const auto& result : report.results) {
                std::cout << (result.available ? "  " : "- ") << result.rule.id << ": "
                          << vacuo::formatBytes(result.stats.bytes) << " in " << result.stats.items
                          << " items";
                if (!result.note.empty()) {
                    std::cout << " (" << result.note << ')';
                }
                std::cout << '\n';
            }
            std::cout << "\nTotal: " << vacuo::formatBytes(report.totalBytes) << " in "
                      << report.totalItems << " items\n";
        }
        return 0;
    }
    if (command == "clean") {
        if (!hasArgument(arguments, "--yes")) {
            std::cerr << "Refusing to clean without --yes. Run 'vacuoctl scan' first.\n";
            return 2;
        }
        const auto categories = categoryArguments(arguments);
        if (categories.empty()) {
            std::cerr << "At least one --category <id> is required. There is intentionally no implicit clean-all.\n";
            return 2;
        }
        for (const auto& category : categories) {
            if (vacuo::RuleCatalog::find(rules, category) == nullptr) {
                std::cerr << "Unknown category: " << category << '\n';
                return 2;
            }
        }
        const auto report = vacuo::Cleaner(system, rules).execute(categories);
        if (json) {
            std::cout << vacuo::toJson(report) << '\n';
        } else {
            for (const auto& action : report.actions) {
                std::cout << (action.success ? "OK   " : "FAIL ") << action.ruleId << ": "
                          << action.message << '\n';
            }
            std::cout << "Freed " << vacuo::formatBytes(report.bytesFreed) << " across "
                      << report.itemsRemoved << " entries.\n";
        }
        return report.allSucceeded() ? 0 : 1;
    }

    std::cerr << "Unknown command: " << command << "\n\n";
    usage(std::cerr);
    return 2;
}
