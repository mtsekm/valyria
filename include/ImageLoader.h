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

#ifndef VALYRIA_IMAGELOADER_H
#define VALYRIA_IMAGELOADER_H

#include <string>

#include <GLES2/gl2.h>

/**
 * A utility class for loading images as OpenGL ES textures.
 */
class ImageLoader {
public:
    /**
     * Loads an image from a file and creates an OpenGL texture.
     *
     * @param filePath The path to the image file.
     * @return The OpenGL texture ID of the loaded texture, or 0 if loading failed.
     */
    static GLuint loadTextureFromFile(const std::string &filePath);

    /**
     * Checks if the provided file path points to a PNG image.
     *
     * @param filePath The path to the image file.
     * @return True if the file is a PNG, false otherwise.
     */
    static bool isPNG(const std::string &filePath);

    /**
     * Checks if the provided file path points to a JPG image.
     *
     * @param filePath The path to the image file.
     * @return True if the file is a JPG, false otherwise.
     */
    static bool isJPG(const std::string &filePath);

private:
    /**
     * Loads a PNG image from a file and creates an OpenGL texture.
     *
     * @param filePath The path to the PNG image file.
     * @return The OpenGL texture ID of the loaded texture, or 0 if loading failed.
     */
    static GLuint loadPNGTexture(const std::string &filePath);

    /**
     * Loads a JPG image from a file and creates an OpenGL texture.
     *
     * @param filePath The path to the JPG image file.
     * @return The OpenGL texture ID of the loaded texture, or 0 if loading failed.
     */
    static GLuint loadJPGTexture(const std::string &filePath);
};

#endif // VALYRIA_IMAGELOADER_H
