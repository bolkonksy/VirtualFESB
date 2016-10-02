#include "TileSheet.h"

#include <iostream>

namespace Engine
{
    bool TileSheet::init(const Texture& texture, const glm::ivec2& gridSize)
    {
        if (gridSize.x < 0 || gridSize.y < 0) {
            std::cerr << "ERROR! Negative tileDimenisions!" << std::endl;
            return false;
        }

        _texture = texture;
        _gridSize = gridSize;
        setUV(0);

        return true;
    }

    void TileSheet::rotateVerticalUV()
    {
        _uv.x += 1.0f / _gridSize.x;
        _uv.z *= -1.0f;
    }

    void TileSheet::rotateHorizontalUV()
    {
        _uv.y += 1.0f / _gridSize.y;
        _uv.w *= -1.0f;
    }

    /* Default parameter for rotateVertical and rotateHorizontal is false */
    void TileSheet::setUV(int index, bool rotateVertical, bool rotateHorizontal)
    {
        static const int MAX_INDEX = _gridSize.x * _gridSize.y - 1;
        if (index > MAX_INDEX || index < 0) {
            std::cerr << "TileSheet: Wrong index call! Nothing changed" << std::endl;
            return;
        }

        glm::ivec2 tile;
        tile.x = index % _gridSize.x;
        tile.y = index / _gridSize.x;
        /* Internally, vertical indexing starts from bottom, so reversing tile.y gets the opposite */
        tile.y = _gridSize.y - tile.y - 1;

        _uv.x = tile.x / (float)_gridSize.x;
        _uv.y = tile.y / (float)_gridSize.y;
        _uv.z = 1.0f / _gridSize.x;
        _uv.w = 1.0f / _gridSize.y;

        if (rotateVertical)
            rotateVerticalUV();
        if (rotateHorizontal)
            rotateHorizontalUV();
    }
}