#include "FloorTile.h"

#include <Engine/IO/ResourceManager.h>

/* Default parameters: fixedUV = false, uvScale = 1.0f */
bool FloorTile::init(b2World* world, const glm::vec2& position, const glm::vec2& renderDimensions,
                     const glm::vec2& collisionDimensions, float angle, b2BodyType type, const Engine::Color& color,
                     const std::string& texturePath, bool fixedUV, float uvScale)
{
    _renderDimensions    = renderDimensions;
    _collisionDimensions = collisionDimensions;
    _color = color;
    _hitBox.init(world, collisionDimensions, position, angle , type);
    _hitBox.body()->SetLinearDamping(0.9f);
    _texture = Engine::ResourceManager::getTexture(texturePath);
    _fixedUV = fixedUV;
    _uvScale = uvScale;

    /* Draw it beneath player and walls */
    _renderDepth = 3.0f;

    /* Collidies with no one */
    b2Filter filter = _hitBox.fixture()->GetFilterData();
    filter.categoryBits = (uint16)CollisionFilter::FLOOR;
    filter.maskBits = 0;
    _hitBox.fixture()->SetFilterData(filter);

    return true;
}
