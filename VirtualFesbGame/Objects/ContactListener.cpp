#include "ContactListener.h"

#include "../Level.h"

void ContactListener::BeginContact(b2Contact* contact)
{
    QuestionPlatform* question = nullptr;
    TeleportPlatform* teleport = nullptr;
    
    if (_level == nullptr)
        return;

    if (PlayerAndSensorsContact(contact, question, teleport)) {
        if (question != nullptr)
            question->setActive(true);
        else if (teleport != nullptr)
            /* This will later be used to call TeleportPlayer(). It cannot be called here since this is
               a Box2D callback function which is called during World.step(), during which World is Locked */
            teleport->setActive(true);
    }
}

void ContactListener::EndContact(b2Contact* contact)
{
    QuestionPlatform* question = nullptr;
    TeleportPlatform* teleport = nullptr;

    if (_level == nullptr)
        return;

    if (PlayerAndSensorsContact(contact, question, teleport)) {
        if (question != nullptr)
            question->setActive(false);
        else if (teleport != nullptr)
            /* Since teleport _activeState is used only to teleport player, SetActive(false) is called within that function */
            return;
    }
}

bool ContactListener::PlayerAndSensorsContact(b2Contact* contact, QuestionPlatform*& question, TeleportPlatform*& teleport)
{
    b2Fixture* fixtureA = contact->GetFixtureA();
    b2Fixture* fixtureB = contact->GetFixtureB();

    /* Make sure only one of the fixtures was a sensor */
    bool sensorA = fixtureA->IsSensor();
    bool sensorB = fixtureB->IsSensor();
    if (!(sensorA ^ sensorB))
        return false;

    /* To find to which level object each body belongs, use position for comparing */
    glm::vec2 posA(fixtureA->GetBody()->GetPosition().x, fixtureA->GetBody()->GetPosition().y);
    glm::vec2 posB(fixtureB->GetBody()->GetPosition().x, fixtureB->GetBody()->GetPosition().y);

    if (sensorA) {
        /* BodyB must be a player */
        if (_level->Player.position() != posB)
            return false;

        /* Check Questions for bodyA */
        for (auto& questionPlatform : _level->Questions) {
            if (questionPlatform.position() == posA) {
                question = &questionPlatform;
                return true;
            }
        }

        /* SensorA must be a teleport then */
        for (auto& teleportPlatform : _level->Teleports) {
            if (teleportPlatform.position() == posA) {
                teleport = &teleportPlatform;
                return true;
            }
        }
    }
    else {
        /* BodyA must be a player */
        if (_level->Player.position() != posA)
            return false;

        /* Check Questions for bodyB */
        for (auto& questionPlatform : _level->Questions) {
            if (questionPlatform.position() == posB) {
                question = &questionPlatform;
                return true;
            }
        }

        /* SensorB must be a teleport then */
        for (auto& teleportPlatform : _level->Teleports) {
            if (teleportPlatform.position() == posB) {
                teleport = &teleportPlatform;
                return true;
            }
        }
    }

    return false;
}