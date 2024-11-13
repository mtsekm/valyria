#ifndef VALYRIA_HTMLREPORTGEN_H
#define VALYRIA_HTMLREPORTGEN_H

#include <fstream>
#include <string>

#include <cjson/cJSON.h>

class HTMLReportGenerator {
public:
    HTMLReportGenerator(const cJSON *jsonData, const std::string &filePath);

    void generateReport();

private:
    std::string generateHeader(const std::string &deviceName) const;
    std::string generateEnvironmentSection(const cJSON *envData) const;
    std::string generateToolConfigSection(const cJSON *toolData) const;
    std::string generateMetricsTabs(const cJSON *metricsData) const;
    std::string generateSparklineJS() const;
    std::string generateFooter() const;
    std::string formatName(const std::string &name) const;

    const cJSON *jsonData;
    std::string filePath;
};

#endif // VALYRIA_HTMLREPORTGEN_H
