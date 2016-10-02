#ifndef _GAMEPLAY_H
#define _GAMEPLAY_H

#define _GAME_MACROS_ENABLED  /* Defined in Game.h */
#define _LEVEL_MACROS_ENABLED /* Defined in Level.h */

#include <Box2d/Box2D.h>
#include <Engine/Render/DebugRenderer.h>
#include <Engine/Render/ShaderProgram.h>
#include <Engine/Render/SpriteBatch.h>
#include <Engine/UserInterface.h>
#include <Engine/IGameState.h>
#include <Engine/Camera.h>
#include "../ExamGUI.h"
#include "../Level.h"

class Game;

class Gameplay : public Engine::IGameState {
public:
    Gameplay(Game* game);

    virtual bool init() override;
    virtual void quit() override;

    virtual void update() override;
    virtual void render() override;

private:
    /* Init functions */
    bool initLevel();
    bool initShaders();
    bool initGUI();
    bool initGUIText();

    /* Update functions */
    virtual void handleEvents() override;

    /* Render functions */
    void renderDebug(const glm::mat4& projectionMatrix);
    void renderGUI();

    /* * * Members * * */
    Game*   _game;
    Level   _level;
    ExamGUI _examGUI;

    bool _debugMode       = false;
    bool _refreshLanguage = false; /* Triggered when returning from Pause GameState */

    Engine::UserInterface _gui;
    Engine::Camera        _camera;

    /* SpriteBatch and ShaderProgram */
    Engine::SpriteBatch   _spriteBatch;
    Engine::ShaderProgram _textureProgram;
    Engine::DebugRenderer _debugRenderer;

    /* * * * *  CEGUI  * * * * */

    /* Activate Notification */
    CEGUI::PushButton* _notifyButton = nullptr;
};

#endif