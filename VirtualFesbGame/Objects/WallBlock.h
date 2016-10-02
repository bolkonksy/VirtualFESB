#ifndef _WALLBLOCK_H
#define _WALLBLOCK_H

#include <string>
#include <Engine/Render/Spritebatch.h>
#include <Engine/Render/Texture.h>
#include "CollisionBox.h"

class WallBlock {
public:
    virtual bool init(b2World* world, const glm::vec2& position, const glm::vec2& renderDimensions,
                      const glm::vec2& collisionDimensions, float angle, b2BodyType type, const Engine::Color& color,
                      const std::string& texturePath, bool fixedUV = false, float uvScale = 1.0f);
    void dispose(b2World* world);

    void render(Engine::SpriteBatch& spriteBatch);
    void renderDebug(Engine::DebugRenderer& debugRenderer);
    void renderDebug(Engine::DebugRenderer& debugRenderer, const Engine::Color& color);

    /* Setters */
    void setFixedUV(bool value) { _fixedUV = value; }

    /* Getters */
    CollisionBox           hitBox()        const { return _hitBox; }
    const glm::vec2        position()      const { return _hitBox.position(); }
    const glm::vec2        renderDims()    const { return _renderDimensions; }
    const glm::vec2        collisionDims() const { return _collisionDimensions; }
    const float            uvScale()       const { return _uvScale; }
    const Engine::Color    color()         const { return _color; }
    const Engine::Texture& texture()       const { return _texture; }
    bool fixedUV()                         const { return _fixedUV; }
    bool testPoint(float x, float y)       const { return _hitBox.testPoint(x, y); }

protected:
    glm::vec2 _renderDimensions    = glm::vec2(1.0f);
    glm::vec2 _collisionDimensions = glm::vec2(1.0f);
    float     _renderDepth         = 2.0f;
    float     _uvScale             = 1.0f;
    bool      _fixedUV             = false;
    Engine::Texture _texture;
    Engine::Color   _color;
    CollisionBox    _hitBox;
};

#endif