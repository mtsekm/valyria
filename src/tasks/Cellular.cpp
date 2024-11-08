#include "tasks/Cellular.h"
#include "Logger.h"
#include "ShaderManager.h"

Cellular::Cellular(const std::string &taskName, bool enableFBM)
    : RenderTask(taskName), enableFBM(enableFBM), elapsedTime(0.0f), iTimeLocation(-1), iResolutionLocation(-1), enableFBMLocation(-1) {}

bool Cellular::setup() {
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

    if (!ShaderManager::getInstance().createShaderProgram("CellularShader", "quad.vert", "cellular.frag")) {
        logError("Failed to create the CellularShader program.");
        return false;
    }

    GLuint programID = ShaderManager::getInstance().getShaderProgram("CellularShader")->getProgramID();

    iTimeLocation = glGetUniformLocation(programID, "iTime");
    iResolutionLocation = glGetUniformLocation(programID, "iResolution");
    enableFBMLocation = glGetUniformLocation(programID, "enableFBM");

    if (iTimeLocation == -1 || iResolutionLocation == -1 || enableFBMLocation == -1) {
        logError("Cellular: Failed to retrieve one or more uniform locations.");
        return false;
    }

    logDebug("Cellular setup OK");
    return true;
}

void Cellular::teardown() {
    glDeleteBuffers(1, &quadVBO);

    if (!ShaderManager::getInstance().removeShaderProgram("CellularShader")) {
        logError("Failed to remove the CellularShader program.");
    }
}

void Cellular::render(int width, int height) {
    ShaderManager::getInstance().useShaderProgram("CellularShader");

    glUniform1f(iTimeLocation, elapsedTime);
    glUniform2f(iResolutionLocation, width, height);
    glUniform1i(enableFBMLocation, enableFBM ? 1 : 0);

    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glDisableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Cellular::update(float elapsed, float deltaTime) { elapsedTime = elapsed / 1000.0; }
