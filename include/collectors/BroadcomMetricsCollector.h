#ifndef BROADCOM_METRICS_COLLECTOR_H
#define BROADCOM_METRICS_COLLECTOR_H

#include "MetricsCollector.h"

class BroadcomMetricsCollector : public MetricsCollector {
public:
    BroadcomMetricsCollector() = default;
    ~BroadcomMetricsCollector() override = default;
    void collectPlatformMetrics() override;

private:
    void parseAvailableGPULoadFiles();
    void parseGPULoadFile(const std::string &filePath);
    void parseCoreFile(const std::string &filePath);
};

#endif // BROADCOM_METRICS_COLLECTOR_H
