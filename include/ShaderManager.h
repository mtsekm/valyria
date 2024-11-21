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

#ifndef VALYRIA_SHADERMANAGER_H
#define VALYRIA_SHADERMANAGER_H

#include "Shader.h"
#include "ShaderProgram.h"
#include <memory>
#include <string>
#include <unordered_map>

/**
 * A singleton class responsible for managing multiple shader programs in a centralized manner.
 */
class ShaderManager {
public:
    /**
     * Retrieves the singleton instance of the ShaderManager.
     *
     * @return The singleton instance of ShaderManager.
     */
    static ShaderManager &getInstance();

    /**
     * Deleted copy constructor.
     */
    ShaderManager(const ShaderManager &) = delete;

    /**
     * Deleted copy assignment.
     */
    ShaderManager &operator=(const ShaderManager &) = delete;

    /**
     * Creates a new shader program and stores it by the specified name.
     *
     * This method compiles the provided vertex and fragment shader sources,
     * links them into a shader program, and stores the program using the
     * provided name. If a program with the same name already exists, creation is skipped.
     *
     * @param programName The name used to store and retrieve the shader program.
     * @param vertexSource The source code of the vertex shader.
     * @param fragmentSource The source code of the fragment shader.
     * @return True if the shader program was created and stored successfully, false otherwise.
     */
    bool createShaderProgram(const std::string &programName, const std::string &vertexSource,
                             const std::string &fragmentSource);

    /**
     * Removes a shader program by its name.
     *
     * @param programName The name of the shader program to remove.
     * @return True if the shader program was removed successfully, false if not found.
     */
    bool removeShaderProgram(const std::string &programName);

    /**
     * Retrieves a shader program by its name.
     *
     * @param programName The name of the shader program to retrieve.
     * @return A shared pointer to the ShaderProgram, or nullptr if not found.
     */
    std::shared_ptr<ShaderProgram> getShaderProgram(const std::string &programName) const;

    /**
     * Uses the specified shader program for rendering.
     *
     * @param programName The name of the shader program to activate.
     */
    void useShaderProgram(const std::string &programName);

private:
    /**
     * Private default constructor.
     */
    ShaderManager();

    /**
     * Default destructor for ShaderManager.
     */
    ~ShaderManager();

    std::unordered_map<std::string, std::shared_ptr<ShaderProgram>> shaderPrograms; ///< Stores shader programs by name.
    std::string currentProgram; ///< Tracks the currently active shader program.
};

#endif // VALYRIA_SHADERMANAGER_H
