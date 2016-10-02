#include "IGame.h"

#include <iostream>
#include "IGameState.h"
#include "Engine.h"
#include "Timer.h"

namespace Engine
{
    IGame::~IGame()
    {
        quitEngine();
    }

    /* Default paramter for FullScreen is false */
    bool IGame::initGame(const std::string& name, size_t screenWidth, size_t screenHeight, bool fullScreen)
    {
        /* Initialize engine */
        if (Engine::initEngine() == false)
            return false;

        /* Window creation */
        if (!fullScreen) {
            if (window.create(name, screenWidth, screenHeight, 0) == false)
                return false;
        }
        else {
            if (window.create(name, screenWidth, screenHeight, WindowFlag::FULLSCREEN) == false)
                return false;
        }

        /* Setup GameStates */
        if (initGameStates() == false)
            return false;

        /* Get the first GameState from StateManger */
        _activeGameState = _stateManager.getActiveState();
        if (_activeGameState == nullptr)
            return false;

        /* Initialize starting GameState */
        if (_activeGameState->init() == false)
            return false;

        return true;
    }

    void IGame::gameloop()
    {
        FPSCounter fpsCounter(60.0f);
        _isRunning = true;

        while (_isRunning) {
            fpsCounter.start();

            update();
            if (_isRunning)
                render();
            fpsCounter.end();
        }
    }

    void IGame::update()
    {
        eventHandler.update();

        if (_activeGameState != nullptr) {
            switch (_activeGameState->getState()) {
            case GameState::RUNNING:
                _activeGameState->update();
                return;
            /* When GameState chooses changes to this state, it also needs to select next gamestate */
            case GameState::PUSH_STATE:
                _activeGameState->setState(GameState::IDLE);
                _activeGameState = _stateManager.nextState();
                if (_activeGameState != nullptr) {
                    _activeGameState->setState(GameState::RUNNING);
                    _activeGameState->init();
                }
                else
                    break;
                return;
            /* When GameState chooses changes to this state, it also needs to select next gamestate */
            case GameState::POP_STATE:
                _activeGameState->quit();
                _activeGameState->setState(GameState::NONE);
                _activeGameState = _stateManager.nextState();
                if (_activeGameState != nullptr) {
                    if (_activeGameState->getState() != GameState::IDLE)
                        _activeGameState->init();
                    _activeGameState->setState(GameState::RUNNING);
                }
                else
                    break;
                return;
            case GameState::EXIT_ALL:
                std::cout << "Exit application with no error" << std::endl;
                break;
            }
            exit();
        }
    }

    void IGame::render()
    {
        if (_activeGameState != nullptr && _activeGameState->getState() == GameState::RUNNING) {
            _activeGameState->render();
            window.swapBuffer();
        }
    }

    void IGame::handleEvents(const SDL_Event& e)
    {
        switch (e.type) {
        case SDL_QUIT:
            _activeGameState->setState(GameState::EXIT_ALL);
            return;
        case SDL_MOUSEMOTION:
            /* Screen coords */
            eventHandler.setMousePosition((float)e.motion.x, (float)e.motion.y);
            eventHandler.setRelativeMousePosition(e.motion.xrel, e.motion.yrel);
            return;
        case SDL_KEYDOWN:
            eventHandler.pressKey(e.key.keysym.sym);
            return;
        case SDL_KEYUP: 
            eventHandler.releaseKey(e.key.keysym.sym);
            return;
        case SDL_MOUSEBUTTONDOWN:
            eventHandler.pressKey(e.button.button);
            return;
        case SDL_MOUSEBUTTONUP:
            eventHandler.releaseKey(e.button.button);
            return;
        case SDL_MOUSEWHEEL:
            eventHandler.incrementMouseWheel(e.wheel.y);
        }
    }

    /* Make it better */
    void IGame::exit()
    {
        _stateManager.dispose();
        _isRunning = false;
    }
}