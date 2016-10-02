#ifndef _COLLISIONCIRCLE_H
#define _COLLISIONCIRCLE_H

#include <GLM/glm.hpp>
#include "ICollisionObject.h"

class CollisionCircle : public ICollisionObject {
public:
    virtual void init(b2World* world, const float radius, const glm::vec2& position,
                      float density = 1.0f, float friction = 0.1f);
    void dispose(b2World* world) override;

    void renderDebug(Engine::DebugRenderer& debugRenderer) override;
    void renderDebug(Engine::DebugRenderer& debugRenderer, const Engine::Color& color) override;

    float radius() const { return _radius; }

private:
    float _radius = 1.0f;
};

#endif