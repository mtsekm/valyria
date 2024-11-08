#ifndef VALYRIA_BENCHMARKENGINE_H
#define VALYRIA_BENCHMARKENGINE_H

#include "GraphicsContext.h"
#include "MetricsCollector.h"
#include "RenderTask.h"

#include <memory>
#include <vector>

/**
 * The BenchmarkEngine class handles the initialization, execution, and cleanup of
 * benchmarks involving one or more RenderTask objects.
 */
class BenchmarkEngine {
public:
    /**
     * Constructs a BenchmarkEngine instance.
     */
    BenchmarkEngine();

    /**
     * Destructor for BenchmarkEngine.
     */
    ~BenchmarkEngine();

    /**
     * Initializes the BenchmarkEngine.
     *
     * @return True if initialization was successful; false otherwise.
     */
    bool initialize();

    /**
     * Runs the benchmarks on multiple RenderTasks in sequence.
     */
    void runBenchmarks();

    /**
     * Lists the names of all available RenderTasks.
     */
    void listAvailableTasks() const;

private:
    std::unique_ptr<GraphicsContext> graphicsContext;   ///< The graphics context for rendering.
    std::unique_ptr<MetricsCollector> metricsCollector; ///< The metrics collector for gathering performance data.
    std::vector<std::shared_ptr<RenderTask>> tasks;     ///< A list of tasks to be executed during benchmarking.

    /**
     * Creates instances of RenderTask objects to be benchmarked.
     */
    void createRenderTasks();

    /**
     * Runs the benchmark with a single RenderTask for a specified duration.
     *
     * @param task A shared pointer to the RenderTask to be benchmarked.
     * @param durationInSeconds The duration in seconds for which the benchmark should run.
     */
    void runBenchmark(std::shared_ptr<RenderTask> task, int durationInSeconds);

    /**
     * Cleans up resources used by the BenchmarkEngine.
     */
    void cleanup();

    /**
     * Adds a RenderTask to the task list managed by the BenchmarkEngine.
     *
     * @param task A shared pointer to the RenderTask to be added.
     */
    void addTask(std::shared_ptr<RenderTask> task);
};

#endif // VALYRIA_BENCHMARKENGINE_H
