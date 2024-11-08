#include "MetricsCollector.h"
#include "ConfigurationManager.h"
#include "HTMLReportGenerator.h"
#include "Logger.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <sstream>
#include <sys/sysinfo.h>

#include <GLES2/gl2.h>
#include <cjson/cJSON.h>

MetricsCollector::MetricsCollector() : frameCount(0), collecting(false), runtimeReport(nullptr) {
    logTrace("MetricsCollector created.");
}

MetricsCollector::~MetricsCollector() {
    stopCollection();
    logTrace("MetricsCollector destroyed.");
}

void MetricsCollector::startCollection() {
    frameCount = 0;
    startBenchTime = std::chrono::steady_clock::now();
    collecting = true;

    collectionThread = std::thread(&MetricsCollector::collectRuntimeMetrics, this);
    logDebug("Metrics collection started.");
}

void MetricsCollector::stopCollection() {
    if (collecting) {
        collecting = false;
        if (collectionThread.joinable()) {
            collectionThread.join();
        }
        endBenchTime = std::chrono::steady_clock::now();
        logDebug("Metrics collection stopped.");
    }
}

void MetricsCollector::clearMetrics() {
    std::lock_guard<std::mutex> lock(metricsMutex);
    collectedMetrics.clear();
    frameCount = 0;
    logTrace("Metrics cleared for a new benchmark run.");
}

void MetricsCollector::incrementFrameCount() { ++frameCount; }

void MetricsCollector::collectStaticSystemInfo() {
    std::ifstream versionFile("/version.txt");
    if (versionFile.is_open()) {
        std::string line;
        while (std::getline(versionFile, line)) {
            if (line.find("imagename:") != std::string::npos) {
                staticInfo["Image Name"] = line.substr(line.find(":") + 1);
                break;
            }
        }
        versionFile.close();
        logTrace("Parsed image name from /version.txt.");
    } else {
        logWarn("/version.txt not found or could not be opened.");
        staticInfo["Image Name"] = "Unknown";
    }

    FILE *unameCmd = popen("uname -r", "r");
    if (unameCmd) {
        char buffer[128];
        if (fgets(buffer, sizeof(buffer), unameCmd) != nullptr) {
            staticInfo["Kernel Version"] = std::string(buffer);
            staticInfo["Kernel Version"].erase(staticInfo["Kernel Version"].find_last_not_of("\n") + 1);
        }
        pclose(unameCmd);
        logTrace("Kernel version collected using `uname -r`.");
    } else {
        logWarn("Failed to run `uname -r` command.");
        staticInfo["Kernel Version"] = "Unknown";
    }

    std::ifstream devicePropertiesFile("/etc/device.properties");
    if (devicePropertiesFile.is_open()) {
        std::string line;
        while (std::getline(devicePropertiesFile, line)) {
            if (line.find("DEVICE_NAME") != std::string::npos) {
                staticInfo["Device Name"] = line.substr(line.find("=") + 1);
            }
        }
        devicePropertiesFile.close();
        logTrace("Parsed device details from /etc/device.properties.");
    } else {
        logWarn("/etc/device.properties not found or could not be opened.");
        staticInfo["Device Name"] = "Unknown";
    }

    const char *version = reinterpret_cast<const char *>(glGetString(GL_VERSION));
    const char *vendor = reinterpret_cast<const char *>(glGetString(GL_VENDOR));
    const char *renderer = reinterpret_cast<const char *>(glGetString(GL_RENDERER));
    const char *extensions = reinterpret_cast<const char *>(glGetString(GL_EXTENSIONS));

    staticInfo["OpenGL Version"] = version ? std::string(version) : "Unknown";
    staticInfo["OpenGL Vendor"] = vendor ? std::string(vendor) : "Unknown";
    staticInfo["OpenGL Renderer"] = renderer ? std::string(renderer) : "Unknown";
    staticInfo["OpenGL Extensions"] = extensions ? std::string(extensions) : "None";

    ConfigurationManager &configManager = ConfigurationManager::getInstance();
    staticInfo["Direct mode"] = configManager.getValue("direct_mode");
    staticInfo["Window size"] = configManager.getValue("window_width") + "x" + configManager.getValue("window_height");

    auto now = std::chrono::system_clock::now();
    auto nowTime = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&nowTime), "%Y-%m-%d %H:%M:%S");
    staticInfo["Timestamp"] = ss.str();

    logDebug("Static system information collected.");
}

void MetricsCollector::collectRuntimeMetrics() {
    ConfigurationManager &configManager = ConfigurationManager::getInstance();
    int samplingRateMs = std::stoi(configManager.getValue("sampling_rate"));
    int sleepTimeMs = 0;

    size_t lastFrameCount = 0;
    auto startTime = std::chrono::steady_clock::now();
    auto lastTime = startTime;
    auto endTime = startTime;

    double fps = 0.0;
    double frameTimeMs = 0.0;
    logTrace("Starting dynamic metrics collection.");
    while (collecting) {
        startTime = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(startTime - lastTime).count();

        if (duration > 0) {
            size_t framesThisInterval = frameCount - lastFrameCount;
            fps = std::min(static_cast<double>(framesThisInterval) / duration, 60.0);
            if (fps) {
                frameTimeMs = 1000.0 / fps;
            } else {
                frameTimeMs = 5000.0;
            }

            recordMetric("FPS", fps, MetricType::GAUGE);
            recordMetric("Frame time (ms)", frameTimeMs, MetricType::GAUGE);

            logInfo("FPS: " + std::to_string(fps) + "  -  Frame time: " + std::to_string(frameTimeMs) + " ms");

            lastFrameCount = frameCount;
            lastTime = startTime;
        }

        // recordMetric("Frame count", frameCount, MetricType::COUNTER);
        recordMetric("CPU load", getCPULoad(), MetricType::GAUGE);
        recordMetric("CPU temperature", getCPUTemperature(), MetricType::GAUGE);
        recordMetric("System memory usage", getSystemMemoryUsage(), MetricType::GAUGE);

        collectPlatformMetrics();

        endTime = std::chrono::steady_clock::now();
        auto elapsedTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
        sleepTimeMs = samplingRateMs - static_cast<int>(elapsedTimeMs);

        if (sleepTimeMs > 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(sleepTimeMs));
        }
    }
    logTrace("Dynamic metrics collection finished.");
}

std::string formatToTwoDecimalPlaces(double value) {
    std::ostringstream out;
    out << std::fixed << std::setprecision(2) << value;
    return out.str();
}

void MetricsCollector::createBenchmarkReport(const std::string &taskName) {
    cJSON *runtimeMetricsJson = cJSON_CreateObject();

    for (const auto &metricEntry : collectedMetrics) {
        const std::string &metricName = metricEntry.first;
        const MetricData &metricData = metricEntry.second;

        cJSON *metricJson = cJSON_CreateObject();

        if (!metricData.values.empty()) {

            if (metricData.type == MetricType::GAUGE) {
                double sum = std::accumulate(metricData.values.begin(), metricData.values.end(), 0.0);
                double avgVal = sum / metricData.values.size();
                double minVal = *std::min_element(metricData.values.begin(), metricData.values.end());
                double maxVal = *std::max_element(metricData.values.begin(), metricData.values.end());

                double variance = 0.0;
                for (double value : metricData.values) {
                    variance += std::pow(value - avgVal, 2);
                }
                variance /= metricData.values.size();
                double stdDev = std::sqrt(variance);

                cJSON_AddStringToObject(metricJson, "average", formatToTwoDecimalPlaces(avgVal).c_str());
                cJSON_AddStringToObject(metricJson, "minimum", formatToTwoDecimalPlaces(minVal).c_str());
                cJSON_AddStringToObject(metricJson, "maximum", formatToTwoDecimalPlaces(maxVal).c_str());
                cJSON_AddStringToObject(metricJson, "std_dev", formatToTwoDecimalPlaces(stdDev).c_str());
            }

            cJSON *valuesArray = cJSON_CreateArray();
            for (double value : metricData.values) {
                cJSON_AddItemToArray(valuesArray, cJSON_CreateString(formatToTwoDecimalPlaces(value).c_str()));
            }
            cJSON_AddItemToObject(metricJson, "values", valuesArray);
        }
        cJSON_AddItemToObject(runtimeMetricsJson, metricName.c_str(), metricJson);
    }

    if (!runtimeReport) {
        runtimeReport = cJSON_CreateObject();
    }

    cJSON_AddItemToObject(runtimeReport, taskName.c_str(), runtimeMetricsJson);
}

cJSON *MetricsCollector::createJSONReport(const std::string &filePath) const {
    cJSON *reportJson = cJSON_CreateObject();

    cJSON *staticInfoJson = cJSON_CreateObject();
    for (const auto &entry : staticInfo) {
        cJSON_AddStringToObject(staticInfoJson, entry.first.c_str(), entry.second.c_str());
    }
    cJSON_AddItemToObject(reportJson, "Environment", staticInfoJson);

    if (runtimeReport) {
        cJSON_AddItemToObject(reportJson, "Benchmark Results", cJSON_Duplicate(runtimeReport, true));
    } else {
        logWarn("No benchmark results to include in the report.");
    }

    char *jsonString = cJSON_Print(reportJson);
    if (!jsonString) {
        logError("Failed to create JSON report string.");
        cJSON_Delete(reportJson);
        reportJson = nullptr;
        return reportJson;
    }

    std::ofstream outFile(filePath);
    if (outFile.is_open()) {
        outFile << jsonString;
        outFile.close();
        logDebug("Benchmark report successfully created at: " + filePath);
    } else {
        logError("Failed to open the file at: " + filePath);
    }

    cJSON_free(jsonString);
    return reportJson;
}

void MetricsCollector::createReport() {
    cJSON *reportJson = createJSONReport("/tmp/valyria_report.json");
    if (reportJson) {
        HTMLReportGenerator html(reportJson, "/tmp/valyria_report.html");
        html.generateReport();
        cJSON_Delete(reportJson);

        logInfo("JSON report: /tmp/valyria_report.json");
        logInfo("HTML report: /tmp/valyria_report.html");
    }
}

void MetricsCollector::recordMetric(const std::string &name, double value, MetricType type) {
    std::lock_guard<std::mutex> lock(metricsMutex);
    if (collectedMetrics.find(name) == collectedMetrics.end()) {
        collectedMetrics[name] = MetricData{type, {value}};
    } else {
        collectedMetrics[name].addValue(value);
    }
    logTrace("Metric recorded: " + name + " = " + std::to_string(value));
}

double MetricsCollector::getCPULoad() const {
    std::ifstream procStat("/proc/stat");
    if (!procStat.is_open()) {
        logWarn("Failed to open /proc/stat for CPU load calculation.");
        return 0.0;
    }

    std::string line;
    std::getline(procStat, line);
    procStat.close();

    std::istringstream iss(line);
    std::string cpuLabel;
    unsigned long user, nice, system, idle;
    iss >> cpuLabel >> user >> nice >> system >> idle;

    static unsigned long prevUser = 0, prevNice = 0, prevSystem = 0, prevIdle = 0;
    double total = (user - prevUser) + (nice - prevNice) + (system - prevSystem);
    double idleTime = idle - prevIdle;
    double usage = (total / (total + idleTime)) * 100.0;

    prevUser = user;
    prevNice = nice;
    prevSystem = system;
    prevIdle = idle;

    logTrace("CPU Load: " + std::to_string(usage));
    return usage;
}

double MetricsCollector::getCPUTemperature() const {
    std::ifstream tempFile("/sys/class/thermal/thermal_zone0/temp");
    if (tempFile.is_open()) {
        double temp;
        tempFile >> temp;
        tempFile.close();
        double temperature = temp / 1000.0;
        logTrace("CPU Temperature: " + std::to_string(temperature));
        return temperature;
    }
    logWarn("Failed to read CPU temperature.");
    return 0.0;
}

double MetricsCollector::getSystemMemoryUsage() const {
    struct sysinfo sysInfo;
    if (sysinfo(&sysInfo) == 0) {
        double totalMemory = sysInfo.totalram * sysInfo.mem_unit;
        double freeMemory = sysInfo.freeram * sysInfo.mem_unit;
        double usage = ((totalMemory - freeMemory) / totalMemory) * 100.0;
        logTrace("System Memory Usage: " + std::to_string(usage));
        return usage;
    }
    logWarn("Failed to retrieve system information for system memory usage.");
    return 0.0;
}
