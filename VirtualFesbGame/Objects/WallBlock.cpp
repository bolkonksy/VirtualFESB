#include "WallBlock.h"

#include <Engine/IO/ResourceManager.h>

/* Default parameters: fixedUV = false, uvScale = 1.0f*/
bool WallBlock::init(b2World* world, const glm::vec2& position, const glm::vec2& renderDimensions,
                     const glm::vec2& collisionDimensions, float angle, b2BodyType type,
                     const Engine::Color& color, const std::string& texturePath, bool fixedUV, float uvScale)
{
    _renderDimensions    = renderDimensions;
    _collisionDimensions = collisionDimensions;
    _color = color;
    _hitBox.init(world, collisionDimensions, position, angle , type);
    _hitBox.body()->SetLinearDamping(0.9f);
    _texture = Engine::ResourceManager::getTexture(texturePath);
    _fixedUV = fixedUV;
    _uvScale = uvScale;

    /* Collidies only with player */
    b2Filter filter = _hitBox.fixture()->GetFilterData();
    filter.categoryBits = (uint16)CollisionFilter::WALL;
    filter.maskBits = (uint16)CollisionFilter::PLAYER;
    _hitBox.fixture()->SetFilterData(filter);

    return true;
}

void WallBlock::dispose(b2World* world)
{
    _hitBox.dispose(world);
}

void WallBlock::render(Engine::SpriteBatch& spriteBatch)
{
    glm::vec2 center = position();
    glm::vec2 position(center.x - _renderDimensions.x / 2.0f, center.y - _renderDimensions.y / 2.0f);
    glm::vec4 destRect = glm::vec4(position, _renderDimensions);

    glm::vec4 uvRect;
    if (_fixedUV)
        uvRect = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
    else
        /* uv / 20.0f - because objects are huge in this game */
        uvRect = glm::vec4(position, _renderDimensions) / (float)_texture.width;

    spriteBatch.draw(destRect, uvRect * _uvScale, _texture.id, _color, _renderDepth, _hitBox.body()->GetAngle());
}

void WallBlock::renderDebug(Engine::DebugRenderer& debugRenderer)
{
    _hitBox.renderDebug(debugRenderer);
}

void WallBlock::renderDebug(Engine::DebugRenderer& debugRenderer, const Engine::Color& color)
{
    _hitBox.renderDebug(debugRenderer, color);
}