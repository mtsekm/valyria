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

#include "HTMLReportGenerator.h"
#include "Logger.h"

#include <algorithm>
#include <string>

HTMLReportGenerator::HTMLReportGenerator(const cJSON *jsonData, const std::string &filePath)
    : jsonData(jsonData), filePath(filePath) {}

void HTMLReportGenerator::generateReport() {
    logDebug("Generating the HTML report");
    std::ofstream file(filePath);
    cJSON *environmentData = cJSON_GetObjectItem(jsonData, "Environment");
    if (!environmentData) {
        logError("Environment data not found in JSON.");
        return;
    }
    cJSON *deviceNameItem = cJSON_GetObjectItem(environmentData, "Device Name");
    if (!deviceNameItem) {
        logError("Device Name not found in Environment data.");
        return;
    }
    std::string deviceName = deviceNameItem->valuestring;
    file << generateHeader(deviceName);
    file << generateEnvironmentSection(cJSON_GetObjectItem(jsonData, "Environment"));
    file << generateToolConfigSection(cJSON_GetObjectItem(jsonData, "Configuration"));
    file << generateMetricsTabs(cJSON_GetObjectItem(jsonData, "Benchmark Results"));
    file << generateSparklineJS();
    file << generateFooter();
    file.close();
    logDebug("HTML report is ready.");
}

std::string HTMLReportGenerator::generateHeader(const std::string &deviceName) const {
    logDebug("Generating HTML header");
    return R"(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>)" +
           deviceName + R"( - Valyria Benchmark Report</title>
    <link href="https://maxcdn.bootstrapcdn.com/bootstrap/4.5.2/css/bootstrap.min.css" rel="stylesheet">
    <style>
        body { font-family: Arial, sans-serif; padding: 20px; background-color: #f4f4f9; color: #333; }
        h1, h2 { color: #333; }
        .table-striped tbody tr:nth-of-type(odd) { background-color: rgba(0,0,0,.05); }
        .sparkline { width: 200px; height: 20px; }
        .tooltip-custom{position:absolute;left:0;top:0;visibility:hidden;background:transparent;color:black;font:10px Arial,sans-serif;text-align:left;white-space:nowrap;padding:0;border:none;z-index:10000}
        .tooltip-custom .jqsfield{color:black;font:10px Arial,sans-serif;text-align:left}
    </style>
    <script src="https://cdnjs.cloudflare.com/ajax/libs/jquery/3.6.0/jquery.min.js"></script>
    <script src="https://cdnjs.cloudflare.com/ajax/libs/jquery-sparklines/2.1.2/jquery.sparkline.min.js"></script>
</head>
<body>
    <div class="container-fluid">
        <div class="row">
)";
}

#include <sstream>

std::string HTMLReportGenerator::generateEnvironmentSection(const cJSON *envData) const {
    logDebug("Generating HTML environment section");
    std::string html = "<div class='col-md-6'><ul class='list-group'>";
    cJSON *item = nullptr;

    // Iterate through all environment items
    cJSON_ArrayForEach(item, envData) {
        std::string key = item->string;
        if (key == "OpenGL Extensions") {
            html += R"(
                <li class='list-group-item'>
                    <strong>OpenGL Extensions:</strong>
                    <button class="btn btn-link" type="button" data-toggle="collapse" data-target="#openglExtensions" aria-expanded="false" aria-controls="openglExtensions">
                        Show Extensions
                    </button>
                    <div class="collapse" id="openglExtensions" style="font-size: 0.9em;">
            )";

            // Display extensions in a compact format
            std::istringstream extensionsStream(item->valuestring);
            std::string extension;
            while (extensionsStream >> extension) {
                html += "<span style='display: inline-block; margin-right: 5px;'>" + extension + "</span>";
            }

            html += R"(
                    </div>
                </li>
            )";
        } else {
            html += "<li class='list-group-item'><strong>" + key + ":</strong> " + item->valuestring + "</li>";
        }
    }

    html += "</ul></div>";
    return html;
}

std::string HTMLReportGenerator::generateToolConfigSection(const cJSON *toolData) const {
    logDebug("Generating HTML configuration section");
    std::string html = "<div class='col-md-6'><ul class='list-group'>";
    cJSON *item = nullptr;
    cJSON_ArrayForEach(item, toolData) {
        html += "<li class='list-group-item'><strong>" + std::string(item->string) + ":</strong> " + item->valuestring +
                "</li>";
    }
    html += "</ul>";

    cJSON *scoreItem = cJSON_GetObjectItem(jsonData, "Score");
    if (scoreItem && cJSON_IsString(scoreItem)) {
        html += R"(
                <div class="alert alert-primary mt-3" role="alert">
                    <h3 class="alert-heading">Score: )" +
                std::string(scoreItem->valuestring) + R"(</h3>
                </div>
        )";
    } else {
        logWarn("Combined Score is missing or not a valid string.");
    }

    html += "</div></div>"; // closing row
    return html;
}

std::string HTMLReportGenerator::generateMetricsTabs(const cJSON *metricsData) const {
    logDebug("Generating HTML metrics tabs section");
    std::string html;
    int tabIndex = 0;
    cJSON *benchmark = nullptr;

    html += R"(<div class="mt-4"><ul class="nav nav-tabs" id="benchmarkTabs" role="tablist">)";
    cJSON_ArrayForEach(benchmark, metricsData) {
        std::string benchmarkName = benchmark->string;
        std::string tabID = "tab_" + formatName(benchmarkName);

        html += "<li class='nav-item'>"
                "<a class='nav-link " +
                std::string(tabIndex == 0 ? "active" : "") + "' id='" + tabID + "-tab' data-toggle='tab' href='#" +
                tabID + "' role='tab'>" + benchmarkName + "</a></li>";
        tabIndex++;
    }
    html += "</ul><div class='tab-content' id='benchmarkTabContent'>";

    tabIndex = 0;
    cJSON_ArrayForEach(benchmark, metricsData) {
        std::string benchmarkName = benchmark->string;
        std::string tabID = "tab_" + formatName(benchmarkName);

        html += "<div class='tab-pane fade " + std::string(tabIndex == 0 ? "show active" : "") + "' id='" + tabID +
                "' role='tabpanel' aria-labelledby='" + tabID + "-tab'>";
        html += "<table class='table table-striped table-hover'><thead><tr>"
                "<th style='width:300px'>Metric Name</th>"
                "<th class='text-right' style='width:80px;'>Min</th>"
                "<th class='text-right' style='width:80px;'>Max</th>"
                "<th class='text-right' style='width:80px;'>Avg</th>"
                "<th class='text-right' style='width:80px;'>StdDev</th>"
                "<th>Chart</th>"
                "</tr></thead><tbody>";

        cJSON *metric = nullptr;
        cJSON_ArrayForEach(metric, benchmark) {
            html += "<tr><td>" + std::string(metric->string) + "</td>";
            for (const auto &stat : {"minimum", "maximum", "average", "std_dev"}) {
                cJSON *value = cJSON_GetObjectItem(metric, stat);
                html += "<td class='text-right' style='width:80px;'>" +
                        std::string(value ? value->valuestring : "N/A") + "</td>";
            }
            html += "<td><span class='sparkline' id='sl_" + formatName(benchmarkName) + "_" +
                    formatName(metric->string) + "'></span></td></tr>";
        }
        html += "</tbody></table></div>";
        tabIndex++;
    }
    html += "</div></div>";
    return html;
}

std::string HTMLReportGenerator::generateSparklineJS() const {
    logDebug("Generating HTML sparkline JS section");
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
                    std::string elementId = "sl_" + formatName(benchmark->string) + "_" + formatName(metric->string);
                    script += "$('#" + elementId + "').sparkline(" + valuesArray +
                              ", {type: 'line', width: '200px', height: '20px', tooltipFormat: '<span style=\"color: #000;\">{{y}}</span>', tooltipClassname: 'tooltip-custom'});\n";
                }
            }
        }
    }

    script += R"(
    });
</script>)";
    return script;
}

std::string HTMLReportGenerator::generateFooter() const {
    logDebug("Generating HTML footer section");
    return R"(
    </div> <!-- container -->
    <script src="https://maxcdn.bootstrapcdn.com/bootstrap/4.5.2/js/bootstrap.bundle.min.js"></script>
</body>
</html>)";
}

std::string HTMLReportGenerator::formatName(const std::string &name) const {
    std::string formattedName = name;
    std::transform(formattedName.begin(), formattedName.end(), formattedName.begin(), ::tolower);
    std::replace(formattedName.begin(), formattedName.end(), ' ', '_');
    formattedName.erase(std::remove(formattedName.begin(), formattedName.end(), '('), formattedName.end());
    formattedName.erase(std::remove(formattedName.begin(), formattedName.end(), ')'), formattedName.end());
    formattedName.erase(std::remove(formattedName.begin(), formattedName.end(), '.'), formattedName.end());
    return formattedName;
}