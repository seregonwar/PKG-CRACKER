#include "src/core/bruteforce.h"
#include "src/ui/menu.h"
#include <csignal>
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    try {
        std::cout << "[MAIN] Starting PKG-CRACKER" << std::endl;
        std::cout << "[MAIN] Argument count: " << argc << std::endl;

        // Register signal handler
        std::signal(SIGINT, core::Bruteforcer::signalHandler);

        // Show menu and get options
        ui::BruteforceOptions options;

        if (argc < 2) {
            std::cout << "[MAIN] Interactive mode" << std::endl;
            // Interactive mode
            options = ui::Menu::showMainMenu();
        }
        else {
            std::cout << "[MAIN] Command line mode" << std::endl;
            // Command line mode
            if (argc < 3) {
                std::cout << "[MAIN] Insufficient arguments" << std::endl;
                ui::Menu::showHelp();
                return 1;
            }

            options.packagePath = argv[1];
            options.outputPath = argv[2];
            options.numThreads = 0;
            options.silenceMode = false;
            options.startingPasscode = "";

            // Parse optional arguments
            for (int i = 3; i < argc; ++i) {
                std::string arg = argv[i];
                std::cout << "[MAIN] Processing argument: " << arg << std::endl;
                
                if (arg == "--silence") {
                    options.silenceMode = true;
                    std::cout << "[MAIN] Silence mode enabled" << std::endl;
                }
                else if (arg == "-t" && i + 1 < argc) {
                    try {
                        options.numThreads = std::stoi(argv[i + 1]);
                        if (options.numThreads < 0) {
                            std::cerr << "[MAIN] Invalid thread count: Must be a non-negative number." << std::endl;
                            return 1;
                        }
                        std::cout << "[MAIN] Thread count set to: " << options.numThreads << std::endl;
                        i++;
                    }
                    catch (...) {
                        std::cerr << "[MAIN] Invalid thread count." << std::endl;
                        return 1;
                    }
                }
                else if (arg == "--start" && i + 1 < argc) {
                    options.startingPasscode = argv[i + 1];
                    std::cout << "[MAIN] Starting passcode set to: " << options.startingPasscode << std::endl;
                    i++;
                }
                else if (arg == "--help") {
                    ui::Menu::showHelp();
                    return 0;
                }
            }
        }

        std::cout << "[MAIN] Bruteforcer options:" << std::endl;
        std::cout << "  Package Path: " << options.packagePath << std::endl;
        std::cout << "  Output Path: " << options.outputPath << std::endl;
        std::cout << "  Threads: " << options.numThreads << std::endl;
        std::cout << "  Silence Mode: " << (options.silenceMode ? "ON" : "OFF") << std::endl;
        std::cout << "  Starting Passcode: " << (options.startingPasscode.empty() ? "DEFAULT" : options.startingPasscode) << std::endl;

        // Create and start bruteforcer
        core::Bruteforcer bruteforcer(options);
        std::cout << "[MAIN] Starting bruteforce with initial code: " 
                  << (options.startingPasscode.empty() ? "0000" : options.startingPasscode) << std::endl;
        bruteforcer.start(options.startingPasscode.empty() ? "0000" : options.startingPasscode);

        std::cout << "[MAIN] PKG-CRACKER completed successfully" << std::endl;
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "[MAIN] Unhandled exception: " << e.what() << std::endl;
        return 1;
    }
}
