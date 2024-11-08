#include "HTMLReportGenerator.h"

#include <algorithm>
#include <string>

HTMLReportGenerator::HTMLReportGenerator(const cJSON *jsonData, const std::string &filePath)
    : jsonData(jsonData), filePath(filePath) {}

void HTMLReportGenerator::generateReport() {
    std::ofstream file(filePath);
    file << generateHeader();
    file << generateEnvironmentSection(cJSON_GetObjectItem(jsonData, "Environment"));
    file << generateMetricsTable(cJSON_GetObjectItem(jsonData, "Benchmark Results"));
    file << generateSparklineJS();
    file << generateFooter();
    file.close();
}

std::string HTMLReportGenerator::generateHeader() const {
    return R"(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Valyria Benchmark Report</title>
    <style>
        body { font-family: Arial, sans-serif; padding: 20px; background-color: #f4f4f9; color: #333; }
        h1, h2 { color: #333; }
        table { width: 100%; border-collapse: collapse; margin-bottom: 20px; }
        th, td { padding: 10px; border-bottom: 1px solid #ddd; text-align: left; }
        th { background-color: #3a6ea5; color: #fff; }
        tr:nth-child(even) { background-color: #f9f9f9; }
        tr:nth-child(odd) { background-color: #e7eef8; }
        .align-right { text-align: right; }
        .sparkline { width: 300px; height: 20px; }
    </style>
    <script src="https://cdnjs.cloudflare.com/ajax/libs/jquery/3.6.0/jquery.min.js"></script>
    <script src="https://cdnjs.cloudflare.com/ajax/libs/jquery-sparklines/2.1.2/jquery.sparkline.min.js"></script>
</head>
<body>
)";
}

std::string HTMLReportGenerator::generateEnvironmentSection(const cJSON *envData) const {
    std::string html = "<h2>Environment</h2><ul>";
    cJSON *item = nullptr;
    cJSON_ArrayForEach(item, envData) {
        html += "<li><strong>" + std::string(item->string) + ":</strong> " + item->valuestring + "</li>";
    }
    html += "</ul>";
    return html;
}

std::string HTMLReportGenerator::generateMetricsTable(const cJSON *metricsData) const {
    std::string html = "<h2>Benchmark Results</h2>\n";
    cJSON *benchmark = nullptr;

    cJSON_ArrayForEach(benchmark, metricsData) {
        html += "  <h3>" + std::string(benchmark->string) + "</h3>\n";
        html += "  <table>\n    <thead>\n      <tr>\n"
                "        <th>Metric Name</th>\n"
                "        <th class='align-right'>Min</th>\n"
                "        <th class='align-right'>Max</th>\n"
                "        <th class='align-right'>Avg</th>\n"
                "        <th class='align-right'>Std Dev</th>\n"
                "        <th>Chart</th>\n"
                "      </tr>\n    </thead>\n    <tbody>\n";

        cJSON *metric = nullptr;
        int rowIndex = 0;
        cJSON_ArrayForEach(metric, benchmark) {
            std::string rowClass = (rowIndex++ % 2 == 0) ? "even" : "odd";
            html += "      <tr class='" + rowClass + "'>\n";
            html += "        <td>" + std::string(metric->string) + "</td>\n";

            for (const auto &stat : {"minimum", "maximum", "average", "std_dev"}) {
                cJSON *value = cJSON_GetObjectItem(metric, stat);
                html += "        <td class='align-right'>" + std::string(value ? value->valuestring : "N/A") + "</td>\n";
            }

            html += "        <td><span class='sparkline' id='sl_" + std::string(benchmark->string) +
                    "_" + formatName(metric->string) + "'></span></td>\n";
            html += "      </tr>\n";
        }

        html += "    </tbody>\n  </table>\n";
    }
    return html;
}

std::string HTMLReportGenerator::generateSparklineJS() const {
    std::string script = R"(
<script>
    $(document).ready(function() {)";

    cJSON *benchmarks = cJSON_GetObjectItem(jsonData, "Benchmark Results");
    if (benchmarks) {
        cJSON *benchmark = nullptr;
        cJSON_ArrayForEach(benchmark, benchmarks) {
            cJSON *metric = nullptr;
            cJSON_ArrayForEach(metric, benchmark) {
                cJSON *values = cJSON_GetObjectItem(metric, "values");
                if (values && cJSON_GetArraySize(values) > 0) {
                    std::string valuesArray = "[";
                    for (int i = 0; i < cJSON_GetArraySize(values); ++i) {
                        if (i > 0)
                            valuesArray += ", ";
                        valuesArray += cJSON_GetArrayItem(values, i)->valuestring;
                    }
                    valuesArray += "]";

                    std::string elementId = "sl_" + std::string(benchmark->string) + "_" + formatName(metric->string);
                    script += "$('#" + elementId + "').sparkline(" + valuesArray +
                              ", {type: 'line', width: '300px', height: '20px'});\n";
                }
            }
        }
    }

    script += R"(
    });
</script>)";
    return script;
}

std::string HTMLReportGenerator::generateFooter() const { return "</body></html>"; }

std::string HTMLReportGenerator::formatName(const std::string &name) const {
    std::string formattedName = name;
    std::transform(formattedName.begin(), formattedName.end(), formattedName.begin(), ::tolower);
    std::replace(formattedName.begin(), formattedName.end(), ' ', '_');
    formattedName.erase(std::remove(formattedName.begin(), formattedName.end(), '('), formattedName.end());
    formattedName.erase(std::remove(formattedName.begin(), formattedName.end(), ')'), formattedName.end());
    formattedName.erase(std::remove(formattedName.begin(), formattedName.end(), '.'), formattedName.end());
    return formattedName;
}