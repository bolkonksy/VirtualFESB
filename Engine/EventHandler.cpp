#include "EventHandler.h"

namespace Engine
{
    EventHandler::~EventHandler()
    {
        _keyMap.clear();
        _previousKeyMap.clear();
    }

    void EventHandler::update()
    {
        /* Update _previousKeyMap */
        for (auto& it : _keyMap)
            _previousKeyMap[it.first] = it.second;

        /* Reset _mouseWheelChange to zero */
        _mouseWheelChange = 0;
        
        /* Reset mouse relative position */
        _xRel = 0;
        _yRel = 0;
    }

    void EventHandler::pressKey(unsigned int keyID)
    {
        _keyMap[keyID] = true;
    }

    void EventHandler::releaseKey(unsigned int keyID)
    {
        _keyMap[keyID] = false;
    }

    void EventHandler::incrementMouseWheel(int mouseWheelChange)
    {
        _mouseWheelChange += mouseWheelChange;
    }

    void EventHandler::setRelativeMousePosition(int x, int y)
    {
        _xRel = x;
        _yRel = y;
    }

    bool EventHandler::keyDown(unsigned int keyID) const
    {
        auto it = _keyMap.find(keyID);
        if (it != _keyMap.end())
            return it->second;
        else
            return false;
    }

    bool EventHandler::keyPressed(unsigned int keyID) const
    {
        /* Key is pressed, if it is pressed now, but it wasn't pressed previous frame */
        if (keyDown(keyID) == true && wasKeyDown(keyID) == false)
            return true;
        return false;
    }

    bool EventHandler::keyReleased(unsigned int keyID) const
    {
        if (keyDown(keyID) == false && wasKeyDown(keyID) == true)
            return true;
        return false;
    }

    bool EventHandler::wasKeyDown(unsigned int keyID) const
    {
        auto it = _previousKeyMap.find(keyID);
        if (it != _previousKeyMap.end())
            return it->second;
        return false;
    }
}