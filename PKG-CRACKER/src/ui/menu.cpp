#include "menu.h"
#include "../utils/file_utils.h"
#include <iostream>
#include <iomanip>
#include <thread>

namespace ui {
    const std::string Menu::ASCII_ART = R"(
   ___  _  _ ____    ____ ____ ____ ____ _  _ ____ ____ 
   |__] |_/  | __    |    |__/ |__| |    |_/  |___ |__/ 
   |    | \_ |__]    |___ |  \ |  | |___ | \_ |___ |  \ 
                                                         
)";

    void Menu::clearScreen() {
        system("cls");
    }

    void Menu::printHeader() {
        clearScreen();
        std::cout << ASCII_ART << std::endl;
        std::cout << "Made by SeregonWar - v0.1b" << std::endl << std::endl;
    }

    std::string Menu::getValidPackagePath() {
        std::string packagePath;
        while (true) {
            std::cout << "Enter package file path: ";
            std::getline(std::cin, packagePath);
            
            if (utils::FileUtils::isPkgFile(packagePath)) {
                return packagePath;
            }
            std::cout << "[-] Invalid package file format. Please enter a .pkg file." << std::endl;
        }
    }

    std::string Menu::getValidOutputPath() {
        std::string outputPath;
        std::cout << "Enter output directory path: ";
        std::getline(std::cin, outputPath);
        return outputPath;
    }

    int Menu::getValidThreadCount() {
        int defaultThreads = std::thread::hardware_concurrency();
        if (defaultThreads == 0) defaultThreads = 4;

        std::cout << "Enter number of threads (0 for default: " << defaultThreads << "): ";
        std::string input;
        std::getline(std::cin, input);
        
        if (input.empty() || input == "0") {
            return defaultThreads;
        }

        try {
            int threads = std::stoi(input);
            if (threads < 0) {
                std::cout << "[-] Invalid thread count. Using default: " << defaultThreads << std::endl;
                return defaultThreads;
            }
            return threads;
        }
        catch (...) {
            std::cout << "[-] Invalid input. Using default: " << defaultThreads << std::endl;
            return defaultThreads;
        }
    }

    bool Menu::getSilenceMode() {
        std::cout << "Enable silence mode? (y/N): ";
        std::string input;
        std::getline(std::cin, input);
        return (input == "y" || input == "Y");
    }

    std::string Menu::getStartingPasscode() {
        std::cout << "Enter starting passcode (leave empty for random): ";
        std::string input;
        std::getline(std::cin, input);
        
        if (input.empty()) {
            return "0000";  // Will trigger random generation in generateNextCode
        }
        
        // Convert to uppercase
        for (char& c : input) {
            c = toupper(c);
        }
        
        // Validate passcode format (32 hex characters)
        if (input.length() != 32) {
            std::cout << "[-] Invalid passcode length. Using random starting point." << std::endl;
            return "0000";
        }
        
        // Check if all characters are valid hex
        const std::string validChars = "0123456789ABCDEF";
        for (char c : input) {
            if (validChars.find(c) == std::string::npos) {
                std::cout << "[-] Invalid character in passcode. Using random starting point." << std::endl;
                return "0000";
            }
        }
        
        return input;
    }

    BruteforceOptions Menu::showMainMenu() {
        BruteforceOptions options;
        printHeader();
        
        std::cout << "PS4/PS5 Package Passcode Bruteforcer" << std::endl;
        std::cout << "==================================" << std::endl << std::endl;
        
        options.packagePath = getValidPackagePath();
        options.outputPath = getValidOutputPath();
        options.numThreads = getValidThreadCount();
        options.silenceMode = getSilenceMode();
        options.startingPasscode = getStartingPasscode();
        
        return options;
    }

    void Menu::showHelp() {
        std::cout << "\nUsage Guide:" << std::endl
                  << "1. Enter the path to your PS4/PS5 package file (.pkg)" << std::endl
                  << "2. Specify an output directory for extracted files" << std::endl
                  << "3. Choose the number of threads (more threads = faster, but more CPU usage)" << std::endl
                  << "4. Enable silence mode to reduce console output" << std::endl
                  << "5. Optionally specify a starting passcode" << std::endl
                  << "\nThe program will:" << std::endl
                  << "- Save all tried passcodes to avoid duplicates" << std::endl
                  << "- Continue from where it left off in case of interruption" << std::endl
                  << "- Save successful passcodes to a .success file" << std::endl;
    }

    void Menu::showProgress(const std::string& passcode, const std::string& identifier,
                          const std::chrono::steady_clock::time_point& startTime,
                          bool isPs5) {
        auto now = std::chrono::steady_clock::now();
        auto elapsed_seconds = std::chrono::duration_cast<std::chrono::seconds>(now - startTime).count();
        auto hours = elapsed_seconds / 3600;
        auto minutes = (elapsed_seconds % 3600) / 60;
        auto seconds = elapsed_seconds % 60;

        std::cout << "[" << std::setw(2) << std::setfill('0') << hours << "h "
                  << std::setw(2) << std::setfill('0') << minutes << "m "
                  << std::setw(2) << std::setfill('0') << seconds << "s] | Passcode: " << passcode;
        
        if (!isPs5) {
            std::cout << " | " << identifier;
        } else {
            std::cout << " | " << identifier;
        }
        std::cout << std::endl;
    }

    void Menu::showSuccess(const std::string& passcode, const std::string& successFile) {
        std::cout << "[+] We did it!" << std::endl;
        std::cout << "[+] Passcode: " << passcode << std::endl;
        std::cout << "[+] Passcode has been saved to: " << successFile << std::endl;
    }

    void Menu::showError(const std::string& message) {
        std::cerr << "[-] " << message << std::endl;
    }
}
