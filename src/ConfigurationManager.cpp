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

#include "ConfigurationManager.h"
#include "Logger.h"

#include <iostream>

#include <cjson/cJSON.h>

ConfigurationManager &ConfigurationManager::getInstance() {
    static ConfigurationManager instance;
    return instance;
}

bool ConfigurationManager::parseCommandLineArguments(int argc, char *argv[]) {
    for (int i = 1; i < argc; ++i) {
        std::string argument = argv[i];
        if (argument == "-h" || argument == "--help") {
            printUsage();
            return true;
        }

        if (argument.rfind("--", 0) == 0) {
            size_t equalSignPos = argument.find('=');
            if (equalSignPos != std::string::npos) {
                std::string key = argument.substr(2, equalSignPos - 2);
                std::string value = argument.substr(equalSignPos + 1);

                if (value.empty()) {
                    logWarn("Option '" + key + "' specified without a value.");
                    continue;
                }

                auto it = configMap.find(key);
                if (it != configMap.end()) {
                    it->second.value = value;
                    logTrace("Updated configuration: " + key + " = " + value);
                } else {
                    logWarn("Configuration key not found: " + key);
                }
            } else {
                logWarn("Invalid argument format for: " + argument);
            }
        }
    }
    return false;
}

void ConfigurationManager::setOption(const std::string &key, const std::string &defaultValue,
                                     const std::string &description) {
    if (configMap.find(key) == configMap.end()) {
        configMap[key] = ConfigOption(defaultValue, description);
        logTrace("Set default configuration: " + key + " = " + defaultValue + " (" + description + ")");
    }
}

std::string ConfigurationManager::getValue(const std::string &key) const {
    auto it = configMap.find(key);
    if (it != configMap.end()) {
        logTrace("Retrieved value for key: " + key + " = " + it->second.value);
        return it->second.value;
    } else {
        logWarn("Configuration key not found: " + key);
        return "";
    }
}

void ConfigurationManager::setValue(const std::string &key, const std::string &value) {
    auto it = configMap.find(key);
    if (it != configMap.end()) {
        it->second.value = value;
        logDebug("Updated configuration key: " + key + " to new value: " + value);
    } else {
        logWarn("Configuration key not found: " + key + ". Update operation skipped.");
    }
}

bool ConfigurationManager::hasKey(const std::string &key) const {
    bool exists = configMap.find(key) != configMap.end();
    if (exists) {
        logTrace("Configuration key exists: " + key);
    } else {
        logWarn("Configuration key does not exist: " + key);
    }
    return exists;
}

void ConfigurationManager::printConfigurations() const {
    logInfo("Current Configurations:");
    for (const auto &pair : configMap) {
        logInfo("--" + pair.first + "=" + pair.second.value);
    }
}

void ConfigurationManager::printUsage() const {
    logInfo("Available options:");
    for (const auto &pair : configMap) {
        logInfo("--" + pair.first + " (default: " + pair.second.value + ") - " + pair.second.description);
    }
}