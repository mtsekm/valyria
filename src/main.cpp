#include "BenchmarkEngine.h"
#include "ConfigurationManager.h"
#include "Logger.h"

int main(int argc, char *argv[]) {

    try {
        logInfo("Valyria version: " + std::string(PROJECT_VERSION));

        ConfigurationManager &configManager = ConfigurationManager::getInstance();
        configManager.setOption("benchmark_duration", "30", "The duration for running each render task in seconds.");
        configManager.setOption("log_level", "INFO", "Log level");
        configManager.setOption("direct_mode", "false", "Whether to use Essos direct mode or run as a wayland client.");
        configManager.setOption("output_dir", "/tmp", "Directory to save results in.");
        configManager.setOption("sampling_rate", "1000", "The sampling rate for metrics collection in milliseconds.");
        configManager.setOption("target_frame_rate", "60", "Specifies the target frame rate for rendering.");
        configManager.setOption("window_width", "0", "Width of the application window. 0 for fullscreen.");
        configManager.setOption("window_height", "0", "Height of the application window. 0 for fullscreen.");

        if (configManager.parseCommandLineArguments(argc, argv)) {
            return EXIT_SUCCESS;
        }

        std::string logLevelStr = configManager.getValue("log_level");
        LoggerConfig::setLogLevel(stringToLogLevel(logLevelStr));

        BenchmarkEngine benchmarkEngine;
        if (!benchmarkEngine.initialize()) {
            logError("Failed to initialize the benchmark engine.");
            return EXIT_FAILURE;
        }

        benchmarkEngine.runBenchmarks();
    } catch (const std::exception &e) {
        logError(std::string("An error occurred: ") + e.what());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
