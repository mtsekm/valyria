/*
* If not stated otherwise in this file or this component's LICENSE file the
* following copyright and licenses apply:
*
* Copyright 2024 Sky UK
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

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
