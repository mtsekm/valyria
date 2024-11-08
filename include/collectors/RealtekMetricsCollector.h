#ifndef REALTEK_METRICS_COLLECTOR_H
#define REALTEK_METRICS_COLLECTOR_H

#include "MetricsCollector.h"

class RealtekMetricsCollector : public MetricsCollector {
public:
    RealtekMetricsCollector() = default;
    ~RealtekMetricsCollector() override = default;
    void collectPlatformMetrics() override;
};

#endif // REALTEK_METRICS_COLLECTOR_H
