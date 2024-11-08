#include "tasks/Clear.h"
#include "Logger.h"
#include "ShaderManager.h"

ClearTask::ClearTask(const std::string &taskName) : RenderTask(taskName) {}

bool ClearTask::setup() {
    glClearColor(0.0f, 0.5f, 0.75f, 1.0f);
    return true;
}

void ClearTask::teardown() { glClearColor(0.0f, 0.0f, 0.0f, 1.0f); }

void ClearTask::render(int width, int height) { glClear(GL_COLOR_BUFFER_BIT); }

void ClearTask::update(float elapsed, float deltaTime) {}
