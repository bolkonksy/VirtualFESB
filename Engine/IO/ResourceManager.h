#ifndef _RESOURCEMANAGER_H
#define _RESOURCEMANAGER_H

#include <string>
#include "../Render/TextureCache.h"

namespace Engine
{
    class ResourceManager {
    public:
        static Texture getTexture(const std::string& texturePath);

    private:
        static TextureCache _textureCache;
    };
}

#endif