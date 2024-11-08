#include "tasks/Triangle.h"
#include "Logger.h"
#include "ShaderManager.h"

#include <cmath>

Triangle::Triangle(const std::string &taskName)
    : RenderTask(taskName), rotationUniform(0), posAttrib(0), colorAttrib(0), rotationAngle(0.0f) {}

bool Triangle::setup() {
    if (!ShaderManager::getInstance().createShaderProgram("TriangleShader", "triangle.vert", "triangle.frag")) {
        logError("Failed to create the TriangleShader program.");
        return false;
    }

    GLuint programID = ShaderManager::getInstance().getShaderProgram("TriangleShader")->getProgramID();
    posAttrib = glGetAttribLocation(programID, "pos");
    colorAttrib = glGetAttribLocation(programID, "color");
    rotationUniform = glGetUniformLocation(programID, "rotation");

   if (posAttrib == -1 || colorAttrib == -1 || rotationUniform == -1) {
        logError("Triangle: Failed to retrieve one or more uniform locations.");
        return false;
    }

    logDebug("Triangle setup OK");
    return true;
}

void Triangle::teardown() {
    if (!ShaderManager::getInstance().removeShaderProgram("TriangleShader")) {
        logError("Failed to remove the CubeShader program.");
    }
}

void Triangle::render(int width, int height) {
    ShaderManager::getInstance().useShaderProgram("TriangleShader");

    static const GLfloat verts[3][2] = {
        {-0.5f, -0.5f},
        {0.5f, -0.5f},
        {0.0f, 0.5f}
    };
    static const GLfloat colors[3][4] = {
        {1.0f, 0.0f, 0.0f, 1.0f},
        {0.0f, 1.0f, 0.0f, 1.0f},
        {0.0f, 0.0f, 1.0f, 1.0f}
    };

    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT);

    GLfloat rotation[4][4] = {
        {static_cast<GLfloat>(cos(rotationAngle)), 0.0f, static_cast<GLfloat>(sin(rotationAngle)), 0.0f},
        {0.0f, 1.0f, 0.0f, 0.0f},
        {static_cast<GLfloat>(-sin(rotationAngle)), 0.0f, static_cast<GLfloat>(cos(rotationAngle)), 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f}
    };

    glUniformMatrix4fv(rotationUniform, 1, GL_FALSE, (GLfloat *)rotation);

    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, verts);
    glVertexAttribPointer(colorAttrib, 4, GL_FLOAT, GL_FALSE, 0, colors);
    glEnableVertexAttribArray(posAttrib);
    glEnableVertexAttribArray(colorAttrib);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    glDisableVertexAttribArray(posAttrib);
    glDisableVertexAttribArray(colorAttrib);
}

void Triangle::update(float elapsedTime, float deltaTime) {
    rotationAngle += (deltaTime / 1000.0f) * 0.5f;
    if (rotationAngle > 2.0f * M_PI) {
        rotationAngle -= 2.0f * M_PI;
    }
}
