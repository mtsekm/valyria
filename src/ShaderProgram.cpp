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

#include "ShaderProgram.h"
#include "Logger.h"
#include <vector>

ShaderProgram::ShaderProgram() {
    programID = glCreateProgram();
    if (programID == 0) {
        logError("Failed to create shader program.");
    } else {
        logTrace("Shader program created successfully. Program ID: " + std::to_string(programID));
    }
}

ShaderProgram::~ShaderProgram() { release(); }

bool ShaderProgram::attachShader(std::shared_ptr<Shader> shader) {
    if (shader && shader->getShaderID() != 0) {
        glAttachShader(programID, shader->getShaderID());
        attachedShaders.push_back(shader);
        logTrace("Shader attached to program ID: " + std::to_string(programID));
        return true;
    } else {
        logError("Failed to attach shader. Shader ID is invalid.");
        return false;
    }
}

bool ShaderProgram::link() {
    glLinkProgram(programID);

    GLint success;
    glGetProgramiv(programID, GL_LINK_STATUS, &success);
    if (!success) {
        logProgramError();
        release();
        return false;
    }

    for (const auto &shader : attachedShaders) {
        glDetachShader(programID, shader->getShaderID());
    }
    attachedShaders.clear();

    logTrace("Shader program linked successfully. Program ID: " + std::to_string(programID));
    return true;
}

void ShaderProgram::use() const {
    if (programID != 0) {
        glUseProgram(programID);
        logTrace("Shader program in use. Program ID: " + std::to_string(programID));
    } else {
        logError("Attempted to use an invalid shader program.");
    }
}

GLuint ShaderProgram::getProgramID() const { return programID; }

void ShaderProgram::release() {
    if (programID != 0) {
        glDeleteProgram(programID);
        logTrace("Shader program deleted. Program ID: " + std::to_string(programID));
        programID = 0;
    }
    attachedShaders.clear();
}

void ShaderProgram::logProgramError() const {
    GLint maxLength = 0;
    glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &maxLength);

    std::vector<GLchar> errorLog(maxLength);
    glGetProgramInfoLog(programID, maxLength, &maxLength, &errorLog[0]);
    logError("Program linking error: " + std::string(&errorLog[0]));
}
