#include "command_utils.h"
#include <filesystem>
#include <stdexcept>
#include <iostream>
#include <fstream> 
#include <string>
#include <vector>
#include <Windows.h>

namespace utils {
    ProcessResult CommandUtils::executeCommand(const std::string& command) {
        ProcessResult result;
        SECURITY_ATTRIBUTES saAttr;
        saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
        saAttr.bInheritHandle = TRUE;
        saAttr.lpSecurityDescriptor = NULL;

        HANDLE hChildStdoutRd, hChildStdoutWr;
        if (!CreatePipe(&hChildStdoutRd, &hChildStdoutWr, &saAttr, 0)) {
            std::cerr << "[COMMAND_UTILS] Failed to create pipe" << std::endl;
            return result;
        }

        if (!SetHandleInformation(hChildStdoutRd, HANDLE_FLAG_INHERIT, 0)) {
            std::cerr << "[COMMAND_UTILS] Failed to set pipe handle information" << std::endl;
            CloseHandle(hChildStdoutRd);
            CloseHandle(hChildStdoutWr);
            return result;
        }

        STARTUPINFOW si;
        PROCESS_INFORMATION pi;
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        si.hStdError = hChildStdoutWr;
        si.hStdOutput = hChildStdoutWr;
        si.dwFlags |= STARTF_USESTDHANDLES;
        ZeroMemory(&pi, sizeof(pi));

        // Converti il comando in wide string
        std::string fullCmd = "cmd.exe /c " + command;
        int wlen = MultiByteToWideChar(CP_UTF8, 0, fullCmd.c_str(), -1, NULL, 0);
        std::vector<wchar_t> wcommand(wlen);
        MultiByteToWideChar(CP_UTF8, 0, fullCmd.c_str(), -1, wcommand.data(), wlen);

        if (!CreateProcessW(NULL, wcommand.data(), NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
            DWORD error = GetLastError();
            std::cerr << "[COMMAND_UTILS] Failed to create process. Error code: " << error << std::endl;
            CloseHandle(hChildStdoutRd);
            CloseHandle(hChildStdoutWr);
            return result;
        }

        CloseHandle(hChildStdoutWr);

        std::string output;
        char buffer[4096];
        DWORD bytesRead;
        while (ReadFile(hChildStdoutRd, buffer, sizeof(buffer) - 1, &bytesRead, NULL)) {
            if (bytesRead == 0) break;
            buffer[bytesRead] = '\0';
            output += buffer;
        }

        WaitForSingleObject(pi.hProcess, INFINITE);
        DWORD exitCode;
        GetExitCodeProcess(pi.hProcess, &exitCode);

        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        CloseHandle(hChildStdoutRd);

        result.returnCode = exitCode;
        result.output = output;

        return result;
    }

    bool CommandUtils::checkExecutable(const std::string& executableName) {
        std::vector<std::string> searchPaths = {
            ".",                                    // Current directory
            std::filesystem::current_path().string(), // Full current path
            R"(c:\Users\marco\source\repos\Waste_Ur_Time\PS4_Passcode_Bruteforcer\build\Release)" // Exact executable path
        };

        std::cout << "[COMMAND_UTILS] Searching for executable '" << executableName << "' in:" << std::endl;
        for (const auto& path : searchPaths) {
            std::cout << "  - " << path << std::endl;
        }

        for (const auto& path : searchPaths) {
            std::filesystem::path fullPath = std::filesystem::path(path) / executableName;
            
            std::cout << "[COMMAND_UTILS] Checking executable path: " << fullPath.string() << std::endl;
            
            if (std::filesystem::exists(fullPath)) {
                std::cout << "[COMMAND_UTILS] Found executable: " << fullPath.string() << std::endl;
                return true;
            }
        }

        std::cerr << "[COMMAND_UTILS] ERROR: Could not find executable '" 
                  << executableName << "' in any search path." << std::endl;
        return false;
    }

    bool CommandUtils::tryCode(const std::string& code, const std::string& packagePath) {
        std::cout << "[COMMAND_UTILS] Attempting to validate code: " << code << std::endl;
        std::cout << "[COMMAND_UTILS] Using PKG file: " << packagePath << std::endl;

        // Magic numbers per PS4 e PS5 PKG
        const unsigned char ps4_header_magic[] = { 0x7F, 0x43, 0x4E, 0x54 };
        const unsigned char ps5_header_magic[] = { 0x7F, 0x46, 0x49, 0x48 };

        // Leggi l'header del PKG per determinare il tipo
        std::cout << "[COMMAND_UTILS] Reading PKG header..." << std::endl;
        std::ifstream file(packagePath, std::ios::binary);
        if (!file) {
            std::cerr << "[COMMAND_UTILS] Failed to open package file." << std::endl;
            return false;
        }

        // Leggi i primi 256 bytes per l'analisi dell'header
        std::vector<unsigned char> header_data(256);
        if (!file.read(reinterpret_cast<char*>(header_data.data()), header_data.size())) {
            std::cerr << "[COMMAND_UTILS] Failed to read package header." << std::endl;
            file.close();
            return false;
        }

        // Verifica il magic number
        bool is_ps4_pkg = std::memcmp(header_data.data(), ps4_header_magic, 4) == 0;
        bool is_ps5_pkg = std::memcmp(header_data.data(), ps5_header_magic, 4) == 0;

        std::cout << "[COMMAND_UTILS] PKG Header magic: ";
        for (int i = 0; i < 4; i++) {
            std::cout << std::hex << std::setw(2) << std::setfill('0') 
                     << static_cast<int>(header_data[i]) << " ";
        }
        std::cout << std::dec << std::endl;

        if (!is_ps4_pkg && !is_ps5_pkg) {
            std::cerr << "[COMMAND_UTILS] Invalid PKG format - unrecognized magic number" << std::endl;
            file.close();
            return false;
        }

        std::cout << "[COMMAND_UTILS] Detected " << (is_ps4_pkg ? "PS4" : "PS5") << " PKG format" << std::endl;

        // Verifica la dimensione del file
        file.seekg(0, std::ios::end);
        std::uintmax_t fileSize = file.tellg();
        file.close();

        if (fileSize < 256) {
            std::cerr << "[COMMAND_UTILS] Package file is too small." << std::endl;
            return false;
        }
        std::cout << "[COMMAND_UTILS] Package file size: " << fileSize << " bytes" << std::endl;

        // Verifica che l'eseguibile esista
        std::string executableName = is_ps4_pkg ? "orbis-pub-cmd.exe" : "prospero-pub-cmd.exe";
        if (!std::filesystem::exists(executableName)) {
            std::cerr << "[COMMAND_UTILS] Required executable '" << executableName << "' not found." << std::endl;
            return false;
        }

        // Crea la directory di output appropriata
        std::filesystem::path outputDir = std::filesystem::current_path() / (is_ps5_pkg ? "ps5_output" : "output");
        try {
            if (!std::filesystem::exists(outputDir)) {
                std::filesystem::create_directory(outputDir);
                std::cout << "[COMMAND_UTILS] Created output directory: " << outputDir.string() << std::endl;
            }
        }
        catch (const std::exception& e) {
            std::cerr << "[COMMAND_UTILS] Failed to create output directory: " << e.what() << std::endl;
            return false;
        }

        // Per PS4, vogliamo estrarre specificamente le directory Sc0 e Image0
        std::string targetPath = is_ps4_pkg ? ":Sc0:Image0" : "";
        
        // Costruisci il comando con il percorso target opzionale
        std::string command = executableName + " img_extract " + 
                             (is_ps5_pkg ? "--no-validation " : "") +
                             "--passcode " + code + " " + 
                             "\"" + packagePath + "\"" + targetPath + " " + 
                             "\"" + outputDir.string() + "\"";

        try {
            std::cout << "[COMMAND_UTILS] Executing command: " << command << std::endl;
            ProcessResult result = executeCommand(command);
            
            std::cout << "[COMMAND_UTILS] Command result - Return Code: " << result.returnCode << std::endl;
            if (!result.output.empty()) {
                std::cout << "[COMMAND_UTILS] Command output:" << std::endl << result.output << std::endl;
            }

            // Verifica il successo del comando
            bool hasError = (result.returnCode != 0) ||
                           (result.output.find("[Error]") != std::string::npos) ||
                           (result.output.find("invalid") != std::string::npos);

            if (!hasError) {
                std::cout << "[COMMAND_UTILS] Code validation successful." << std::endl;
                return true;
            }

            // Log dettagliato degli errori
            if (result.returnCode != 0) {
                std::cerr << "[COMMAND_UTILS] Command failed with return code: " << result.returnCode << std::endl;
            }
            if (result.output.find("[Error]") != std::string::npos) {
                std::cerr << "[COMMAND_UTILS] Error found in command output." << std::endl;
            }
            if (result.output.find("invalid") != std::string::npos) {
                std::cerr << "[COMMAND_UTILS] Invalid passcode or package." << std::endl;
            }
        }
        catch (const std::exception& e) {
            std::cerr << "[COMMAND_UTILS] Exception during command execution: " << e.what() << std::endl;
        }

        std::cout << "[COMMAND_UTILS] Code validation failed." << std::endl;
        return false;
    }

    void CommandUtils::setWindowTitle(const std::string& title) {
        SetConsoleTitleA(title.c_str());
    }
}
