#include "bruteforce.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <iomanip>
#include <ctime> // Added for time function
#include <cstdlib> // Added for srand function
#include <fstream> // Added for file operations

namespace core {
    std::atomic<bool> Bruteforcer::stopSignal(false);

    Bruteforcer::Bruteforcer(const ui::BruteforceOptions& options) 
        : running(false)
        , startTime(std::chrono::steady_clock::now())
        , options(options) {
        std::signal(SIGINT, signalHandler);
        srand(static_cast<unsigned int>(time(nullptr)));
    }

    void Bruteforcer::signalHandler(int signal) {
        if (signal == SIGINT) {
            stopSignal = true;
            std::cout << "\nReceived interrupt signal. Stopping..." << std::endl;
        }
    }

    void Bruteforcer::start(const std::string& startCode) {
        std::cout << "[BRUTEFORCER] Starting bruteforce with initial code: " << startCode << std::endl;
        
        if (running) {
            std::cout << "[BRUTEFORCER] ERROR: Bruteforcer is already running!" << std::endl;
            return;
        }

        // Validate package path
        if (options.packagePath.empty()) {
            std::cerr << "[BRUTEFORCER] ERROR: No package path specified!" << std::endl;
            return;
        }

        running = true;
        stopSignal = false;
        startTime = std::chrono::steady_clock::now();
        
        std::cout << "[BRUTEFORCER] Package Path: " << options.packagePath << std::endl;
        std::cout << "[BRUTEFORCER] Output Path: " << options.outputPath << std::endl;
        std::cout << "[BRUTEFORCER] Threads: " << options.numThreads << std::endl;
        std::cout << "[BRUTEFORCER] Silence Mode: " << (options.silenceMode ? "ON" : "OFF") << std::endl;

        std::cout << "[BRUTEFORCER] Loading used codes..." << std::endl;
        loadUsedCodes();

        std::string currentCode = startCode;
        std::cout << "[BRUTEFORCER] Initial code set to: " << currentCode << std::endl;
        
        size_t totalAttempts = 0;
        const size_t MAX_ATTEMPTS = 1000000;  // Prevent infinite loop

        while (running && !stopSignal && totalAttempts < MAX_ATTEMPTS) {
            std::cout << "[BRUTEFORCER] Attempt #" << totalAttempts 
                      << " - Trying code: " << currentCode << std::endl;
            
            try {
                if (tryCode(currentCode, options.packagePath)) {
                    const auto duration = std::chrono::duration_cast<std::chrono::seconds>(
                        std::chrono::steady_clock::now() - startTime
                    ).count();
                    
                    std::cout << "\n[BRUTEFORCER] SUCCESS! Passcode found: " << currentCode << std::endl;
                    std::cout << "[BRUTEFORCER] Time taken: " << duration << " seconds" << std::endl;
                    std::cout << "[BRUTEFORCER] Total attempts: " << totalAttempts << std::endl;
                    
                    utils::FileUtils::saveSuccessFile(options.packagePath, currentCode);
                    break;
                }
            }
            catch (const std::exception& e) {
                std::cerr << "[BRUTEFORCER] Exception during code attempt: " << e.what() << std::endl;
            }

            currentCode = generateNextCode(currentCode);
            totalAttempts++;
            
            // Print progress every 100 attempts
            if (totalAttempts % 100 == 0 && !options.silenceMode) {
                ui::Menu::showProgress(currentCode, "Testing codes...", startTime, false);
            }
        }

        if (totalAttempts >= MAX_ATTEMPTS) {
            std::cout << "[BRUTEFORCER] WARN: Maximum attempts reached!" << std::endl;
        }

        running = false;
        saveUsedCodes();
        std::cout << "[BRUTEFORCER] Bruteforce completed. Total attempts: " << totalAttempts << std::endl;
    }

    void Bruteforcer::stop() {
        running = false;
    }

    bool Bruteforcer::isRunning() const {
        return running;
    }

    std::string Bruteforcer::generateNextCode(const std::string& currentCode) {
        std::cout << "[BRUTEFORCER] Generating next code from: " << currentCode << std::endl;
        
        if (currentCode.empty() || currentCode == "0000") {
            // Generate a random 32-character hex string
            std::string result;
            const char hexChars[] = "0123456789ABCDEF";
            for (int i = 0; i < 32; i++) {
                result += hexChars[rand() % 16];
            }
            std::cout << "[BRUTEFORCER] Generated random start code: " << result << std::endl;
            return result;
        }

        // Increment the hex string
        std::string nextCode = currentCode;
        for (int i = nextCode.length() - 1; i >= 0; i--) {
            if (nextCode[i] == 'F') {
                nextCode[i] = '0';
                continue;
            }
            if (nextCode[i] == '9') {
                nextCode[i] = 'A';
                break;
            }
            nextCode[i]++;
            break;
        }
        
        std::cout << "[BRUTEFORCER] Generated next incremental code: " << nextCode << std::endl;
        return nextCode;
    }

    bool Bruteforcer::tryCode(const std::string& code, const std::string& pkgPath) {
        std::cout << "[BRUTEFORCER] Attempting to validate code: " << code << std::endl;
        
        if (usedCodes.find(code) != usedCodes.end()) {
            std::cout << "[BRUTEFORCER] Code already tried, skipping: " << code << std::endl;
            return false;
        }

        std::cout << "[BRUTEFORCER] Calling CommandUtils to try code..." << std::endl;
        bool success = utils::CommandUtils::tryCode(code, pkgPath);
        
        std::cout << "[BRUTEFORCER] Code attempt result: " 
                  << (success ? "SUCCESS" : "FAILED") << std::endl;
        
        usedCodes[code] = success ? "success" : "failed";
        return success;
    }

    void Bruteforcer::loadUsedCodes() {
        std::cout << "[BRUTEFORCER] Attempting to load used codes..." << std::endl;
        
        try {
            std::string usedCodesPath = utils::FileUtils::getUsedCodesFilePath(options.packagePath);
            std::cout << "[BRUTEFORCER] Used codes file path: " << usedCodesPath << std::endl;
            
            std::ifstream file(usedCodesPath);
            if (!file.is_open()) {
                std::cout << "[BRUTEFORCER] No previous used codes file found. Starting fresh." << std::endl;
                return;
            }

            std::string line;
            while (std::getline(file, line)) {
                // Format: code,status
                size_t commaPos = line.find(',');
                if (commaPos != std::string::npos) {
                    std::string code = line.substr(0, commaPos);
                    std::string status = line.substr(commaPos + 1);
                    usedCodes[code] = status;
                    std::cout << "[BRUTEFORCER] Loaded code: " << code << " (status: " << status << ")" << std::endl;
                }
            }

            file.close();
            std::cout << "[BRUTEFORCER] Loaded " << usedCodes.size() << " used codes." << std::endl;
        }
        catch (const std::exception& e) {
            std::cerr << "[BRUTEFORCER] Error loading used codes: " << e.what() << std::endl;
        }
    }

    void Bruteforcer::saveUsedCodes() {
        std::cout << "[BRUTEFORCER] Attempting to save used codes..." << std::endl;
        
        try {
            std::string usedCodesPath = utils::FileUtils::getUsedCodesFilePath(options.packagePath);
            std::cout << "[BRUTEFORCER] Used codes file path: " << usedCodesPath << std::endl;
            
            std::ofstream file(usedCodesPath, std::ios::trunc);
            if (!file.is_open()) {
                std::cerr << "[BRUTEFORCER] ERROR: Could not open file for saving used codes." << std::endl;
                return;
            }

            for (const auto& entry : usedCodes) {
                file << entry.first << "," << entry.second << std::endl;
                std::cout << "[BRUTEFORCER] Saved code: " << entry.first 
                          << " (status: " << entry.second << ")" << std::endl;
            }

            file.close();
            std::cout << "[BRUTEFORCER] Saved " << usedCodes.size() << " used codes." << std::endl;
        }
        catch (const std::exception& e) {
            std::cerr << "[BRUTEFORCER] Error saving used codes: " << e.what() << std::endl;
        }
    }
}
