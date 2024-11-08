#ifndef VALYRIA_TRIANGLE_H
#define VALYRIA_TRIANGLE_H

#include "GraphicsContext.h"
#include "RenderTask.h"

#include <GLES2/gl2.h>

/**
 * A RenderTask to draw a rotating colored triangle on the screen.
 */
class Triangle : public RenderTask {
public:
    Triangle(const std::string &taskName);

    bool setup() override;
    void teardown() override;

    void render(int width, int height) override;
    void update(float elapsedTime, float deltaTime) override;

private:
    GLuint rotationUniform;
    GLint posAttrib, colorAttrib;
    float rotationAngle;
};

#endif // VALYRIA_TRIANGLE_H
