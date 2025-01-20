#pragma once

#include <string>
#include <map>
#include <filesystem>

namespace utils {
    class FileUtils {
    public:
        static bool isPkgFile(const std::string& fileName);
        static void ensureDirectoryExists(const std::string& path);
        static std::string getDataPath();
        static std::string getUsedCodesFilePath();
        static std::string getUsedCodesFilePath(const std::string& packagePath);
        static std::map<std::string, std::string> loadUsedCodes(const std::string& filename);
        static void saveUsedCodes(const std::string& filename, const std::map<std::string, std::string>& codes);
        static std::string readCid(const std::string& filename);
        static void saveCid(const std::string& filename, const std::string& cid);
        static void saveSuccessFile(const std::string& packageName, const std::string& passcode);
    };
}
