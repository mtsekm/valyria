#include "RenderTask.h"
#include "Logger.h"

RenderTask::RenderTask(const std::string &taskName)
    : name(taskName) {
}

std::string RenderTask::getName() const { return name; }
