#ifndef VALYRIA_RENDERTASK_H
#define VALYRIA_RENDERTASK_H

#include <memory>
#include <string>

/**
 * A base class for defining and managing individual rendering tasks within a benchmark.
 */
class RenderTask {
public:
    /**
     * Constructs a `RenderTask` with a given name.
     *
     * @param taskName A unique name identifying the render task.
     */
    RenderTask(const std::string &taskName);

    /**
     * Destructor.
     */
    virtual ~RenderTask() = default;

    /**
     * Initializes resources required for the rendering task.
     *
     * @return True if the setup was successful; false otherwise.
     */
    virtual bool setup() { return true; };

    /**
     * Releases resources and performs cleanup.
     */
    virtual void teardown() {};

    /**
     * Retrieves the name of the render task.
     *
     * @return A string containing the name of the render task.
     */
    std::string getName() const;

    /**
     * Executes the rendering task.
     *
     * Derived classes must implement this pure virtual method to define the specific
     * rendering logic and draw calls for each frame.
     *
     * @param width The width of the render area in pixels.
     * @param height The height of the render area in pixels.
     */
    virtual void render(int width, int height) = 0;

    /**
     * Updates the task state, allowing for dynamic changes between frames.
     *
     * This method can be used to adjust properties based on time, enabling
     * animations or transformations over the course of the benchmark.
     *
     * @param elapsedTime The total elapsed time since the start of the benchmark, in milliseconds.
     * @param deltaTime The time difference since the last frame, in milliseconds.
     */
    virtual void update(float elapsedTime, float deltaTime) = 0;

protected:
    std::string name; ///< Name of the render task.
};

#endif // VALYRIA_RENDERTASK_H
