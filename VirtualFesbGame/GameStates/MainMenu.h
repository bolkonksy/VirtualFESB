#ifndef _MAINMENU_H
#define _MAINMENU_H

#include <Engine/Render/ShaderProgram.h>
#include <Engine/Render/SpriteBatch.h>
#include <Engine/UserInterface.h>
#include <Engine/IGameState.h>
#include <Engine/Camera.h>

class Game;

class MainMenu : public Engine::IGameState {
public:
    MainMenu(Game* game);

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

    /* GUI Buttons */
    CEGUI::PushButton* _playButton     = nullptr;
    CEGUI::PushButton* _editorButton   = nullptr;
    CEGUI::PushButton* _settingsButton = nullptr;
    CEGUI::PushButton* _exitButton     = nullptr;

    /* GUI Button Click Events */
    bool newGameClicked     (const CEGUI::EventArgs& e);
    bool levelEditorClicked (const CEGUI::EventArgs& e);
    bool settingsClicked    (const CEGUI::EventArgs& e);
    bool exitGameClicked    (const CEGUI::EventArgs& e);
};

#endif