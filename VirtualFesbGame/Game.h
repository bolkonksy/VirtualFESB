#ifndef _GAME_H
#define _GAME_H

#ifdef _GAME_MACROS_ENABLED

#define getLanguage_           _game->lang.language()
#define setLanguage_(language) _game->lang.changeLanguage(language)
#define language_(text)        _game->lang.getText(text)  
#define window_                _game->window
#define eventHandler_          _game->eventHandler
#define audio_                 _game->audio

#endif

#include <memory>
#include <Engine/IGame.h>
#include <Engine/Audio/AudioEngine.h>
#include "GameStates/MainMenu.h"
#include "GameStates/Settings.h"
#include "GameStates/Gameplay.h"
#include "GameStates/Editor.h"
#include "GameStates/Pause.h"
#include "Language.h"

class Game : public Engine::IGame {
public:
    Language lang;
    Engine::AudioEngine audio;
    Engine::Music mainBackgroundMusic;
    Engine::Texture backgroundImage;
    std::vector<std::string> stack;         /* Stack for the names of pushed Gamestates */

    virtual bool initGameStates() override;

private:
    std::unique_ptr<MainMenu> _mainMenuGameState = nullptr;
    std::unique_ptr<Settings> _settingsGameState = nullptr;
    std::unique_ptr<Gameplay> _gameplayGameState = nullptr;
    std::unique_ptr<Editor>   _editorGameState   = nullptr;
    std::unique_ptr<Pause>    _pauseGameState    = nullptr;
};

#endif