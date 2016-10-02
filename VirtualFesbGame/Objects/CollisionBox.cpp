#include "CollisionBox.h"

/* Default paramters: isSensor = false, density = 1.0f, friction = 0.1f */
void CollisionBox::init(b2World* world, const glm::vec2& dimensions, const glm::vec2& position, float angle,
                        b2BodyType type, bool isSensor, float density, float friction)
{
    _dimensions = dimensions;

    b2BodyDef bodyDef;
    bodyDef.type = type;
    bodyDef.position.Set(position.x, position.y);
    bodyDef.angle = angle;
    bodyDef.fixedRotation = true;

    _body = world->CreateBody(&bodyDef);

    b2PolygonShape boxShape;
    boxShape.SetAsBox(dimensions.x / 2.0f, dimensions.y / 2.0f);
     
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &boxShape;
    fixtureDef.density = density;
    fixtureDef.friction = friction;
    fixtureDef.isSensor = isSensor;
    
    _fixture = _body->CreateFixture(&fixtureDef);
}

void CollisionBox::dispose(b2World* world)
{
    if (_body != nullptr) {
        world->DestroyBody(_body);
        _body = nullptr;
        _dimensions = glm::vec2(1.0f);
    }
}

void CollisionBox::renderDebug(Engine::DebugRenderer& debugRenderer)
{
    glm::vec4 destRect(position() - _dimensions / 2.0f, _dimensions);
    debugRenderer.drawBox(destRect, Engine::Color::White(), _body->GetAngle());
}

void CollisionBox::renderDebug(Engine::DebugRenderer& debugRenderer, const Engine::Color& color)
{
    glm::vec4 destRect(position() - _dimensions / 2.0f, _dimensions);
    debugRenderer.drawBox(destRect, color, _body->GetAngle());
}
