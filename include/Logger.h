#ifndef VALYRIA_LOGGER_H
#define VALYRIA_LOGGER_H

#include <iostream>
#include <string>

/**
 * Defines the different levels of logging supported.
 */
enum class LogLevel {
    TRACE,  ///< More detailed information for debugging purposes.
    DEBUG,  ///< Detailed information for debugging purposes.
    INFO,   ///< General information about the application’s execution.
    WARN,   ///< Warning messages indicating potential issues.
    ERROR   ///< Error messages indicating serious issues.
};

/**
 * Contains configuration settings for the logger.
 */
namespace LoggerConfig {
    extern LogLevel currentLevel;
    extern bool fileLoggingEnabled;

    void setLogLevel(LogLevel level);
} // namespace LoggerConfig

LogLevel stringToLogLevel(const std::string &levelStr);

void logMessage(LogLevel level, const std::string &message);

inline void logTrace(const std::string &message) { logMessage(LogLevel::TRACE, message); }
inline void logDebug(const std::string &message) { logMessage(LogLevel::DEBUG, message); }
inline void logInfo(const std::string &message) { logMessage(LogLevel::INFO, message); }
inline void logWarn(const std::string &message) { logMessage(LogLevel::WARN, message); }
inline void logError(const std::string &message) { logMessage(LogLevel::ERROR, message); }

#endif // VALYRIA_LOGGER_H
