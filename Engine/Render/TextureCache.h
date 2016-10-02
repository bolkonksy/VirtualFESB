#ifndef _TEXTURECACHE_H
#define _TEXTURECACHE_H

#include <map>
#include "Texture.h"

namespace Engine
{
    class TextureCache {
    public:
        Texture getTexture(const std::string& texturePath);

    private:
        std::map<std::string, Texture> _textureMap;
    };
}

#endif