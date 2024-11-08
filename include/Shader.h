#ifndef VALYRIA_SHADER_H
#define VALYRIA_SHADER_H

#include <GLES2/gl2.h>
#include <string>

/**
 * A class that represents an individual OpenGL ES shader (vertex or fragment).
 */
class Shader {
public:
    /**
     * Constructs a Shader object with the specified type and file path.
     *
     * @param type The type of the shader (GL_VERTEX_SHADER or GL_FRAGMENT_SHADER).
     * @param fileName The file name of the shader source code, relative to the asset base directory.
     */
    Shader(GLenum type, const std::string &fileName);

    /**
     * Destructor that releases OpenGL resources associated with the shader.
     */
    ~Shader();

    /**
     * Compiles the shader source code from the specified file.
     *
     * @return True if the shader compiled successfully; false otherwise.
     */
    bool compile();

    /**
     * Gets the OpenGL shader ID.
     *
     * @return The OpenGL ID of the compiled shader, or 0 if the shader failed to compile.
     */
    GLuint getShaderID() const;

    /**
     * Releases the OpenGL resources associated with the shader.
     */
    void release();

private:
    GLenum type;          ///< The type of shader (GL_VERTEX_SHADER or GL_FRAGMENT_SHADER).
    GLuint shaderID;      ///< The OpenGL ID of the compiled shader.
    std::string filename; ///< The path to the shader file within the asset base directory.

    /**
     * Logs any errors encountered during shader compilation.
     */
    void logShaderError() const;

    /**
     * Loads the shader source code from a file.
     *
     * @param fileName The file name of the shader source code.
     * @return The contents of the shader file as a string, or an empty string if the file failed to load.
     */
    std::string loadShaderFromFile(const std::string &fileName);
};

#endif // VALYRIA_SHADER_H
