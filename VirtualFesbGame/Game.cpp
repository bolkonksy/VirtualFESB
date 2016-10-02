#include "Game.h"

#include <Engine/IO/ResourceManager.h>

bool Game::initGameStates()
{
    lang.changeLanguage(Lang::CRO);

    if (audio.init() == false)  /* Dispose is called when quiting game in MainMenu::quit() */
        return false;

    mainBackgroundMusic = audio.loadMusic("Music/test_2.ogg");

    backgroundImage = Engine::ResourceManager::getTexture("Textures/Static/fesb.png");

    _mainMenuGameState = std::make_unique<MainMenu>(this);
    _settingsGameState = std::make_unique<Settings>(this);
    _gameplayGameState = std::make_unique<Gameplay>(this);
    _editorGameState   = std::make_unique<Editor>  (this);
    _pauseGameState    = std::make_unique<Pause>   (this);

    _stateManager.addGameState(_mainMenuGameState.get(), "MainMenu");
    _stateManager.addGameState(_settingsGameState.get(), "Settings");
    _stateManager.addGameState(_gameplayGameState.get(), "Gameplay");
    _stateManager.addGameState(_editorGameState.get(),   "Editor");
    _stateManager.addGameState(_pauseGameState.get(),    "Pause");

    if (_stateManager.setActiveGameState("MainMenu") == -1)
        return false;
    stack.reserve(12);   /* Well, probably won't need more than 6, but just in case */
    stack.push_back("MainMenu");

    return true;
}