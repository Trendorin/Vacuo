#include "vacuo/core/json.hpp"

#include <iomanip>
#include <sstream>

namespace vacuo {

std::string jsonEscape(const std::string_view value) {
    std::ostringstream output;
    for (const char rawCharacter : value) {
        const auto character = static_cast<unsigned char>(rawCharacter);
        switch (character) {
        case '"':
            output << "\\\"";
            break;
        case '\\':
            output << "\\\\";
            break;
        case '\b':
            output << "\\b";
            break;
        case '\f':
            output << "\\f";
            break;
        case '\n':
            output << "\\n";
            break;
        case '\r':
            output << "\\r";
            break;
        case '\t':
            output << "\\t";
            break;
        default:
            if (character < 0x20) {
                output << "\\u" << std::hex << std::setw(4) << std::setfill('0')
                       << static_cast<unsigned>(character) << std::dec;
            } else {
                output << static_cast<char>(character);
            }
        }
    }
    return output.str();
}

std::string toJson(const ScanReport& report) {
    std::ostringstream output;
    output << "{\"schema\":\"io.github.trendorin.vacuo.scan/v1\",\"system\":{"
           << "\"id\":\"" << jsonEscape(report.system.id) << "\","
           << "\"pretty_name\":\"" << jsonEscape(report.system.prettyName) << "\","
           << "\"family\":\"" << jsonEscape(toString(report.system.family)) << "\","
           << "\"package_manager\":\"" << jsonEscape(report.system.packageManager) << "\","
           << "\"desktop\":\"" << jsonEscape(report.system.desktop) << "\","
           << "\"session_type\":\"" << jsonEscape(report.system.sessionType) << "\","
           << "\"home_filesystem\":\"" << jsonEscape(report.system.homeFileSystem) << "\"},"
           << "\"total_bytes\":" << report.totalBytes << ",\"total_items\":" << report.totalItems
           << ",\"categories\":[";
    for (std::size_t index = 0; index < report.results.size(); ++index) {
        const auto& result = report.results[index];
        if (index != 0) {
            output << ',';
        }
        output << "{\"id\":\"" << jsonEscape(result.rule.id) << "\","
               << "\"title\":\"" << jsonEscape(result.rule.title) << "\","
               << "\"scope\":\"" << jsonEscape(toString(result.rule.scope)) << "\","
               << "\"risk\":\"" << jsonEscape(toString(result.rule.risk)) << "\","
               << "\"available\":" << (result.available ? "true" : "false") << ','
               << "\"bytes\":" << result.stats.bytes << ','
               << "\"items\":" << result.stats.items << ','
               << "\"inaccessible\":" << result.stats.inaccessible << ','
               << "\"truncated\":" << (result.stats.truncated ? "true" : "false") << ','
               << "\"note\":\"" << jsonEscape(result.note) << "\"}";
    }
    output << "]}";
    return output.str();
}

std::string toJson(const CleanReport& report) {
    std::ostringstream output;
    output << "{\"schema\":\"io.github.trendorin.vacuo.clean/v1\","
           << "\"success\":" << (report.allSucceeded() ? "true" : "false") << ','
           << "\"bytes_freed\":" << report.bytesFreed << ','
           << "\"items_removed\":" << report.itemsRemoved << ",\"actions\":[";
    for (std::size_t index = 0; index < report.actions.size(); ++index) {
        const auto& action = report.actions[index];
        if (index != 0) {
            output << ',';
        }
        output << "{\"id\":\"" << jsonEscape(action.ruleId) << "\","
               << "\"success\":" << (action.success ? "true" : "false") << ','
               << "\"bytes_freed\":" << action.bytesFreed << ','
               << "\"items_removed\":" << action.itemsRemoved << ','
               << "\"message\":\"" << jsonEscape(action.message) << "\"}";
    }
    output << "]}";
    return output.str();
}

} // namespace vacuo
