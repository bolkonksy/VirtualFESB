#ifndef _TILESHEET_H
#define _TILESHEET_H

#include <GLM\glm.hpp>
#include "Texture.h"

namespace Engine
{
    class TileSheet {
    public:
        bool init(const Texture& texture, const glm::ivec2& gridSize);

        void rotateVerticalUV();
        void rotateHorizontalUV();

        /* Setters */
        /* Indexing from 0 to MAX_INDEX starting from top left corner of the spritesheet */
        void setUV(int index, bool rotateVertical = false, bool rotateHorizontal = false);

        /* Getters */
        const glm::vec4& UV()         const { return _uv; }
        const Texture&   getTexture() const { return _texture; }

    private:
        Texture    _texture;
        glm::ivec2 _gridSize = glm::ivec2(1, 1);
        glm::vec4  _uv       = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
    };
}

#endif