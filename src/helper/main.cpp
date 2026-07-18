#include "vacuo/core/helper_actions.hpp"
#include "vacuo/core/system_detector.hpp"

#include <algorithm>
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <set>
#include <string>
#include <sys/file.h>
#include <sys/prctl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>

namespace {

std::string singleLine(std::string value) {
    std::replace(value.begin(), value.end(), '\t', ' ');
    std::replace(value.begin(), value.end(), '\r', ' ');
    std::replace(value.begin(), value.end(), '\n', ' ');
    if (value.size() > 1000) {
        value.resize(1000);
        value += "...";
    }
    return value;
}

void emit(const vacuo::ActionResult& result) {
    std::cout << "VACUO_RESULT\t" << result.ruleId << '\t' << (result.success ? 0 : 1) << '\t'
              << result.bytesFreed << '\t' << result.itemsRemoved << '\t'
              << singleLine(result.message) << '\n';
}

} // namespace

int main(int argc, char* argv[]) {
    if (::geteuid() != 0 || ::getuid() != 0) {
        std::cerr << "vacuo-helper must be launched as root by PolicyKit.\n";
        return 77;
    }
    ::umask(0077);
    (void)::prctl(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0);

    std::set<std::string> actions;
    for (int index = 1; index < argc; ++index) {
        const std::string argument(argv[index]);
        if (argument != "--action" || index + 1 >= argc) {
            std::cerr << "Only repeated --action <fixed-id> arguments are accepted.\n";
            return 64;
        }
        const std::string action(argv[++index]);
        if (!vacuo::HelperActions::isKnown(action)) {
            std::cerr << "Unknown privileged action.\n";
            return 64;
        }
        actions.insert(action);
    }
    if (actions.empty()) {
        std::cerr << "No privileged action requested.\n";
        return 64;
    }

    const int lock = ::open("/run/vacuo-helper.lock", O_CREAT | O_CLOEXEC | O_RDWR | O_NOFOLLOW, 0600);
    if (lock < 0 || ::flock(lock, LOCK_EX | LOCK_NB) != 0) {
        std::cerr << "Another privileged Vacuo cleanup is already running.\n";
        if (lock >= 0) {
            ::close(lock);
        }
        return 75;
    }

    ::clearenv();
    ::setenv("PATH", "/usr/sbin:/usr/bin:/sbin:/bin", 1);
    ::setenv("LANG", "C.UTF-8", 1);
    ::setenv("LC_ALL", "C.UTF-8", 1);

    const auto system = vacuo::SystemDetector::detect();
    bool success = true;
    for (const auto& action : actions) {
        const auto result = vacuo::HelperActions::execute(action, system);
        emit(result);
        success = success && result.success;
    }
    ::flock(lock, LOCK_UN);
    ::close(lock);
    return success ? 0 : 1;
}
