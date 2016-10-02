#ifndef _STATEMANAGER_H
#define _STATEMANAGER_H

#include <vector>
#include <string>
#include <unordered_map>

namespace Engine
{
    class IGame;
    class IGameState;

    class StateManager {
    public:
        void dispose();

        void addGameState(IGameState* newScreen, const std::string& gameStateName);
        int setActiveGameState(const std::string& gameStateName);
        int setNextGameState(const std::string& gameStateName);
        
        /* Replaces current state with the next state and returns */
        IGameState* nextState();
        /* Always returns the current state */
        IGameState* getActiveState();

    protected:
        int _currentState = -1;
        int _nextState    = -1;

        std::vector<IGameState*> _gameStates;
        std::unordered_map<std::string, int> _gameStatesMap;
    };
}

#endif