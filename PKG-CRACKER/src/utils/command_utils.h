#pragma once
#include <string>
#include <Windows.h>
#include <chrono>

namespace utils {
    struct ProcessResult {
        int returnCode;
        std::string output = "";
    };

    class CommandUtils {
    public:
        static ProcessResult executeCommand(const std::string& command);
        static bool checkExecutable(const std::string& executableName);
        static void setWindowTitle(const std::string& title);
        static bool tryCode(const std::string& code, const std::string& pkgPath);
    };
}
