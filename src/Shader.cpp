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

#include "Shader.h"
#include "ConfigurationManager.h"
#include "Logger.h"

#include <fstream>
#include <sstream>
#include <vector>

Shader::Shader(GLenum type, const std::string &fileName) : type(type), shaderID(0), filename(fileName) {}

Shader::~Shader() { release(); }

bool Shader::compile() {
    logTrace("Compiling " + std::string((type == GL_VERTEX_SHADER ? "Vertex" : "Fragment")) +
             " shader using file: " + filename);
    shaderID = glCreateShader(type);
    if (shaderID == 0) {
        logError("Failed to create shader.");
        return false;
    }

    std::string source = loadShaderFromFile(filename);
    if (source.empty()) {
        glDeleteShader(shaderID);
        shaderID = 0;
        return false;
    }

    const char *src = source.c_str();
    glShaderSource(shaderID, 1, &src, nullptr);
    glCompileShader(shaderID);

    GLint success;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
    if (!success) {
        logShaderError();
        glDeleteShader(shaderID);
        shaderID = 0;
        return false;
    }

    logDebug(std::string((type == GL_VERTEX_SHADER ? "Vertex" : "Fragment")) +
             " shader compiled successfully from file - " + filename);
    return true;
}

GLuint Shader::getShaderID() const { return shaderID; }

void Shader::release() {
    if (shaderID != 0) {
        glDeleteShader(shaderID);
        shaderID = 0;
        logDebug(std::string((type == GL_VERTEX_SHADER ? "Vertex" : "Fragment")) + " shader deleted - " + filename);
    }
}

void Shader::logShaderError() const {
    GLint maxLength = 0;
    glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &maxLength);

    std::vector<GLchar> errorLog(maxLength);
    glGetShaderInfoLog(shaderID, maxLength, &maxLength, &errorLog[0]);
    logError(std::string((type == GL_VERTEX_SHADER ? "Vertex" : "Fragment")) +
             " shader compilation error: " + std::string(&errorLog[0]));
}

std::string Shader::loadShaderFromFile(const std::string &fileName) {
    std::string fullPath = ConfigurationManager::getInstance().getValue("asset_dir") + "/" + fileName;
    std::ifstream file(fullPath);

    if (!file.is_open()) {
        logError("Failed to open shader file: " + fullPath);
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();

    logTrace("Shader file loaded from: " + fullPath);
    return buffer.str();
}
