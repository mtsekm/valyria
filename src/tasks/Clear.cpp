/*
* If not stated otherwise in this file or this component's LICENSE file the
* following copyright and licenses apply:
*
* Copyright 2024 Sky UK
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

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
