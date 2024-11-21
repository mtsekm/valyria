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

#include "BenchmarkEngine.h"
#include "ConfigurationManager.h"
#include "Logger.h"
#include "ShaderManager.h"

#ifdef PLATFORM_AMLOGIC
#include "collectors/AmlogicMetricsCollector.h"
#elif defined(PLATFORM_BROADCOM)
#include "collectors/BroadcomMetricsCollector.h"
#elif defined(PLATFORM_REALTEK)
#include "collectors/RealtekMetricsCollector.h"
#endif

#include "tasks/Cellular.h"
#include "tasks/Clear.h"
#include "tasks/Cube.h"
#include "tasks/Triangle.h"

#include <chrono>
#include <thread>

BenchmarkEngine::BenchmarkEngine() : graphicsContext(std::make_unique<GraphicsContext>()), metricsCollector(nullptr) {}

BenchmarkEngine::~BenchmarkEngine() { cleanup(); }

bool BenchmarkEngine::initialize() {
    if (!graphicsContext->initialize()) {
        logError("Failed to initialize graphics context.");
        return false;
    }

#ifdef PLATFORM_AMLOGIC
    logInfo("Using Amlogic metrics collector.");
    metricsCollector = std::make_unique<AmlogicMetricsCollector>();
#elif defined(PLATFORM_BROADCOM)
    logInfo("Using Broadcom metrics collector.");
    metricsCollector = std::make_unique<BroadcomMetricsCollector>();
#elif defined(PLATFORM_REALTEK)
    logInfo("Using Realtek metrics collector.");
    metricsCollector = std::make_unique<RealtekMetricsCollector>();
#else
    metricsCollector = std::make_unique<MetricsCollector>();
    logWarn("No platform-specific MetricsCollector will be used.");
#endif

    if (!metricsCollector) {
        logError("Failed to create MetricsCollector instance.");
        return false;
    }

    metricsCollector->collectStaticSystemInfo();

    createRenderTasks();

    logDebug("BenchmarkEngine initialized successfully.");
    return true;
}

void BenchmarkEngine::addTask(std::shared_ptr<RenderTask> task) {
    if (task) {
        tasks.push_back(task);
        logTrace("RenderTask '" + task->getName() + "' added to the BenchmarkEngine.");
    } else {
        logWarn("Attempted to add a null RenderTask.");
    }
}

void BenchmarkEngine::listAvailableTasks() const {
    if (tasks.empty()) {
        return;
    }

    for (const auto &task : tasks) {
        if (task) {
            std::cout << "- " << task->getName() << std::endl;
        }
    }
}

void BenchmarkEngine::runBenchmark(std::shared_ptr<RenderTask> task, int durationInSeconds) {
    if (!graphicsContext || !metricsCollector) {
        logError("BenchmarkEngine is not properly initialized.");
        return;
    }

    ConfigurationManager &configManager = ConfigurationManager::getInstance();
    int targetFrameRate = std::stoi(configManager.getValue("target_frame_rate"));

    if (!task->setup()) {
        logError("Failed to setup RenderTask: " + task->getName());
        task->teardown();
        return;
    }

    metricsCollector->clearMetrics();
    metricsCollector->startCollection();

    auto startTime = std::chrono::steady_clock::now();
    auto endTime = startTime + std::chrono::seconds(durationInSeconds);
    auto previousFrameTime = startTime;
    auto frameStartTime = startTime;
    auto frameEndTime = startTime;

    float elapsedTime = 0.0f;
    float deltaTime = 0.0f;
    int frameTimeMs = 1000 / targetFrameRate;
    int elapsedTimeMs = 0;
    int sleepTimeMs = 0;

    logInfo("Running '" + task->getName() + "' for " + std::to_string(durationInSeconds) + " seconds.");

    // main loop
    while (std::chrono::steady_clock::now() < endTime) {
        frameStartTime = std::chrono::steady_clock::now();

        elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(frameStartTime - startTime).count();
        deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(frameStartTime - previousFrameTime).count();
        previousFrameTime = frameStartTime;

        task->update(elapsedTime, deltaTime);
        task->render(graphicsContext->getWidth(), graphicsContext->getHeight());

        graphicsContext->updateDisplay();
        metricsCollector->incrementFrameCount();

        if (targetFrameRate > 0) {
            frameEndTime = std::chrono::steady_clock::now();
            elapsedTimeMs =
                std::chrono::duration_cast<std::chrono::milliseconds>(frameEndTime - frameStartTime).count();
            sleepTimeMs = frameTimeMs - elapsedTimeMs;

            if (sleepTimeMs > 0) {
                std::this_thread::sleep_for(std::chrono::milliseconds(sleepTimeMs));
            }
        }
    }

    metricsCollector->stopCollection();
    task->teardown();
    logInfo("Benchmark run completed.");
    metricsCollector->createBenchmarkReport(task->getName());
}

void BenchmarkEngine::runBenchmarks() {
    ConfigurationManager &configManager = ConfigurationManager::getInstance();
    int benchmarkDuration = std::stoi(configManager.getValue("benchmark_duration"));
    for (const auto &task : tasks) {
        if (task) {
            runBenchmark(task, benchmarkDuration);
        }
    }

    metricsCollector->createReport(tasks.size());
}

void BenchmarkEngine::cleanup() {
    if (graphicsContext) {
        graphicsContext->cleanup();
    }
    graphicsContext.reset();
    metricsCollector.reset();
    logTrace("BenchmarkEngine resources have been released.");
}

void BenchmarkEngine::createRenderTasks() {
    logTrace("Creating RenderTasks.");

    std::shared_ptr<RenderTask> clearTask = std::make_shared<ClearTask>("Clear");
    addTask(clearTask);

    std::shared_ptr<RenderTask> triangleTask = std::make_shared<Triangle>("Triangle");
    addTask(triangleTask);

    std::shared_ptr<RenderTask> cellularTask = std::make_shared<Cellular>("Cellular");
    addTask(cellularTask);

    std::shared_ptr<RenderTask> cubeTask3 = std::make_shared<Cube>("Cube-AA1", 1);
    addTask(cubeTask3);

    std::shared_ptr<RenderTask> cubeTask4 = std::make_shared<Cube>("Cube-AA2", 2);
    addTask(cubeTask4);
}
