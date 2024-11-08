#ifndef VALYRIA_CLEAR_H
#define VALYRIA_CLEAR_H

#include "GraphicsContext.h"
#include "RenderTask.h"

#include <GLES2/gl2.h>

/**
 * A RenderTask that clears the screen.
 */
class ClearTask : public RenderTask {
public:
    ClearTask(const std::string &taskName);

    bool setup() override;
    void teardown() override;

    void render(int width, int height) override;
    void update(float elapsedTime, float deltaTime) override;
};

#endif // VALYRIA_CLEAR_H
