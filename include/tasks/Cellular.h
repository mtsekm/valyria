#ifndef VALYRIA_CELLULAR_H
#define VALYRIA_CELLULAR_H

#include "GraphicsContext.h"
#include "RenderTask.h"

#include <GLES2/gl2.h>

/**
 * A RenderTask that draws a dynamic cellular pattern using a GLSL shader.
 *
 * Original shader: "Cellular" - A colony of single-cell organisms.
 * Source: https://www.shadertoy.com/view/Xs2GDd
 */
class Cellular : public RenderTask {
public:
    Cellular(const std::string &taskName, bool enableFBM = false);

    bool setup() override;
    void teardown() override;

    void render(int width, int height) override;
    void update(float elapsedTime, float deltaTime) override;

private:
    bool enableFBM;
    float elapsedTime;
    GLuint quadVBO;

    GLint iTimeLocation;
    GLint iResolutionLocation;
    GLint enableFBMLocation;
};

#endif // VALYRIA_CELLULAR_H
