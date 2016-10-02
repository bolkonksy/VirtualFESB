#include "StateManager.h"

#include "IGameState.h"

namespace Engine
{
    void StateManager::dispose()
    {
        _currentState = -1;
        _nextState    = -1;
        for (auto& gameState : _gameStates)
            gameState->quit();
        _gameStates.resize(0);
        _gameStatesMap.clear();
    }

    IGameState* StateManager::getActiveState()
    {
        if (_currentState == -1)
            return nullptr;
        return _gameStates[_currentState];
    }

    void StateManager::addGameState(IGameState* gameState, const std::string& gameStateName)
    {
        _gameStatesMap[gameStateName] = (int)_gameStates.size();
        _gameStates.push_back(gameState);
    }

    int StateManager::setActiveGameState(const std::string& gameStateName)
    {
        auto it = _gameStatesMap.find(gameStateName);

        if (it == _gameStatesMap.end())
            return -1;

        _currentState = it->second;
        return _currentState;
    }

    int StateManager::setNextGameState(const std::string& gameStateName)
    {
        auto it = _gameStatesMap.find(gameStateName);

        if (it == _gameStatesMap.end())
            return -1;

        _nextState = it->second;
        return _nextState;
    }

    IGameState* StateManager::nextState()
    {
        if (_nextState == -1)
            return nullptr;
        _currentState = _nextState;
        return _gameStates[_currentState];
    }
}