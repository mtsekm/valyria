#ifndef AMLOGIC_METRICS_COLLECTOR_H
#define AMLOGIC_METRICS_COLLECTOR_H

#include "MetricsCollector.h"

class AmlogicMetricsCollector : public MetricsCollector {
public:
    AmlogicMetricsCollector() = default;
    ~AmlogicMetricsCollector() override = default;
    void collectPlatformMetrics() override;
};

#endif // AMLOGIC_METRICS_COLLECTOR_H
