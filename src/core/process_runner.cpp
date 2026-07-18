#include "vacuo/core/process_runner.hpp"

#include <algorithm>
#include <array>
#include <cerrno>
#include <csignal>
#include <cstring>
#include <fcntl.h>
#include <poll.h>
#include <sys/wait.h>
#include <thread>
#include <unistd.h>

namespace vacuo {
namespace {

constexpr std::size_t kOutputLimit = 1U << 20U;

void appendFrom(const int descriptor, std::string& destination, bool& open) {
    std::array<char, 4096> buffer{};
    while (open) {
        const auto count = ::read(descriptor, buffer.data(), buffer.size());
        if (count > 0) {
            const auto remaining = kOutputLimit > destination.size() ? kOutputLimit - destination.size() : 0;
            destination.append(buffer.data(), std::min<std::size_t>(static_cast<std::size_t>(count), remaining));
        } else if (count == 0) {
            open = false;
            ::close(descriptor);
        } else if (errno != EAGAIN && errno != EWOULDBLOCK && errno != EINTR) {
            open = false;
            ::close(descriptor);
        } else {
            break;
        }
    }
}

} // namespace

ProcessResult ProcessRunner::run(const std::filesystem::path& executable,
                                 const std::vector<std::string>& arguments,
                                 const std::chrono::seconds timeout) {
    ProcessResult result;
    if (!executable.is_absolute()) {
        result.error = "Process executable must be an absolute path.";
        return result;
    }

    int standardOutput[2]{};
    int standardError[2]{};
    if (::pipe2(standardOutput, O_CLOEXEC) != 0 || ::pipe2(standardError, O_CLOEXEC) != 0) {
        result.error = std::string("pipe2 failed: ") + std::strerror(errno);
        if (standardOutput[0] != 0 || standardOutput[1] != 0) {
            ::close(standardOutput[0]);
            ::close(standardOutput[1]);
        }
        return result;
    }

    const pid_t child = ::fork();
    if (child < 0) {
        result.error = std::string("fork failed: ") + std::strerror(errno);
        ::close(standardOutput[0]);
        ::close(standardOutput[1]);
        ::close(standardError[0]);
        ::close(standardError[1]);
        return result;
    }
    if (child == 0) {
        (void)::setpgid(0, 0);
        ::dup2(standardOutput[1], STDOUT_FILENO);
        ::dup2(standardError[1], STDERR_FILENO);
        ::close(standardOutput[0]);
        ::close(standardOutput[1]);
        ::close(standardError[0]);
        ::close(standardError[1]);

        ::clearenv();
        ::setenv("PATH", "/usr/sbin:/usr/bin:/sbin:/bin", 1);
        ::setenv("LANG", "C.UTF-8", 1);
        ::setenv("LC_ALL", "C.UTF-8", 1);

        const auto executableString = executable.string();
        std::vector<char*> argv;
        argv.reserve(arguments.size() + 2);
        argv.push_back(const_cast<char*>(executableString.c_str()));
        for (const auto& argument : arguments) {
            argv.push_back(const_cast<char*>(argument.c_str()));
        }
        argv.push_back(nullptr);
        ::execv(executableString.c_str(), argv.data());
        _exit(127);
    }

    (void)::setpgid(child, child);
    ::close(standardOutput[1]);
    ::close(standardError[1]);
    ::fcntl(standardOutput[0], F_SETFL, ::fcntl(standardOutput[0], F_GETFL) | O_NONBLOCK);
    ::fcntl(standardError[0], F_SETFL, ::fcntl(standardError[0], F_GETFL) | O_NONBLOCK);

    bool outputOpen = true;
    bool errorOpen = true;
    bool childExited = false;
    int status = 0;
    const auto deadline = std::chrono::steady_clock::now() + timeout;
    while (!childExited || outputOpen || errorOpen) {
        appendFrom(standardOutput[0], result.output, outputOpen);
        appendFrom(standardError[0], result.error, errorOpen);

        if (!childExited) {
            const auto waited = ::waitpid(child, &status, WNOHANG);
            childExited = waited == child;
        }
        if (std::chrono::steady_clock::now() >= deadline && (!childExited || outputOpen || errorOpen)) {
            result.timedOut = true;
            if (!childExited) {
                ::kill(-child, SIGTERM);
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            if (!childExited && ::waitpid(child, &status, WNOHANG) == 0) {
                ::kill(-child, SIGKILL);
            }
            if (!childExited) {
                ::waitpid(child, &status, 0);
                childExited = true;
            }
            if (outputOpen) {
                ::close(standardOutput[0]);
                outputOpen = false;
            }
            if (errorOpen) {
                ::close(standardError[0]);
                errorOpen = false;
            }
        }
        if (!childExited || outputOpen || errorOpen) {
            std::array<pollfd, 2> descriptors{{
                {outputOpen ? standardOutput[0] : -1, POLLIN | POLLHUP, 0},
                {errorOpen ? standardError[0] : -1, POLLIN | POLLHUP, 0},
            }};
            ::poll(descriptors.data(), descriptors.size(), 25);
        }
    }

    if (!result.timedOut) {
        if (WIFEXITED(status)) {
            result.exitCode = WEXITSTATUS(status);
        } else if (WIFSIGNALED(status)) {
            result.exitCode = 128 + WTERMSIG(status);
        }
    }
    return result;
}

} // namespace vacuo
