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
