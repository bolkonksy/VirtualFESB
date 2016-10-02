#ifndef _ICOLLISIONOBJECT_H
#define _ICOLLISIONOBJECT_H

#include <Box2D/Box2D.h>
#include <Engine/Render/DebugRenderer.h>

/* Represents categoryBits of gameObjects. Used for defining collision between different objets via maskBits */
enum class CollisionFilter : uint16 {
    PLAYER            = 0x0001,
    WALL              = 0x0002,
    FLOOR             = 0x0004,
    QUESTION_PLATFORM = 0x0008,
    TELEPORT_PLATFORM = 0x0010
};

/* Inteface for objects using Box2D collision */
class ICollisionObject {
public:
    /* virtual void init( DIFFERENT_ARGS ) = 0; */
    virtual void dispose(b2World* world) = 0;

    virtual void renderDebug(Engine::DebugRenderer& debugRenderer) = 0;
    virtual void renderDebug(Engine::DebugRenderer& debugRenderer, const Engine::Color& color) = 0;

    /* Getters */
    b2Body*         body()     const { return _body; }
    b2Fixture*      fixture()  const { return _fixture; }
    const glm::vec2 position() const
    {
        glm::vec2 position(_body->GetPosition().x, _body->GetPosition().y);
        return position;
    }
    bool testPoint(float x, float y) const { return _fixture->TestPoint(b2Vec2(x, y)); }

protected:
    b2Body*    _body    = nullptr;
    b2Fixture* _fixture = nullptr;
};

#endif