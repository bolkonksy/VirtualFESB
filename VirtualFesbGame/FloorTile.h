#ifndef _FLOORTILE_H
#define _FLOORTILE_H

#include "Objects/WallBlock.h"

class FloorTile : public WallBlock {
/*
 * Only difference between FloorTile and WallBlock is their collision property 
 * Also, it's easier to render them separatley, and rules for placing and editing them in
 * level editor are different
 */
public:
    virtual bool init(b2World* world, const glm::vec2& position, const glm::vec2& renderDimensions,
                      const glm::vec2& collisionDimensions, float angle, b2BodyType type,
                      const Engine::Color& color, const std::string& texturePath, bool fixedUV = false) override;
};

#endif