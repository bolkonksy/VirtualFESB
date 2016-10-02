#include "TextureCache.h"

#include "../IO/TextureLoader.h"

namespace Engine
{
    Texture TextureCache::getTexture(const std::string& texturePath)
    {
        auto iterator = _textureMap.find(texturePath);

        if (iterator == _textureMap.end()) {
            Texture newTexture = TextureLoader::loadPNG(texturePath);
            _textureMap.insert(std::make_pair(texturePath, newTexture));

            return newTexture;
        }

        return iterator->second;
    }
}