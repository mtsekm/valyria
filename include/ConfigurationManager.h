#ifndef VALYRIA_CONFIGURATION_MANAGER_H
#define VALYRIA_CONFIGURATION_MANAGER_H

#include <string>
#include <unordered_map>

class cJSON;

/**
 * Represents a general configuration option with its value and description.
 */
struct ConfigOption {
    std::string value;
    std::string description;

    ConfigOption() : value(""), description("No description available") {}
    ConfigOption(const std::string &val, const std::string &desc) : value(val), description(desc) {}
};

/**
 * Singleton class for managing configuration options.
 */
class ConfigurationManager {
public:
    static ConfigurationManager &getInstance();

    ConfigurationManager(const ConfigurationManager &) = delete;
    ConfigurationManager &operator=(const ConfigurationManager &) = delete;

    bool parseCommandLineArguments(int argc, char *argv[]);
    void setOption(const std::string &key, const std::string &defaultValue, const std::string &description);
    std::string getValue(const std::string &key) const;
    void setValue(const std::string &key, const std::string &value);
    bool hasKey(const std::string &key) const;
    void printConfigurations() const;
    void printUsage() const;

private:
    ConfigurationManager() = default;

    std::unordered_map<std::string, ConfigOption> configMap;   ///< Stores general configuration options.
};

#endif // VALYRIA_CONFIGURATION_MANAGER_H
