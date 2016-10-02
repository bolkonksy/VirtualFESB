#ifndef _ISENSOR_H
#define _ISENSOR_H

#include "CollisionBox.h"

/* Sensor doesn't collide with anything, but it detects collision */
class ISensor {
public:
    /* HitBox Wrapper functions */
    inline void dispose(b2World* world) 
         { _hitBox.dispose(world); };
    inline virtual void renderDebug(Engine::DebugRenderer& debugRenderer) 
         { _hitBox.renderDebug(debugRenderer); }
    inline virtual void renderDebug(Engine::DebugRenderer& debugRenderer, const Engine::Color& color)
         { _hitBox.renderDebug(debugRenderer, color); }

    /* Since this needs to be an interface, user defines this function */
    virtual void setActive(bool value) = 0;

    /* Getters */
    inline bool           getActive()   const { return _isActive; }
    inline CollisionBox   hitBox()      const { return _hitBox; }

protected:
    void initSensor(b2World* world, const glm::vec2& position, const glm::vec2& dimensions, float angle)
         { _hitBox.init(world, dimensions, position, angle, b2_staticBody, true); }
                      
    CollisionBox _hitBox;
    bool _isActive = false;
};

#endif