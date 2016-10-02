#include "Player.h"

#include <SDL/SDL_keycode.h>
#include <Engine/Render/Texture.h>
#include <Engine/IO/ResourceManager.h>

bool Player::init(b2World* world, const glm::vec2& position, const float renderRadius, 
                  const float collisionRadius, const Engine::Color& color)
{
    _collisionRadius = renderRadius * 0.9f;
    _renderRadius    = renderRadius;
    _color           = color;
    _hitBox.init(world, _collisionRadius, position);

    Engine::Texture texture = Engine::ResourceManager::getTexture("Textures/Static/player.png");
    if (_tileSheet.init(texture, glm::ivec2(4, 4)) == false)
        return false;
    _tileSheet.setUV(0);

    /* Collidies with walls and both platforms */
    b2Filter filter = _hitBox.fixture()->GetFilterData();
    filter.categoryBits = (uint16)CollisionFilter::PLAYER;
    filter.maskBits = (uint16)CollisionFilter::PLAYER | (uint16)CollisionFilter::WALL |
                      (uint16)CollisionFilter::QUESTION_PLATFORM | (uint16)CollisionFilter::TELEPORT_PLATFORM;
    _hitBox.fixture()->SetFilterData(filter);

    return true;
}

void Player::dispose(b2World* world)
{
    _collisionRadius = _renderRadius = 1.0f;
    _color = Engine::Color(255, 255, 255, 255);
    _hitBox.dispose(world);
}

void Player::update(const Engine::EventHandler& eventHandler)
{
    if (!isInit())
        return;

    b2Body* body = _hitBox.body();
    const float MOVE_FORCE = 220.0f;
    const float MOVE_STOPPING_SPEED = 0.85f;
    
    /* ---> Vertical movement
       If no key is being pressed or both up and down keys are pressed, dont move */
    if ((eventHandler.keyDown(SDLK_w) && eventHandler.keyDown(SDLK_s)) ||
        !(eventHandler.keyDown(SDLK_w) || eventHandler.keyDown(SDLK_s))) {
        body->SetLinearVelocity(b2Vec2(body->GetLinearVelocity().x,
                                       body->GetLinearVelocity().y * MOVE_STOPPING_SPEED));
    }
    /* Move up */
    else if (eventHandler.keyDown(SDLK_w)) {
        body->ApplyForceToCenter(b2Vec2(0.0f, body->GetMass() * MOVE_FORCE), true);
    }
    /* Move down */
    else if (eventHandler.keyDown(SDLK_s)) {
        body->ApplyForceToCenter(b2Vec2(0.0f, body->GetMass() * -MOVE_FORCE), true);
    }

    /* ---> Horizontal movement
       If no key is being pressed or both left and right keys are pressed, dont move */
    if ( (eventHandler.keyDown(SDLK_a) && eventHandler.keyDown(SDLK_d)) ||
        !(eventHandler.keyDown(SDLK_a) || eventHandler.keyDown(SDLK_d))) {
        body->SetLinearVelocity(b2Vec2(body->GetLinearVelocity().x * MOVE_STOPPING_SPEED,
                                       body->GetLinearVelocity().y));
    }
    /* Move left */
    else if (eventHandler.keyDown(SDLK_a)) {
        body->ApplyForceToCenter(b2Vec2(body->GetMass() * -MOVE_FORCE, 0.0f), true);
    }
    /* Move right */
    else if (eventHandler.keyDown(SDLK_d)) {
        body->ApplyForceToCenter(b2Vec2(body->GetMass() * MOVE_FORCE, 0.0f), true);
    }


    /*** Animation frame calculation ***/
    auto velocity = _hitBox.body()->GetLinearVelocity();
    glm::vec2 vel(velocity.x, velocity.y);

    /* Standing still - Used 5.0f instead of 1.0f bcs of slow deceleration */
    if (abs(vel.x) < 1.0f && abs(vel.y) < 1.0f) {
        _tileSheet.setUV(0);
        return;
    }

    /* Calculate animation time */
    _animTime += 0.001f * _hitBox.body()->GetLinearVelocity().Length();
    /* Just a precaution */
    if (_animTime > 100.0f)
        _animTime -= 100.0f;

    /* Calculate animation frame */
    /* Module 4, bcs my texture is divided in 4x4, where every row belongs to different animated direction movement */
    int i = (int)_animTime % 4;

    /* Horizontal movement more dominant */
    if (abs(vel.x) > abs(vel.y)) {
        /* Moving right */
        if (vel.x > 0) {
            _tileSheet.setUV(12 + i);
        }
        /* Moving left */
        else {
            _tileSheet.setUV(8 + i);
        }
    }

    /* Vertical movement more dominant */
    else {
        /* Moving up */
        if (vel.y > 0) {
            _tileSheet.setUV(4 + i);
        }
        /* Moving down */
        else {
            _tileSheet.setUV(0 + i);
        }
    }
}

void Player::render(Engine::SpriteBatch& spriteBatch)
{
    if (!isInit())
        return;

    glm::vec2 center = position();
    glm::vec2 position = center - glm::vec2(_renderRadius);
    glm::vec4 destRect = glm::vec4(position, _renderRadius * 2.0f, _renderRadius * 2.0f);

    /* Use velocity vector to calculate the direction vector */
    /* OLD WAY FOR CALCUATING ANIMATION
    auto velo = _hitBox.body()->GetLinearVelocity();
    glm::vec2 vel(velo.x, velo.y);
    static glm::vec2 direction(1.0f, 0.0f);
    if (vel.x != 0.0f || vel.y != 0.0f)
        direction = glm::normalize(glm::vec2(vel.x, vel.y));
    std::cout << vel.x << " " << vel.y << " : " << direction.x << " " << direction.y << std::endl;
    */

    spriteBatch.draw(destRect, _tileSheet.UV(), _tileSheet.getTexture().id, _color, _hitBox.body()->GetAngle());
}

void Player::renderDebug(Engine::DebugRenderer& debugRenderer)
{
    if (!isInit())
        return;

    _hitBox.renderDebug(debugRenderer);
}

void Player::renderDebug(Engine::DebugRenderer& debugRenderer, const Engine::Color& color)
{
    if (!isInit())
        return;

    _hitBox.renderDebug(debugRenderer, color);
}