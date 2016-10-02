#ifndef _SETTINGS_H
#define _SETTINGS_H

#include <Engine/Render/ShaderProgram.h>
#include <Engine/Render/SpriteBatch.h>
#include <Engine/UserInterface.h>
#include <Engine/IGameState.h>
#include <Engine/Camera.h>

class Game;

class Settings : public Engine::IGameState {
public:
    Settings(Game* game);

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
    CEGUI::PushButton* _changeLanguageButton = nullptr;
    CEGUI::PushButton* _changeFullscreen     = nullptr;
    CEGUI::PushButton* _backButton           = nullptr;
    CEGUI::PushButton* _volumeButton         = nullptr; /* Used just for the text box, not button funcionality */
    CEGUI::Spinner*    _volumeSpinner        = nullptr;

    /* GUI Button Click Events */
    bool changeLanguageClicked   (const CEGUI::EventArgs& e);
    bool changeFullscreenClicked (const CEGUI::EventArgs& e);
    bool backClicked             (const CEGUI::EventArgs& e);
};

#endif