#include "ErrorHandler.h"

#include <iostream>
#include <GL/glew.h>
#include "Engine.h"

namespace Engine
{
    void Engine::applicationError(const std::string& errorMessage)
    {
        std::cerr << errorMessage << std::endl;
        Engine::quitEngine();
        system("pause");
        exit(1);
    }

    bool errorCheckOpenGL()
    {
        GLenum errorCode;
        //const GLubyte* errorString;

        if ((errorCode = glGetError()) != GL_NO_ERROR) {
            //errorString = gluErrorString(errorCode);
            //std::cerr << errorString << std::endl;
            std::cerr << glewGetErrorString(errorCode) << std::endl;
            return false;
        }

        return true;
    }
}