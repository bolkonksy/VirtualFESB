#ifndef _CONTACTLISTENER_H
#define _CONTACTLISTENER_H

#include "QuestionPlatform.h"
#include "TeleportPlatform.h"

class Level;

/* I lost 4 hours trying to make this collision work. Ended up using hacky access to Level objects */
class ContactListener : public b2ContactListener {
public:
    void init(Level* level) { _level = level; }

    void BeginContact(b2Contact* contact);
    void EndContact(b2Contact* contact);

private:
    bool PlayerAndSensorsContact(b2Contact* contact, QuestionPlatform*& question, TeleportPlatform*& teleport);

    /* To use level objects which are not dynamically allocated */
    Level* _level = nullptr;
};

#endif