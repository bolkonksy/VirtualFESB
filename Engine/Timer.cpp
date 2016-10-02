#include "Timer.h"

#include <SDL/SDL_timer.h>

namespace Engine
{
    void FPSCounter::start()
    {
        _startTicks = SDL_GetTicks();
    }

    void FPSCounter::end()
    {
        calculateFPS();
        limitFPS();
    }

    /* No need to comment. Seems complicated, but it's not really */
    void FPSCounter::calculateFPS()
    {   
        static const unsigned int NUM_FRAMES = 16;

        static Uint32 frameTime;
        static Uint32 frameTimes[NUM_FRAMES];
        static unsigned int currentFrame = 0;

        static Uint32 prevTicks = SDL_GetTicks();
        Uint32 currentTicks = SDL_GetTicks();

        frameTime = currentTicks - prevTicks;
        frameTimes[currentFrame % NUM_FRAMES] = frameTime;
        prevTicks = currentTicks;

        size_t count = 0;
        if (currentFrame > NUM_FRAMES)
            count = NUM_FRAMES;
        else
            count = currentFrame;
        currentFrame++;

        float frameTimeAverage = 0.0f;
        for (size_t i = 0; i < count; i++)
            frameTimeAverage += (float)frameTimes[i];
        frameTimeAverage /= (float)count;

        if (frameTimeAverage > 0)
            _fps = 1000.0f / frameTimeAverage;
        else
            _fps = 60.0f;
    }

    void FPSCounter::limitFPS()
    {
        Uint32 frameTicks = SDL_GetTicks() - _startTicks;
        if (1000.0f / _maxFPS > frameTicks)
            SDL_Delay((Uint32)(1000.0f / _maxFPS) - frameTicks);
    }
}