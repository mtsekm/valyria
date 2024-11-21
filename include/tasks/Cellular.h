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
