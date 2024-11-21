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

#include "GraphicsContext.h"
#include "ConfigurationManager.h"
#include "Logger.h"

GraphicsContext::GraphicsContext() : context(nullptr), displayWidth(0), displayHeight(0) {}

GraphicsContext::~GraphicsContext() { cleanup(); }

bool GraphicsContext::initialize() {
    if (!createEssosContext()) {
        logError("Failed to initialize the graphics context.");
        return false;
    }
    logTrace("Graphics context initialized successfully.");
    return true;
}

void GraphicsContext::cleanup() {
    if (context) {
        EssContextDestroy(context);
        context = nullptr;
        logTrace("Graphics context destroyed.");
    }
}

bool GraphicsContext::createEssosContext() {
    ConfigurationManager &configManager = ConfigurationManager::getInstance();

    context = EssContextCreate();
    if (!context) {
        logError("Failed to create Essos context.");
        return false;
    }

    auto cleanupOnError = [this]() {
        logEssosError();
        EssContextDestroy(context);
        context = nullptr;
    };

    if (!EssContextSetUseDirect(context, configManager.getValue("direct_mode") == "true")) {
        logError("Failed to set Essos context to use direct mode.");
        cleanupOnError();
        return false;
    }

    if (!EssContextInit(context)) {
        logError("Failed to initialize Essos context.");
        cleanupOnError();
        return false;
    }

    if (!EssContextGetDisplaySize(context, &displayWidth, &displayHeight)) {
        logError("Failed to get display size from Essos context.");
        cleanupOnError();
        return false;
    }

    int windowWidth = displayWidth;
    int windowHeight = displayHeight;

    int widthValue = std::stoi(configManager.getValue("window_width"));
    int heightValue = std::stoi(configManager.getValue("window_height"));
    if (widthValue > 0 && heightValue > 0) {
        windowWidth = widthValue;
        windowHeight = heightValue;
        logTrace("Configured window size: " + std::to_string(windowWidth) + "x" + std::to_string(windowHeight));
    } else {
        logTrace("Using default display size: " + std::to_string(displayWidth) + "x" + std::to_string(displayHeight));
    }

    if (!EssContextSetInitialWindowSize(context, windowWidth, windowHeight)) {
        logError("Failed to set initial window size for Essos context.");
        cleanupOnError();
        return false;
    }

    if (!EssContextStart(context)) {
        logError("Failed to start Essos context.");
        cleanupOnError();
        return false;
    }

    configManager.setValue("window_width", std::to_string(windowWidth));
    configManager.setValue("window_height", std::to_string(windowHeight));
    displayWidth = windowWidth;
    displayHeight = windowHeight;
    logInfo("Essos context created with window size: " + std::to_string(windowWidth) + "x" +
            std::to_string(windowHeight));
    return true;
}

void GraphicsContext::logEssosError() {
    logError("Essos error: " + std::string(EssContextGetLastErrorDetail(context)));
}

void GraphicsContext::updateDisplay() {
    EssContextUpdateDisplay(context);
    EssContextRunEventLoopOnce(context);
    logTrace("Display updated and event loop run once.");
}
