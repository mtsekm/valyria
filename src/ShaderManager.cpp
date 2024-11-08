#include "ShaderManager.h"
#include "Logger.h"

ShaderManager &ShaderManager::getInstance() {
    static ShaderManager instance;
    return instance;
}

ShaderManager::ShaderManager() {  }

ShaderManager::~ShaderManager() { }

bool ShaderManager::createShaderProgram(const std::string &programName, const std::string &vertexSource,
                                        const std::string &fragmentSource) {
    if (shaderPrograms.find(programName) != shaderPrograms.end()) {
        logWarn("Shader program '" + programName + "' already exists. Creation skipped.");
        return false;
    }

    logTrace("Creating shaders for '" + programName + "' program.");
    auto vertexShader = std::make_shared<Shader>(GL_VERTEX_SHADER, vertexSource);
    auto fragmentShader = std::make_shared<Shader>(GL_FRAGMENT_SHADER, fragmentSource);

    if (!vertexShader->compile()) {
        logError("Failed to compile vertex shader for program '" + programName + "'.");
        return false;
    }
    if (!fragmentShader->compile()) {
        logError("Failed to compile fragment shader for program '" + programName + "'.");
        return false;
    }

    logTrace("Create and link the shader program.");
    auto shaderProgram = std::make_shared<ShaderProgram>();
    if (shaderProgram->attachShader(vertexShader) && shaderProgram->attachShader(fragmentShader)) {
        if (shaderProgram->link()) {
            shaderPrograms[programName] = shaderProgram;
            logDebug("Shader program '" + programName + "' successfully created.");
            return true;
        } else {
            logError("Failed to link shader program '" + programName + "'.");
        }
    } else {
        logError("Failed to attach shaders for program '" + programName + "'.");
    }

    return false;
}

bool ShaderManager::removeShaderProgram(const std::string &programName) {
    auto it = shaderPrograms.find(programName);
    if (it == shaderPrograms.end()) {
        logWarn("Shader program '" + programName + "' does not exist. Removal skipped.");
        return false;
    }

    shaderPrograms.erase(it);
    logDebug("Shader program '" + programName + "' removed successfully.");

    return true;
}

std::shared_ptr<ShaderProgram> ShaderManager::getShaderProgram(const std::string &programName) const {
    auto it = shaderPrograms.find(programName);
    if (it != shaderPrograms.end()) {
        logTrace("Shader program '" + programName + "' found.");
        return it->second;
    }
    logWarn("Shader program '" + programName + "' not found.");
    return nullptr;
}

void ShaderManager::useShaderProgram(const std::string &programName) {
    if (currentProgram == programName) {
        return;
    }

    auto shaderProgram = getShaderProgram(programName);
    if (shaderProgram) {
        logTrace("Using shader program '" + programName + "'.");
        shaderProgram->use();
        currentProgram = programName;
    } else {
        logError("Unable to use shader program '" + programName + "' because it was not found.");
    }
}
