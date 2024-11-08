#include "tasks/Cube.h"
#include "Logger.h"
#include "ShaderManager.h"

Cube::Cube(const std::string &taskName, int AA, int maxSteps)
    : RenderTask(taskName), AA(AA), maxSteps(maxSteps), elapsedTime(0.0f), iTimeLocation(-1), iResolutionLocation(-1),
      AALocation(-1), maxStepsLocation(-1) {}

bool Cube::setup() {
    GLfloat quadVertices[] = {
        -1.0f, -1.0f, // Bottom left
        1.0f,  -1.0f, // Bottom right
        -1.0f, 1.0f,  // Top left
        1.0f,  1.0f   // Top right
    };

    glGenBuffers(1, &quadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    if (!ShaderManager::getInstance().createShaderProgram("CubeShader", "quad.vert", "cube.frag")) {
        logError("Failed to create the CubeShader program.");
        return false;
    }

    GLuint programID = ShaderManager::getInstance().getShaderProgram("CubeShader")->getProgramID();

    iTimeLocation = glGetUniformLocation(programID, "iTime");
    iResolutionLocation = glGetUniformLocation(programID, "iResolution");
    AALocation = glGetUniformLocation(programID, "AA");
    maxStepsLocation = glGetUniformLocation(programID, "maxSteps");

    if (iTimeLocation == -1 || iResolutionLocation == -1 || AALocation == -1 || maxStepsLocation == -1) {
        logError("Cube: Failed to retrieve one or more uniform locations.");
        return false;
    }

    logDebug("Cube setup OK");
    return true;
}

void Cube::teardown() {
    glDeleteBuffers(1, &quadVBO);

    if (!ShaderManager::getInstance().removeShaderProgram("CubeShader")) {
        logError("Failed to remove the CubeShader program.");
    }
}

void Cube::render(int width, int height) {
    ShaderManager::getInstance().useShaderProgram("CubeShader");

    glUniform1f(iTimeLocation, elapsedTime);
    glUniform2f(iResolutionLocation, width, height);
    glUniform1i(AALocation, AA);
    glUniform1i(maxStepsLocation, maxSteps);

    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glDisableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Cube::update(float elapsed, float deltaTime) { elapsedTime = elapsed / 1000.0; }
