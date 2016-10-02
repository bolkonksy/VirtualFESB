#ifndef _PLAYER_H
#define _PLAYER_H

#include <Engine/EventHandler.h>
#include <Engine/Render/TileSheet.h>
#include <Engine/Render/Spritebatch.h>
#include "CollisionCircle.h"

class Player {
public:
    bool init(b2World* world, const glm::vec2& position, const float renderRadius, const float collisionRadius,
              const Engine::Color& color);
    void dispose(b2World* world);

    void update(const Engine::EventHandler& eventHandler);
    void render(Engine::SpriteBatch& spriteBatch);
    void renderDebug(Engine::DebugRenderer& debugRenderer);
    void renderDebug(Engine::DebugRenderer& debugRenderer, const Engine::Color& color);
    
    /* Setters */
    void setPosition(const glm::vec2& position)
         { _hitBox.body()->SetTransform(b2Vec2(position.x, position.y), _hitBox.body()->GetAngle()); }

    /* Getters */
    inline bool isInit()                     const { return (_hitBox.body() != nullptr); }
    inline bool testPoint(float x, float y)  const { return isInit() ? _hitBox.testPoint(x, y) : false; }
    const float            renderRadius()    const { return _renderRadius; }
    const float            collisionRadius() const { return _collisionRadius; }
    const glm::vec2        position()        const { return isInit() ? _hitBox.position() : glm::vec2(0.0f, 0.0f); }
    const Engine::Color    color()           const { return _color; }
    const Engine::Texture& texture()         const { return _tileSheet.getTexture(); }

private:
    float _animTime        = 0.0f; /* Animation time */
    float _renderRadius    = 1.0f;
    float _collisionRadius = 1.0f;
    Engine::Color _color;
    Engine::TileSheet _tileSheet;
    CollisionCircle _hitBox;
};

#endif