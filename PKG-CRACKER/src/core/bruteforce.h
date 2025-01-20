#pragma once

#include <string>
#include <atomic>
#include <csignal>
#include <map>
#include <chrono>
#include <unordered_set>
#include <random>
#include "../utils/file_utils.h"
#include "../utils/command_utils.h"
#include "../ui/menu.h"

namespace core {
    class Bruteforcer {
    public:
        explicit Bruteforcer(const ui::BruteforceOptions& options);
        void start(const std::string& startCode);
        void stop();
        bool isRunning() const;
        static void signalHandler(int signal);
        
    private:
        using TimePoint = std::chrono::steady_clock::time_point;
        
        std::atomic<bool> running;
        static std::atomic<bool> stopSignal;
        std::map<std::string, std::string> usedCodes;
        TimePoint startTime;
        ui::BruteforceOptions options;
        
        bool tryCode(const std::string& code, const std::string& pkgPath);
        std::string generateNextCode(const std::string& currentCode);
        void loadUsedCodes();
        void saveUsedCodes();
    };
}
