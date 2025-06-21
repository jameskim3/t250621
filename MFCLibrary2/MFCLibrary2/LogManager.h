#pragma once
#include "pch.h"
#pragma once
#include <string>

class LogManager {
public:
    LogManager(const std::string& folder);
    void WriteLog(const std::string& screen, const std::string& button);

private:
    std::string _logFolder;
    std::string _getTodayFilename();
    std::string _getCurrentTime();
    void _ensureFolder(const std::string& path);
};