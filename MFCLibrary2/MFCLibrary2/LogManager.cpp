#include "pch.h"
#include "LogManager.h"
#include <fstream>
#include <ctime>
#include <direct.h>  // for _mkdir
#include <windows.h>

LogManager::LogManager(const std::string& folder) : _logFolder(folder) {
    _ensureFolder(_logFolder);
}

void LogManager::_ensureFolder(const std::string& path) {
    _mkdir(path.c_str());
}

std::string LogManager::_getTodayFilename() {
    time_t now = time(0);
    struct tm t;
    localtime_s(&t, &now);

    char folderName[32];
    strftime(folderName, sizeof(folderName), "%Y-%m-%d", &t);
    std::string fullFolder = _logFolder + "\\" + folderName;

    _ensureFolder(fullFolder); // create date folder

    return fullFolder + "\\ui.txt";
}

std::string LogManager::_getCurrentTime() {
    time_t now = time(0);
    struct tm t;
    localtime_s(&t, &now);

    char buf[16];
    strftime(buf, sizeof(buf), "%H:%M:%S", &t);
    return std::string(buf);
}

void LogManager::WriteLog(const std::string& screen, const std::string& button) {
    std::ofstream log(_getTodayFilename().c_str(), std::ios::app);
    log << "[" << _getCurrentTime() << "] " << screen << " > " << button << std::endl;
}

// ==================== Interface.cpp ====================
#include "LogManager.h"

static LogManager* g_logManager = nullptr;

extern "C" __declspec(dllexport)
void InitUILogger() {
    g_logManager = new LogManager("C:\\log\\ui");
}

extern "C" __declspec(dllexport)
void LogManager_WriteLog(const char* screen, const char* button) {
    if (g_logManager) {
        g_logManager->WriteLog(screen, button);
    }
}