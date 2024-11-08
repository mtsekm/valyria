#include "collectors/BroadcomMetricsCollector.h"
#include "Logger.h"

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

namespace fs = std::filesystem;

void BroadcomMetricsCollector::collectPlatformMetrics() {
    parseAvailableGPULoadFiles();
    parseCoreFile("/proc/brcm/core");
}

void BroadcomMetricsCollector::parseAvailableGPULoadFiles() {
    for (int driIndex : {0, 1, 128}) {
        std::string filePath = "/sys/kernel/debug/dri/" + std::to_string(driIndex) + "/gpu_load";
        if (fs::exists(filePath)) {
            parseGPULoadFile(filePath);
            return;
        }
    }
    logError("No valid gpu_load file found.");
}

void BroadcomMetricsCollector::parseCoreFile(const std::string &filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        logError("Unable to open GFX core file: " + filePath);
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.find("GFX0") != std::string::npos) {
            std::istringstream lineStream(line);
            std::string token;
            int columnIndex = 0;
            double gfxUsed = 0.0;

            while (lineStream >> token) {
                columnIndex++;
                if (columnIndex == 7) {
                    if (token.back() == '%') {
                        token.pop_back();
                    }
                    try {
                        gfxUsed = std::stod(token);
                    } catch (const std::invalid_argument &e) {
                        logError("Invalid format for GFX used value: " + token);
                        return;
                    }
                    recordMetric("GFX Heap used", gfxUsed, MetricType::GAUGE);
                    break;
                }
            }
        }
    }
    file.close();
}

void BroadcomMetricsCollector::parseGPULoadFile(const std::string &filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        logError("Unable to open GPU load file: " + filePath);
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.find("load average:") != std::string::npos) {
            std::istringstream lineStream(line);
            std::string temp;
            double avg16ms = 0.0, avg0_5s = 0.0, avg16s = 0.0;

            lineStream >> temp >> temp; // Skip "load average:"
            lineStream >> avg16ms;
            lineStream.ignore(1, '%');
            lineStream >> temp;        // Skip "@"
            lineStream >> temp;        // Skip "16ms,"

            lineStream >> avg0_5s;
            lineStream.ignore(1, '%');
            lineStream >> temp; // Skip "@"
            lineStream >> temp; // Skip "0.5s,"

            lineStream >> avg16s;
            lineStream.ignore(1, '%');

            recordMetric("Total GPU load (16ms)", avg16ms, MetricType::GAUGE);
            recordMetric("Total GPU load (0.5s)", avg0_5s, MetricType::GAUGE);
            recordMetric("Total GPU load (16s)", avg16s, MetricType::GAUGE);
        } else {
            std::istringstream lineStream(line);
            int pid;
            double avg16ms = 0.0, avg0_5s = 0.0, avg16s = 0.0;
            std::string command;

            lineStream >> pid;
            if (lineStream.fail()) {
                continue;
            }

            lineStream >> avg16ms;
            lineStream.ignore(1, '%');
            lineStream >> avg0_5s;
            lineStream.ignore(1, '%');
            lineStream >> avg16s;
            lineStream.ignore(1, '%');
            lineStream >> command;

            if (command == "valyria" || command == "westeros" || command == "GlRenderLoop") {
                recordMetric(command + " GPU load (16ms)", avg16ms, MetricType::GAUGE);
                recordMetric(command + " GPU load (0.5s)", avg0_5s, MetricType::GAUGE);
                recordMetric(command + " GPU load (16s)", avg16s, MetricType::GAUGE);
            }
        }
    }
    file.close();
}
