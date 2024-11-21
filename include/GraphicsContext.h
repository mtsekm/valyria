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

#ifndef VALYRIA_GRAPHICSCONTEXT_H
#define VALYRIA_GRAPHICSCONTEXT_H

#include <essos.h>

/**
 * A class that manages the graphics context for rendering operations.
 */
class GraphicsContext {
public:
    /**
     * Constructs a GraphicsContext.
     */
    GraphicsContext();

    /**
     * Destructor for GraphicsContext.
     */
    ~GraphicsContext();

    /**
     * Initializes the graphics context.
     *
     * This method creates and sets up the Essos context, initializes
     * display parameters, and prepares the context for rendering.
     *
     * @return True if initialization was successful, false otherwise.
     */
    bool initialize();

    /**
     * Cleans up and releases resources associated with the graphics context.
     */
    void cleanup();

    /**
     * Gets the width of the display/window.
     *
     * @return The width of the display in pixels.
     */
    int getWidth() const { return displayWidth; }

    /**
     * Gets the height of the display/window.
     *
     * @return The height of the display in pixels.
     */
    int getHeight() const { return displayHeight; }

    /**
     * Updates the display to present the rendered content.
     *
     * This method handles the buffer swap and display refresh to show the
     * most recent rendering on the screen.
     */
    void updateDisplay();

private:
    EssCtx *context;   ///< Pointer to the Essos context used for rendering.
    int displayWidth;  ///< Width of the display in pixels.
    int displayHeight; ///< Height of the display in pixels.

    /**
     * Creates and initializes the Essos context.
     *
     * @return True if the Essos context was created successfully, false otherwise.
     */
    bool createEssosContext();

    /**
     * Logs Essos error messages.
     */
    void logEssosError();
};

#endif // VALYRIA_GRAPHICSCONTEXT_H
