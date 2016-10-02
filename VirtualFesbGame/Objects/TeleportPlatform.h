#ifndef _TELEPORTPLATFORM_H
#define _TELEPORTPLATFORM_H

#include "ISensor.h"
#include "Player.h"

const Engine::Color TELEPORT_COLOR = Engine::Color::Blue();

class TeleportPlatform : public ISensor {
public:
    void init(b2World* world, const glm::vec2& position, const glm::vec2& dimensions, float angle);
    void init(b2World* world, const glm::vec2& position, const glm::vec2& dimensions, float angle,
              const glm::vec2& spawnPoint);

    virtual void renderDebug(Engine::DebugRenderer& debugRenderer) override;
    virtual void renderDebug(Engine::DebugRenderer& debugRenderer, const Engine::Color& color) override;
    
    void teleportPlayer(Player& player)     { if (_hasSpawnPoint)   player.setPosition(_spawnPoint); }

    /* Setters */
    virtual void setActive(bool value) override { _isActive = value; };
    void         setSpawnPoint(const glm::vec2& position) { _spawnPoint = position; _hasSpawnPoint = true; }
    void         removeSpawnPoint() { _hasSpawnPoint = false; }

    /* Getters */
    const bool           hasSpawnPoint() const { return _hasSpawnPoint; }
    const glm::vec2      getSpawnPoint() const { return _spawnPoint; }
    const glm::vec2      position()      const { return _hitBox.position(); }
    const glm::vec2      dimensions()    const { return _hitBox.dimensions(); }
    bool testPoint(float x, float y)     const { return _hitBox.testPoint(x, y); }

private:
    glm::vec2 _spawnPoint    = glm::vec2(0.0f);
    bool      _hasSpawnPoint = false;
};

#endif