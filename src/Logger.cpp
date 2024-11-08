#include "Logger.h"

#include <algorithm>
#include <cctype>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace LoggerConfig {
LogLevel currentLevel = LogLevel::INFO;
bool fileLoggingEnabled = false;

void setLogLevel(LogLevel level) { currentLevel = level; }

} // namespace LoggerConfig

std::string toLowerCase(const std::string &str) {
    std::string lowerStr = str;
    std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), [](unsigned char c) { return std::tolower(c); });
    return lowerStr;
}

LogLevel stringToLogLevel(const std::string &levelStr) {
    std::string lowerLevelStr = toLowerCase(levelStr);

    if (lowerLevelStr == "trace") {
        return LogLevel::TRACE;
    } else if (lowerLevelStr == "debug") {
        return LogLevel::DEBUG;
    } else if (lowerLevelStr == "info") {
        return LogLevel::INFO;
    } else if (lowerLevelStr == "warn") {
        return LogLevel::WARN;
    } else if (lowerLevelStr == "error") {
        return LogLevel::ERROR;
    } else {
        return LogLevel::INFO;
    }
}

void logMessage(LogLevel level, const std::string &message) {
    if (level < LoggerConfig::currentLevel) {
        return;
    }

    auto now = std::chrono::system_clock::now();
    auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    auto duration_ms = now_ms.time_since_epoch().count() % 1000;

    std::ostringstream timestamp;
    std::tm *tm_ptr = std::localtime(&in_time_t);
    timestamp << std::setfill('0') << std::setw(2) << tm_ptr->tm_hour << ':' << std::setfill('0') << std::setw(2)
              << tm_ptr->tm_min << ':' << std::setfill('0') << std::setw(2) << tm_ptr->tm_sec << '.'
              << std::setfill('0') << std::setw(3) << duration_ms;

    std::string levelStr;
    switch (level) {
    case LogLevel::TRACE:
        levelStr = " [TRC] ";
        break;
    case LogLevel::DEBUG:
        levelStr = " [DBG] ";
        break;
    case LogLevel::INFO:
        levelStr = " [INF] ";
        break;
    case LogLevel::WARN:
        levelStr = " [WRN] ";
        break;
    case LogLevel::ERROR:
        levelStr = " [ERR] ";
        break;
    }

    std::ostringstream logEntry;
    logEntry << "[" << timestamp.str() << "]" << levelStr << message << std::endl;
    std::cout << logEntry.str();
}
