#ifndef _TEXTURE_H
#define _TEXTURE_H

#include <string>
#include <GL/glew.h>

namespace Engine
{
    struct Texture {
        GLuint id;
        int width;
        int height;
        std::string filePath = "";
    };
}

#endif