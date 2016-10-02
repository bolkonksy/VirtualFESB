#include "QuestionPlatform.h"

/* Default paramter for questionFile is false */
void QuestionPlatform::init(b2World* world, const glm::vec2& position, const glm::vec2& dimensions, float angle, 
                            const std::string& questionFile)
{
    _questionFile = questionFile;
    initSensor(world, position, dimensions, angle);

    /* Collidies only with player */
    b2Filter filter = _hitBox.fixture()->GetFilterData();
    filter.categoryBits = (uint16)CollisionFilter::QUESTION_PLATFORM;
    filter.maskBits = (uint16)CollisionFilter::PLAYER;
    _hitBox.fixture()->SetFilterData(filter);
}

bool QuestionPlatform::initGUI(const Engine::UserInterface& gui)
{
    return false;
}