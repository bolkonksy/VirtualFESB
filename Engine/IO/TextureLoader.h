#ifndef _TEXTURELOADER_H
#define _TEXTURELOADER_H

#include <string>
#include "../Render/Texture.h"

namespace Engine
{
    class TextureLoader {
    public:
        static Texture loadPNG(const std::string& filePath);
    };
}

#endif