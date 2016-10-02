#ifndef _COLLISIONBOX_H
#define _COLLISIONBOX_H

#include <GLM/glm.hpp>
#include "ICollisionObject.h"

class CollisionBox : public ICollisionObject {
public:
    void init(b2World* world, const glm::vec2& dimensions, const glm::vec2& position, float angle,
              b2BodyType type, bool isSensor = false, float density = 1.0f, float friction = 0.1f);
    void dispose(b2World* world) override;

    void renderDebug(Engine::DebugRenderer& debugRenderer) override;
    void renderDebug(Engine::DebugRenderer& debugRenderer, const Engine::Color& color) override;

    glm::vec2 dimensions() const { return _dimensions; }

private:
    glm::vec2 _dimensions = glm::vec2(10.0f);
};

#endif