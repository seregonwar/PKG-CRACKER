#pragma once
#include <string>
#include <vector>
#include <chrono>

namespace ui {
    struct BruteforceOptions {
        std::string packagePath;
        std::string outputPath;
        int numThreads;
        bool silenceMode;
        std::string startingPasscode;
    };

    class Menu {
    public:
        static const std::string ASCII_ART;
        static BruteforceOptions showMainMenu();
        static void showHelp();
        static void showProgress(const std::string& passcode, const std::string& identifier, 
                               const std::chrono::steady_clock::time_point& startTime,
                               bool isPs5);
        static void showSuccess(const std::string& passcode, const std::string& successFile);
        static void showError(const std::string& message);

    private:
        static void clearScreen();
        static void printHeader();
        static std::string getValidPackagePath();
        static std::string getValidOutputPath();
        static int getValidThreadCount();
        static bool getSilenceMode();
        static std::string getStartingPasscode();
    };
}
