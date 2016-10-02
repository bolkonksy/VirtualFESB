#include "TeleportPlatform.h"

const float SPAWP_POINT_BIG_RADIUS = 7.0f;
const float SPAWP_POINT_SMALL_RADIUS = 5.0f;
const Engine::Color SPAWP_POINT_SMALL_COLOR = Engine::Color::Black();

void TeleportPlatform::init(b2World* world, const glm::vec2& position, const glm::vec2& dimensions, float angle)
{
    _hasSpawnPoint = false;
    initSensor(world, position, dimensions, angle);

    /* Collidies only with the player */
    b2Filter filter = _hitBox.fixture()->GetFilterData();
    filter.categoryBits = (uint16)CollisionFilter::TELEPORT_PLATFORM;
    filter.maskBits = (uint16)CollisionFilter::PLAYER;
    _hitBox.fixture()->SetFilterData(filter);
}

void TeleportPlatform::init(b2World* world, const glm::vec2& position, const glm::vec2& dimensions, float angle,
                            const glm::vec2& spawnPoint)
{
    init(world, position, dimensions, angle);
    setSpawnPoint(spawnPoint);
}

void TeleportPlatform::renderDebug(Engine::DebugRenderer& debugRenderer)
{
    _hitBox.renderDebug(debugRenderer);
    if (_hasSpawnPoint) {
        debugRenderer.drawCircle(_spawnPoint, Engine::Color(255, 255, 255, 127), SPAWP_POINT_BIG_RADIUS);
        debugRenderer.drawCircle(_spawnPoint, SPAWP_POINT_SMALL_COLOR, SPAWP_POINT_SMALL_RADIUS);
    }
}

void TeleportPlatform::renderDebug(Engine::DebugRenderer& debugRenderer, const Engine::Color& color)
{
    _hitBox.renderDebug(debugRenderer, color);
    if (_hasSpawnPoint) {
        debugRenderer.drawCircle(_spawnPoint, color, SPAWP_POINT_BIG_RADIUS);
        debugRenderer.drawCircle(_spawnPoint, SPAWP_POINT_SMALL_COLOR, SPAWP_POINT_SMALL_RADIUS);
    }
}