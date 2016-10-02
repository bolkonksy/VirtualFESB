#ifndef _SHADERPROGRAM_H
#define _SHADERPROGRAM_H

#include <string>
#include <GL/glew.h>

namespace Engine
{
    /* Class for handling the compilation, linking and using the GLSL shader program */
    class ShaderProgram {
    public:
        /* There is no init function, but to properly use the shader, follow these steps in correct order */

        /* 1. step is compiling shaders */
        bool compileShadersFromFile(const std::string& vertexShaderFilePath,
                                    const std::string& fragmentShaderFilePath);
        bool compileShadersFromSource(const char* vertexSource,
                                      const char* fragmentSource);

        /* 2. step is binding attributes */
        void bindAttribute(const std::string& attributeName);

        /* 3. step is linking shaders */
        bool linkShaders();

        bool getUniformLocation(const std::string& uniformName, GLint& location);

        void use();
        void unuse();

        void dispose();

    private:
        /* Gets automatically set to true after third init step */
        bool   _initialized      = false;
        GLuint _programID        = 0;
        GLuint _vertexShaderID   = 0;
        GLuint _fragmentShaderID = 0;
        size_t _attributeCount   = 0;

        /* Returns false and prints message if shader is not initialized properly */
        bool initialized();
        /* Returns false and prints message if something is not created */
        bool programAndShadersCreated();
        bool compileShader(const char* source, const std::string& type, GLuint id);
    };
}

#endif