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

#ifndef VALYRIA_SHADERPROGRAM_H
#define VALYRIA_SHADERPROGRAM_H

#include "Shader.h"
#include <GLES2/gl2.h>
#include <memory>
#include <vector>

/**
 * A class that represents an OpenGL ES shader program, allowing the attachment of
 * shaders, linking, and usage in rendering.
 */
class ShaderProgram {
public:
    /**
     * Constructs a ShaderProgram and creates an OpenGL ES program object.
     */
    ShaderProgram();

    /**
     * Destructor that releases OpenGL resources associated with the shader program.
     */
    ~ShaderProgram();

    /**
     * Attaches a compiled shader to the shader program.
     *
     * @param shader A shared pointer to the Shader object to be attached.
     * @return True if the shader was successfully attached, false otherwise.
     */
    bool attachShader(std::shared_ptr<Shader> shader);

    /**
     * Links the attached shaders into an executable shader program.
     *
     * @return True if the program was linked successfully, false otherwise.
     */
    bool link();

    /**
     * Uses the shader program for subsequent rendering operations.
     */
    void use() const;

    /**
     * Gets the OpenGL program ID of the shader program.
     *
     * @return The OpenGL program ID.
     */
    GLuint getProgramID() const;

    /**
     * Releases the OpenGL resources associated with the shader program.
     */
    void release();

private:
    GLuint programID;                                     ///< The OpenGL ID of the shader program.
    std::vector<std::shared_ptr<Shader>> attachedShaders; ///< List of shaders attached to the program.

    /**
     * Logs any errors encountered during the linking of the shader program.
     */
    void logProgramError() const;
};

#endif // VALYRIA_SHADERPROGRAM_H
