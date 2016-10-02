#ifndef _PAUSE_H
#define _PAUSE_H

#include <Engine/Render/ShaderProgram.h>
#include <Engine/Render/SpriteBatch.h>
#include <Engine/UserInterface.h>
#include <Engine/IGameState.h>
#include <Engine/Camera.h>

class Game;

class Pause : public Engine::IGameState {
public:
    Pause(Game* game);

    virtual bool init() override;
    virtual void quit() override;

    virtual void update() override;
    virtual void render() override;

private:
    Game* _game;
    Engine::UserInterface _gui;
    Engine::Camera        _guiCamera;

    /* For rendering background image */
    Engine::SpriteBatch   _spriteBatch;
    Engine::ShaderProgram _textureProgram;

    virtual void handleEvents() override;

    /* * * * *  CEGUI  * * * * */
    bool initShaders();
    bool initGUI();
    void setTextOnGUI();

    /* GUI Buttons */
    CEGUI::PushButton* _backToGame = nullptr;
    CEGUI::PushButton* _settingsButton = nullptr;
    CEGUI::PushButton* _toMainMenu = nullptr;

    /* GUI Button Click Events */
    bool backToGameClicked(const CEGUI::EventArgs& e);
    bool settingsClicked  (const CEGUI::EventArgs& e);
    bool toMainMenuClicked(const CEGUI::EventArgs& e);
};

#endif