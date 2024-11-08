#ifndef VALYRIA_BASICSCENE_H
#define VALYRIA_BASICSCENE_H

#include "GraphicsContext.h"
#include "RenderTask.h"

#include <GLES2/gl2.h>

/**
 * A RenderTask that simulates a 3D rotating cube using GLSL shaders.
 * 
 * Original shader: Box Frame - Distance 3D
 * Source: https://www.shadertoy.com/view/3ljcRh
 */
class Cube : public RenderTask {
public:
    explicit Cube(const std::string &taskName, int AA = 1, int maxSteps = 128);
    ~Cube() override = default;

    bool setup() override;
    void teardown() override;

    void render(int width, int height) override;
    void update(float elapsedTime, float deltaTime) override;

private:
    int AA;
    int maxSteps;
    float elapsedTime;
    GLuint quadVBO;

    GLint iTimeLocation;
    GLint iResolutionLocation;
    GLint AALocation;
    GLint maxStepsLocation;
};

#endif // VALYRIA_BASICSCENE_H
