#include "file_utils.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <chrono>

namespace utils {
    bool FileUtils::isPkgFile(const std::string& fileName) {
        return std::filesystem::path(fileName).extension() == ".pkg";
    }

    void FileUtils::ensureDirectoryExists(const std::string& path) {
        std::filesystem::create_directories(path);
    }

    std::string FileUtils::getDataPath() {
        return std::filesystem::current_path().string() + "/data";
    }

    std::string FileUtils::getUsedCodesFilePath() {
        return getDataPath() + "/used_codes.xml";
    }

    std::string FileUtils::getUsedCodesFilePath(const std::string& packagePath) {
        std::filesystem::path pkgPath(packagePath);
        std::string filename = pkgPath.stem().string() + "_used_codes.xml";
        return getDataPath() + "/" + filename;
    }

    std::map<std::string, std::string> FileUtils::loadUsedCodes(const std::string& filename) {
        std::map<std::string, std::string> codes;
        std::ifstream file(filename);
        
        if (file.is_open()) {
            std::string line;
            std::string currentCode;
            std::string currentStatus;
            bool insideCode = false;
            
            while (std::getline(file, line)) {
                if (line.find("<code>") != std::string::npos) {
                    insideCode = true;
                    currentCode = line.substr(line.find(">") + 1, line.find("</code>") - line.find(">") - 1);
                }
                else if (line.find("<status>") != std::string::npos && insideCode) {
                    currentStatus = line.substr(line.find(">") + 1, line.find("</status>") - line.find(">") - 1);
                    codes[currentCode] = currentStatus;
                    insideCode = false;
                }
            }
            file.close();
        }
        
        return codes;
    }

    void FileUtils::saveUsedCodes(const std::string& filename, const std::map<std::string, std::string>& codes) {
        ensureDirectoryExists(std::filesystem::path(filename).parent_path().string());
        
        std::ofstream file(filename);
        if (file.is_open()) {
            file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
            file << "<codes>\n";
            for (const auto& [code, status] : codes) {
                file << "  <entry>\n";
                file << "    <code>" << code << "</code>\n";
                file << "    <status>" << status << "</status>\n";
                file << "  </entry>\n";
            }
            file << "</codes>";
            file.close();
        }
    }

    std::string FileUtils::readCid(const std::string& filename) {
        std::string cid;
        std::ifstream file(filename);
        
        if (file.is_open()) {
            std::string line;
            while (std::getline(file, line)) {
                if (line.find("<cid>") != std::string::npos) {
                    cid = line.substr(line.find(">") + 1, line.find("</cid>") - line.find(">") - 1);
                    break;
                }
            }
            file.close();
        }
        
        return cid;
    }

    void FileUtils::saveCid(const std::string& filename, const std::string& cid) {
        std::ofstream file(filename);
        if (file.is_open()) {
            file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
            file << "<data>\n";
            file << "  <cid>" << cid << "</cid>\n";
            file << "</data>";
            file.close();
        }
    }

    void FileUtils::saveSuccessFile(const std::string& packageName, const std::string& passcode) {
        std::string timestamp = std::to_string(
            std::chrono::system_clock::now().time_since_epoch().count()
        );
        
        std::string filename = getDataPath() + "/" + packageName + "_success.xml";
        std::ofstream file(filename);
        if (file.is_open()) {
            file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
            file << "<success>\n";
            file << "  <package>" << packageName << "</package>\n";
            file << "  <passcode>" << passcode << "</passcode>\n";
            file << "  <timestamp>" << timestamp << "</timestamp>\n";
            file << "</success>";
            file.close();
        }
    }
}
