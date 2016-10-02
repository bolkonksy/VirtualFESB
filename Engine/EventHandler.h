#ifndef _EVENTHANDLER_H
#define _EVENTHANDLER_H

#include <unordered_map>
#include <GLM/glm.hpp>
#include <SDL/SDL_events.h>

namespace Engine
{
    class EventHandler {
    public:
        ~EventHandler();

        void update();

        /* Setters */
        void pressKey(unsigned int keyID);
        void releaseKey(unsigned int keyID);
        void incrementMouseWheel(int mouseWheelChange);
        void setRelativeMousePosition(int x, int y);
        /* Set Mouse position related to screen coordinate system */
        inline void setMousePosition(float x, float y) { _mousePosition.x = x; _mousePosition.y = y; }
        inline void setMousePosition(const glm::vec2& position) { setMousePosition(position.x, position.y); }

        /* Getters*/
        bool keyDown(unsigned int keyID) const;
        bool keyPressed(unsigned int keyID) const;
        bool keyReleased(unsigned int keyID) const;
        void getRelativeMousePosition(int* xRel, int* yRel) { *xRel = _xRel; *yRel = _yRel; }
        int  mouseWheel()       const { return _mouseWheelChange; }
        glm::vec2 mouseCoords() const { return _mousePosition; }

    private:
        std::unordered_map<unsigned int, bool> _keyMap;
        std::unordered_map<unsigned int, bool> _previousKeyMap;
        glm::vec2 _mousePosition = glm::vec2(0.0f);
        Sint32 _mouseWheelChange = 0;
        int _xRel = 0;
        int _yRel = 0;

        bool wasKeyDown(unsigned int keyID) const;
    };
}

#endif