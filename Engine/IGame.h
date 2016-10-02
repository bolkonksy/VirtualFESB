#ifndef _IGAME_H
#define _IGAME_H

#include <SDL/SDL_events.h>
#include "StateManager.h"
#include "EventHandler.h"
#include "Window.h"

namespace Engine
{
    class StateManager;
    class IGameState;

    class IGame {
    public:
        EventHandler eventHandler;
        Window       window;

        virtual ~IGame();

        bool initGame(const std::string& name, size_t screenWidth, size_t screenHeight, bool fullScreen = false);
        void gameloop();
        
        void handleEvents(const SDL_Event& e);

        inline int nextGameState(const std::string& gamestate) { return _stateManager.setNextGameState(gamestate); }

        virtual bool initGameStates() = 0;

    protected:
        void exit();

        virtual void update();
        virtual void render();

        IGameState*  _activeGameState = nullptr; /* make unique*/
        StateManager _stateManager;
        bool         _isRunning       = false;
    };
}

#endif