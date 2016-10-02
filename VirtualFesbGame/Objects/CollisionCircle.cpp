#include "CollisionCircle.h"

/* Default paramters: density = 1.0f, friction = 0.1f */
void CollisionCircle::init(b2World* world, const float radius, const glm::vec2& position,
                           float density, float friction)
{
    _radius = radius;

    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.fixedRotation = true;
    bodyDef.position.Set(position.x, position.y);

    _body = world->CreateBody(&bodyDef);

    b2CircleShape circleShape;
    circleShape.m_p.Set(0, 0);
    circleShape.m_radius = radius;
     
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &circleShape;
    fixtureDef.density = density;
    fixtureDef.friction = friction;
    
    _fixture = _body->CreateFixture(&fixtureDef);
}

void CollisionCircle::dispose(b2World* world)
{
    if (_body != nullptr) {
        world->DestroyBody(_body);
        _body = nullptr;
        _radius = 1.0f;
    }
}

void CollisionCircle::renderDebug(Engine::DebugRenderer& debugRenderer)
{
    debugRenderer.drawCircle(position(), Engine::Color::White(), _radius);
}

void CollisionCircle::renderDebug(Engine::DebugRenderer& debugRenderer, const Engine::Color& color)
{
    debugRenderer.drawCircle(position(), color, _radius);
}