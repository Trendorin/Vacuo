#include "vacuo/core/system_detector.hpp"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <fstream>
#include <pwd.h>
#include <sstream>
#include <string>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

namespace vacuo {
namespace {

std::string trim(std::string value) {
    const auto first = value.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) {
        return {};
    }
    const auto last = value.find_last_not_of(" \t\r\n");
    return value.substr(first, last - first + 1);
}

std::string lower(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(), [](const unsigned char character) {
        return static_cast<char>(std::tolower(character));
    });
    return value;
}

std::string unquote(std::string value) {
    value = trim(std::move(value));
    if (value.size() < 2 || (value.front() != '"' && value.front() != '\'')) {
        return value;
    }
    const char quote = value.front();
    if (value.back() != quote) {
        return value;
    }

    std::string result;
    result.reserve(value.size() - 2);
    bool escaped = false;
    for (std::size_t index = 1; index + 1 < value.size(); ++index) {
        const char character = value[index];
        if (escaped) {
            if (character == quote || character == '\\' || character == '$' || character == '`') {
                result.push_back(character);
            } else {
                result.push_back('\\');
                result.push_back(character);
            }
            escaped = false;
        } else if (character == '\\' && quote == '"') {
            escaped = true;
        } else {
            result.push_back(character);
        }
    }
    if (escaped) {
        result.push_back('\\');
    }
    return result;
}

std::vector<std::string> words(const std::string& input) {
    std::istringstream stream(input);
    std::vector<std::string> result;
    for (std::string word; stream >> word;) {
        result.push_back(lower(std::move(word)));
    }
    return result;
}

bool containsAny(const std::vector<std::string>& values,
                 const std::initializer_list<std::string_view> needles) {
    for (const auto& value : values) {
        for (const auto needle : needles) {
            if (value == needle) {
                return true;
            }
        }
    }
    return false;
}

std::string decodeMountField(const std::string& input) {
    std::string result;
    result.reserve(input.size());
    for (std::size_t index = 0; index < input.size(); ++index) {
        if (input[index] == '\\' && index + 3 < input.size()
            && std::isdigit(static_cast<unsigned char>(input[index + 1])) != 0
            && std::isdigit(static_cast<unsigned char>(input[index + 2])) != 0
            && std::isdigit(static_cast<unsigned char>(input[index + 3])) != 0) {
            const int value = (input[index + 1] - '0') * 64
                + (input[index + 2] - '0') * 8 + (input[index + 3] - '0');
            result.push_back(static_cast<char>(value));
            index += 3;
        } else {
            result.push_back(input[index]);
        }
    }
    return result;
}

bool pathContains(const fs::path& parent, const fs::path& child) {
    const auto normalizedParent = parent.lexically_normal();
    const auto normalizedChild = child.lexically_normal();
    auto parentPart = normalizedParent.begin();
    auto childPart = normalizedChild.begin();
    while (parentPart != normalizedParent.end()) {
        if (childPart == normalizedChild.end() || *parentPart != *childPart) {
            return false;
        }
        ++parentPart;
        ++childPart;
    }
    return true;
}

std::string environment(const char* name, const std::string& fallback = {}) {
    const char* value = std::getenv(name);
    return value != nullptr && *value != '\0' ? std::string(value) : fallback;
}

std::string packageManagerFor(const DistroFamily family) {
    switch (family) {
    case DistroFamily::Arch:
        return "pacman";
    case DistroFamily::Debian:
        return "apt";
    case DistroFamily::Fedora:
        return fs::exists("/usr/bin/dnf5") ? "dnf5" : "dnf";
    case DistroFamily::OpenSuse:
        return "zypper";
    case DistroFamily::Alpine:
        return "apk";
    case DistroFamily::Nix:
        return "nix";
    case DistroFamily::Unknown:
        return "unknown";
    }
    return "unknown";
}

} // namespace

std::map<std::string, std::string> SystemDetector::parseOsRelease(const std::string_view content) {
    std::map<std::string, std::string> values;
    std::istringstream stream{std::string(content)};
    for (std::string line; std::getline(stream, line);) {
        line = trim(std::move(line));
        if (line.empty() || line.front() == '#') {
            continue;
        }
        const auto separator = line.find('=');
        if (separator == std::string::npos || separator == 0) {
            continue;
        }
        auto key = trim(line.substr(0, separator));
        if (!std::all_of(key.begin(), key.end(), [](const unsigned char character) {
                return std::isalnum(character) != 0 || character == '_';
            })) {
            continue;
        }
        values[std::move(key)] = unquote(line.substr(separator + 1));
    }
    return values;
}

DistroFamily SystemDetector::classify(const std::string_view id, const std::string_view idLike) {
    auto identifiers = words(std::string(idLike));
    identifiers.push_back(lower(std::string(id)));

    if (containsAny(identifiers, {"arch", "manjaro", "cachyos", "endeavouros", "garuda"})) {
        return DistroFamily::Arch;
    }
    if (containsAny(identifiers, {"debian", "ubuntu", "linuxmint", "pop", "kali", "neon"})) {
        return DistroFamily::Debian;
    }
    if (containsAny(identifiers, {"fedora", "rhel", "centos", "rocky", "almalinux"})) {
        return DistroFamily::Fedora;
    }
    if (containsAny(identifiers, {"suse", "opensuse", "sles"})) {
        return DistroFamily::OpenSuse;
    }
    if (containsAny(identifiers, {"alpine"})) {
        return DistroFamily::Alpine;
    }
    if (containsAny(identifiers, {"nixos", "nix"})) {
        return DistroFamily::Nix;
    }
    return DistroFamily::Unknown;
}

fs::path SystemDetector::resolveHome() {
    if (const passwd* entry = getpwuid(getuid()); entry != nullptr && entry->pw_dir != nullptr) {
        return fs::path(entry->pw_dir).lexically_normal();
    }
    const auto home = environment("HOME");
    return home.empty() ? fs::path{} : fs::path(home).lexically_normal();
}

fs::path SystemDetector::resolveXdgPath(const char* variable,
                                        const fs::path& fallback,
                                        const fs::path& home) {
    const auto raw = environment(variable);
    if (raw.empty()) {
        return (home / fallback).lexically_normal();
    }
    const fs::path value(raw);
    return value.is_absolute() ? value.lexically_normal() : (home / fallback).lexically_normal();
}

std::string SystemDetector::fileSystemForPath(const fs::path& path, const fs::path& mountInfo) {
    std::ifstream input(mountInfo);
    if (!input) {
        return "unknown";
    }

    const auto normalized = path.lexically_normal();
    std::size_t bestLength = 0;
    std::string bestType = "unknown";
    for (std::string line; std::getline(input, line);) {
        std::istringstream stream(line);
        std::vector<std::string> fields;
        for (std::string field; stream >> field;) {
            fields.push_back(std::move(field));
        }
        const auto separator = std::find(fields.begin(), fields.end(), "-");
        if (fields.size() < 7 || separator == fields.end() || separator + 1 == fields.end()) {
            continue;
        }
        const fs::path mountPoint(decodeMountField(fields[4]));
        if (!pathContains(mountPoint, normalized)) {
            continue;
        }
        const auto length = mountPoint.native().size();
        if (length >= bestLength) {
            bestLength = length;
            bestType = *(separator + 1);
        }
    }
    return bestType;
}

SystemInfo SystemDetector::detect() {
    SystemInfo system;
    system.home = resolveHome();
    system.cacheHome = resolveXdgPath("XDG_CACHE_HOME", ".cache", system.home);
    system.dataHome = resolveXdgPath("XDG_DATA_HOME", ".local/share", system.home);

    std::ifstream release("/etc/os-release");
    if (!release) {
        release.open("/usr/lib/os-release");
    }
    std::ostringstream content;
    content << release.rdbuf();
    const auto values = parseOsRelease(content.str());

    const auto valueOr = [&values](const std::string& key, const std::string& fallback) {
        const auto found = values.find(key);
        return found == values.end() || found->second.empty() ? fallback : found->second;
    };

    system.id = lower(valueOr("ID", "unknown"));
    system.idLike = lower(valueOr("ID_LIKE", ""));
    system.prettyName = valueOr("PRETTY_NAME", valueOr("NAME", "Unknown Linux"));
    system.version = valueOr("VERSION_ID", "");
    system.family = classify(system.id, system.idLike);
    system.packageManager = packageManagerFor(system.family);
    system.desktop = environment("XDG_CURRENT_DESKTOP", environment("DESKTOP_SESSION", "Unknown"));
    system.sessionType = lower(environment("XDG_SESSION_TYPE", "unknown"));
    system.homeFileSystem = fileSystemForPath(system.home);
    return system;
}

} // namespace vacuo
