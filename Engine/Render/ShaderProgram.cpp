#include "ShaderProgram.h"

#include <vector>
#include <fstream>
#include "../IO/IOManager.h"

namespace Engine
{
    bool ShaderProgram::compileShadersFromFile(const std::string& vertexShaderFilePath, const std::string& fragmentShaderFilePath)
    {
        std::string vertSource;
        std::string fragSource;
        
        if (IOManager::readFileToBuffer(vertexShaderFilePath, vertSource) == false)
            return false;
        if (IOManager::readFileToBuffer(fragmentShaderFilePath, fragSource) == false)
            return false;

        if (compileShadersFromSource(vertSource.c_str(), fragSource.c_str()) == false)
            return false;

        _initialized = true;
        return true;
    }

    bool ShaderProgram::compileShadersFromSource(const char* vertexSource, const char* fragmentSource)
    {
        /* Create program and shaders */
        _programID = glCreateProgram();
        _vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
        _fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
        
        /* Checking if everything got created correctly */
        if (programAndShadersCreated() == false)
            return false;

        if (compileShader(vertexSource, "Vertex Shader", _vertexShaderID) == false)
            return false;
        if (compileShader(fragmentSource, "Fragment Shader", _fragmentShaderID) == false)
            return false;

        _initialized = true;
        return true;
    }

    void ShaderProgram::bindAttribute(const std::string& attributeName)
    {
        /* Check if tbe Program was previously created */
        if (programAndShadersCreated() == true)
            glBindAttribLocation(_programID, _attributeCount++, attributeName.c_str());
        else
            fprintf(stderr, "Binding attribute error: Shader program not found\n");
    }

    /* Code is mostly taken from offical wiki http://www.opengl.org/wiki/Shader_Compilation */
    bool ShaderProgram::linkShaders()
    {
        /* Shouldn't be able to call this function twice */
        if (initialized() == false)
            return false;

        /* Checking - Program and shadres should be created by now */
        if (programAndShadersCreated() == false)
            return false;

        /* Attach our shaders to our program */
        glAttachShader(_programID, _vertexShaderID);
        glAttachShader(_programID, _fragmentShaderID);

        /* Link our program */
        glLinkProgram(_programID);

        /* Note the different functions here: glGetProgram* instead of glGetShader* */
        GLint isLinked = 0;
        glGetProgramiv(_programID, GL_LINK_STATUS, (int *)&isLinked);
        if (isLinked == GL_FALSE) {
            GLint maxLength = 0;
            glGetProgramiv(_programID, GL_INFO_LOG_LENGTH, &maxLength);

            /* The maxLength includes the NULL character */
            std::vector<GLchar> errorLog(maxLength);
            glGetProgramInfoLog(_programID, maxLength, &maxLength, &errorLog[0]);

            /* We don't need the program anymore. */
            glDetachShader(_programID, _vertexShaderID);
            glDetachShader(_programID, _fragmentShaderID);
            glDeleteProgram(_programID);
            /* Don't leak shaders either */
            glDeleteShader(_vertexShaderID);
            glDeleteShader(_fragmentShaderID);

            std::fprintf(stderr, "Shaders failed to link!\n");
            std::fprintf(stderr, "%s\n", &(errorLog[0]));

            if (glGetError() == GL_NO_ERROR)
                _initialized = true;

            return _initialized;
        }

        /* Detach and delete shaders after linking - we don't need them anymore */
        glDetachShader(_programID, _vertexShaderID);
        glDetachShader(_programID, _fragmentShaderID);
        glDeleteShader(_vertexShaderID);
        glDeleteShader(_fragmentShaderID);

        return true;
    }

    bool ShaderProgram::getUniformLocation(const std::string& uniformName, GLint& location)
    {
        if (initialized() == false)
            return false;

        location = glGetUniformLocation(_programID, uniformName.c_str());
        if (location == GL_INVALID_INDEX) {
            std::fprintf(stderr, "Uniform %s not found in shader!", uniformName.c_str());
            return false;
        }

        return true;
    }

    void ShaderProgram::use()
    {
        if (initialized() == false)
            return;

        glUseProgram(_programID);
        for (size_t i = 0; i < _attributeCount; i++)
            glEnableVertexAttribArray(i);
    }

    void ShaderProgram::unuse()
    {
        /* Return to fixed functionality */
        glUseProgram(0);
        for (size_t i = 0; i < _attributeCount; i++)
            glDisableVertexAttribArray(i);
    }

    void ShaderProgram::dispose()
    {
        _initialized = false;
        /* Reset attribute count */
        _attributeCount = 0;

        /* Delete program and shaders, if they still exist */
        if (glIsProgram(_programID) == GL_TRUE)
            glDeleteProgram(_programID);
        if (glIsShader(_vertexShaderID) == GL_TRUE)
            glDeleteShader(_vertexShaderID);
        if (glIsShader(_fragmentShaderID) == GL_TRUE)
            glDeleteShader(_fragmentShaderID);
    }

    bool ShaderProgram::initialized()
    {
        if (_initialized)
            return true;
        
        fprintf(stderr, "ShaderProgram not initialized properly!\n");
        return false;
    }

    bool ShaderProgram::programAndShadersCreated()
    {
        bool success = true;

        if (glIsProgram(_programID) != GL_TRUE) {
            fprintf(stderr, "Program wasn't created correctly!\n");
            success = false;
        }

        if (glIsShader(_vertexShaderID) != GL_TRUE) {
            fprintf(stderr, "Vertex shader wasn't created correctly!\n");
            success = false;
        }

        if (glIsShader(_fragmentShaderID) != GL_TRUE) {
            fprintf(stderr, "Fragment shader wasn't created correctly!\n");
            return false;
        }

        return success;
    }

    bool ShaderProgram::compileShader(const char* source, const std::string& type, GLuint id)
    {
        /* Define shader input data */
        glShaderSource(id, 1, &source, nullptr);

        /* Compilation */
        glCompileShader(id);

        /* Error checking */
        GLint status = 0;
        glGetShaderiv(id, GL_COMPILE_STATUS, &status);
        if (status == GL_FALSE) {
            GLint maxLength = 0;
            glGetShaderiv(id, GL_INFO_LOG_LENGTH, &maxLength);

            std::vector<char> errorLog(maxLength);
            glGetShaderInfoLog(id, maxLength, &maxLength, &errorLog[0]);

            glDeleteShader(id);

            std::fprintf(stderr, "Shader %s failed to compile!\n", type.c_str());
            std::fprintf(stderr, "%s\n", &(errorLog[0]));
            return false;
        }

        return true;
    }
}