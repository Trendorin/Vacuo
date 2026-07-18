#include "vacuo/core/cleaner.hpp"

#include "vacuo/core/process_runner.hpp"
#include "vacuo/core/rule_catalog.hpp"
#include "vacuo/core/safe_delete.hpp"

#include <algorithm>
#include <filesystem>
#include <map>
#include <optional>
#include <set>
#include <sstream>
#include <unistd.h>

namespace vacuo {
namespace {

std::optional<fs::path> existingExecutable(const std::initializer_list<fs::path> candidates) {
    for (const auto& candidate : candidates) {
        if (::access(candidate.c_str(), X_OK) == 0) {
            return candidate;
        }
    }
    return std::nullopt;
}

std::vector<std::string> splitTabs(const std::string& line) {
    std::vector<std::string> fields;
    std::size_t start = 0;
    while (start <= line.size()) {
        const auto tab = line.find('\t', start);
        fields.push_back(line.substr(start, tab == std::string::npos ? std::string::npos : tab - start));
        if (tab == std::string::npos) {
            break;
        }
        start = tab + 1;
    }
    return fields;
}

} // namespace

Cleaner::Cleaner(SystemInfo system, std::vector<CleanRule> rules)
    : system_(std::move(system)), rules_(std::move(rules)) {}

bool Cleaner::isRunningAsRoot() {
    return ::geteuid() == 0;
}

CleanReport Cleaner::execute(const std::vector<std::string>& ruleIds) const {
    CleanReport report;
    if (isRunningAsRoot()) {
        report.actions.push_back({"security", false, 0, 0,
                                  "Vacuo and vacuoctl must not run as root. Privileged actions use the helper."});
        return report;
    }

    std::set<std::string> uniqueIds(ruleIds.begin(), ruleIds.end());
    std::vector<const CleanRule*> systemRules;
    std::vector<fs::path> approvedRoots;
    for (const auto& rule : rules_) {
        if (rule.scope != RuleScope::User) {
            continue;
        }
        for (const auto& target : rule.targets) {
            approvedRoots.push_back(target.path);
        }
    }
    const PathGuard guard(system_.home, std::move(approvedRoots));

    for (const auto& id : uniqueIds) {
        const auto* rule = RuleCatalog::find(rules_, id);
        if (rule == nullptr) {
            report.actions.push_back({id, false, 0, 0, "Unknown cleanup rule."});
            continue;
        }
        if (rule->scope == RuleScope::System) {
            systemRules.push_back(rule);
            continue;
        }

        ActionResult action;
        action.ruleId = rule->id;
        action.success = true;
        action.message = "Already empty.";
        bool attempted = false;
        for (const auto& target : rule->targets) {
            std::error_code error;
            if (!fs::exists(target.path, error)) {
                continue;
            }
            attempted = true;
            const auto deleted = SafeDelete::clearContents(target, guard);
            action.success = action.success && deleted.success;
            action.bytesFreed += deleted.bytes;
            action.itemsRemoved += deleted.items;
            if (action.message == "Already empty.") {
                action.message.clear();
            }
            if (!action.message.empty()) {
                action.message += '\n';
            }
            action.message += target.path.string() + ": " + deleted.message;
        }
        if (attempted && action.message.empty()) {
            action.message = "Cleanup completed.";
        }
        report.bytesFreed += action.bytesFreed;
        report.itemsRemoved += action.itemsRemoved;
        report.actions.push_back(std::move(action));
    }

    auto systemResults = executeSystemActions(systemRules);
    for (auto& action : systemResults) {
        report.bytesFreed += action.bytesFreed;
        report.itemsRemoved += action.itemsRemoved;
        report.actions.push_back(std::move(action));
    }
    return report;
}

std::vector<ActionResult> Cleaner::executeSystemActions(
    const std::vector<const CleanRule*>& rules) const {
    if (rules.empty()) {
        return {};
    }

    const auto pkexec = existingExecutable({"/usr/bin/pkexec", "/bin/pkexec"});
    const auto helper = existingExecutable({"/usr/libexec/vacuo-helper",
                                            "/usr/lib/vacuo/vacuo-helper",
                                            "/usr/local/libexec/vacuo-helper"});
    if (!pkexec || !helper) {
        std::vector<ActionResult> failures;
        for (const auto* rule : rules) {
            failures.push_back({rule->id, false, 0, 0,
                                "Vacuo's installed PolicyKit helper is unavailable."});
        }
        return failures;
    }

    std::vector<std::string> arguments{helper->string()};
    std::set<std::string> requestedActions;
    for (const auto* rule : rules) {
        requestedActions.insert(rule->helperAction);
    }
    for (const auto& action : requestedActions) {
        arguments.emplace_back("--action");
        arguments.push_back(action);
    }
    const auto process = ProcessRunner::run(*pkexec, arguments);

    std::map<std::string, ActionResult> parsed;
    std::istringstream lines(process.output);
    for (std::string line; std::getline(lines, line);) {
        const auto fields = splitTabs(line);
        if (fields.size() < 6 || fields[0] != "VACUO_RESULT") {
            continue;
        }
        ActionResult action;
        action.ruleId = fields[1];
        action.success = fields[2] == "0";
        try {
            action.bytesFreed = std::stoull(fields[3]);
            action.itemsRemoved = std::stoull(fields[4]);
        } catch (...) {
            action.success = false;
        }
        action.message = fields[5];
        parsed[action.ruleId] = std::move(action);
    }

    std::vector<ActionResult> results;
    for (const auto* rule : rules) {
        const auto found = parsed.find(rule->helperAction);
        if (found != parsed.end()) {
            auto action = found->second;
            action.ruleId = rule->id;
            results.push_back(std::move(action));
        } else {
            const auto message = process.timedOut
                ? "PolicyKit helper timed out."
                : (process.error.empty() ? "PolicyKit authorization was cancelled or the helper failed."
                                         : process.error);
            results.push_back({rule->id, false, 0, 0, message});
        }
    }
    return results;
}

} // namespace vacuo
