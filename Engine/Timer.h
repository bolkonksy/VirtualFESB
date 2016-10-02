#ifndef _TIMER_H
#define _TIMER_H

#include <SDL/SDL_stdinc.h>

namespace Engine
{
    class FPSCounter {
    public:
        FPSCounter() {}
        FPSCounter(float maxFPS) { setMaxFPS(maxFPS); }

        /* Should be placed at the begining of the gameloop */
        void start();

        /* Calculates FPS
         * Should be put on the end of the gameloop */
        void end();
        
        /* Setters */
        inline void setMaxFPS(float maxFPS) { maxFPS <= 0 ? _maxFPS = 60.0f : _maxFPS = maxFPS; }

        /* Getters */
        float currentFPS() const { return _fps; }

    private:
        float   _fps    = 0.0f;
        float   _maxFPS = 60.0f;
        Uint32  _startTicks;

        void calculateFPS();
        void limitFPS();
    };
}

#endif