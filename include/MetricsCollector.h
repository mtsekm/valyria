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

#ifndef VALYRIA_METRICSCOLLECTOR_H
#define VALYRIA_METRICSCOLLECTOR_H

#include <atomic>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

class cJSON;

/**
 * Enum representing types of metrics that can be collected.
 */
enum class MetricType {
    GAUGE,  ///< Value sampled at a single point in time.
    COUNTER ///< Cumulative value that increments over time.
};

/**
 * Struct representing a single metric entry that holds multiple values for tracking over time.
 */
struct MetricData {
    MetricType type;            ///< Type of metric (GAUGE or COUNTER).
    std::vector<double> values; ///< Collection of recorded values.

    void addValue(double value) { values.push_back(value); }
};

/**
 * The `MetricsCollector` class collects, processes, and manages system and performance metrics
 * during a benchmarking session.
 */
class MetricsCollector {
public:
    /**
     * Constructs a new `MetricsCollector`.
     */
    MetricsCollector();

    /**
     * Destructor for `MetricsCollector`.
     */
    virtual ~MetricsCollector();

    /**
     * Begins the background metric collection process, including system and platform-specific data.
     */
    void startCollection();

    /**
     * Stops the background metric collection process.
     */
    void stopCollection();

    /**
     * Clears all collected metrics data, preparing the collector for a new benchmark session.
     */
    void clearMetrics();

    /**
     * Increments the internal frame counter for FPS calculations, typically called on each frame render.
     */
    void incrementFrameCount();

    /**
     * Gathers static system information, such as OS version or build metadata, at the start of a benchmark.
     */
    void collectStaticSystemInfo();

    /**
     * Collects dynamic runtime metrics, including CPU load, memory usage, and other system parameters.
     */
    void collectRuntimeMetrics();

    /**
     * Collects platform-specific metrics that may vary across system configurations.
     * Derived classes should implement this to add custom metrics relevant to a specific platform.
     */
    virtual void collectPlatformMetrics() {};

    /**
     * Creates a report for the benchmark run from collected metrics.
     * 
     * @param tasks The number of RenderTasks.
     */
    void createReport(int tasks);

protected:
    /**
     * Records a metric by name, value, and type.
     *
     * @param name The metric's unique identifier.
     * @param value The current value of the metric.
     * @param type The metric type (e.g., GAUGE or COUNTER).
     */
    void recordMetric(const std::string &name, double value, MetricType type);

    /**
     * Retrieves the current CPU load percentage.
     *
     * @return The CPU load as a percentage.
     */
    double getCPULoad() const;

    /**
     * Retrieves the current CPU temperature.
     *
     * @return The CPU temperature in degrees Celsius.
     */
    double getCPUTemperature() const;

    /**
     * Retrieves the current memory usage of the system.
     *
     * @return The memory usage in megabytes (MB).
     */
    double getSystemMemoryUsage() const;

    std::chrono::time_point<std::chrono::steady_clock> startBenchTime; ///< Start time of the benchmark.
    std::chrono::time_point<std::chrono::steady_clock> endBenchTime;   ///< End time of the benchmark.
    size_t frameCount; ///< Total number of frames rendered during the benchmark period.

private:
    friend class BenchmarkEngine;
    std::map<std::string, std::string> staticInfo;      ///< Stores static system metadata.
    std::map<std::string, std::string> toolInfo;        ///< Stores static system metadata.
    std::map<std::string, MetricData> collectedMetrics; ///< Maps each metric name to its data.
    std::atomic<bool> collecting;                       ///< Status of the collection process.
    std::thread collectionThread;                       ///< Background thread for metrics collection.
    mutable std::mutex metricsMutex;                    ///< Synchronizes access to metric data.
    cJSON *runtimeReport;                               ///< JSON object representing runtime metrics.
    double combinedScore;                               ///< Accumulated score across all benchmark tasks.
    /**
     * Generates a JSON report from collected metrics and writes it to the specified file.
     *
     * @param filePath The output path for the JSON report.
     * @param tasks The number of tasks.
     * @return A pointer to the created JSON structure.
     */
    cJSON *createJSONReport(const std::string &filePath, int tasks) const;

    /**
     * Compiles collected runtime metrics for a specific benchmark task into a report structure.
     *
     * @param taskName The name of the benchmark task for which the report is generated.
     */
    void createBenchmarkReport(const std::string &taskName);
};

#endif // VALYRIA_METRICSCOLLECTOR_H
