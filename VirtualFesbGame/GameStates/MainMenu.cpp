#include "MainMenu.h"

#define _GAME_MACROS_ENABLED 

#include <Engine/Render/Vertex.h>
#include <Engine/IGame.h>
#include "../Game.h"

MainMenu::MainMenu(Game* game) : _game(game) {}

bool MainMenu::init()
{
    _gameState = Engine::GameState::RUNNING;
    window_.setBackgroundColor(Engine::Color::Blue());

    _guiCamera.init(window_.width(), window_.height(), 1.0f);
    _guiCamera.setPosition(glm::vec2(window_.width(2.0f), window_.height(2.0f)));
    _game->mainBackgroundMusic.play(-1);

    if (_spriteBatch.init() == false)
        return false;
    if (initShaders() == false)
        return false;
    if (initGUI() == false)
        return false;

    return true;
}

void MainMenu::quit()
{
    _spriteBatch.dispose();
    _textureProgram.dispose();
    _gui.dispose();

    if (_gameState == Engine::GameState::EXIT_ALL || _game->stack.empty())
        audio_.destroy();
}

void MainMenu::update()
{
    _gui.update();
    handleEvents();
}

void MainMenu::render()
{
    _textureProgram.use();
    
    GLint textureUniform;
    if (_textureProgram.getUniformLocation("mySampler", textureUniform) == true) {
        glUniform1i(textureUniform, 0);
        glActiveTexture(GL_TEXTURE0);
    }

    glm::mat4 projectionMatrix = _guiCamera.cameraMatrix();
    GLint pUniform;
    if (_textureProgram.getUniformLocation("P", pUniform) == true)
        glUniformMatrix4fv(pUniform, 1, GL_FALSE, &projectionMatrix[0][0]);

    _spriteBatch.begin();

    const glm::vec4 window(0.0f, 0.0f, window_.width(), window_.height());
    const glm::vec4 uv(0.0f, 0.0f, 1.0f, 1.0f);
    const Engine::Color none(255, 255, 255, 255);
    _spriteBatch.draw(window, uv, _game->backgroundImage.id, none, 0.0f);

    _spriteBatch.end();
    _textureProgram.unuse();

    _gui.render();
}

void MainMenu::handleEvents()
{
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        _game->handleEvents(e);
        _gui.handleEvents(e);
    }
}

bool MainMenu::initShaders()
{
    if (_textureProgram.compileShadersFromFile("Shaders/textureShading.vert",
        "Shaders/textureShading.frag") == false)
        return false;
    _textureProgram.bindAttribute("vertexPosition");
    _textureProgram.bindAttribute("vertexColor");
    _textureProgram.bindAttribute("vertexUV");
    if (_textureProgram.linkShaders() == false)
        return false;
    return true;
}

bool MainMenu::initGUI()
{
    _gui.init("GUI");
    _gui.setScheme("WindowsLook");
    _gui.setMouseCursor("WindowsLook/MouseArrow");
    _gui.showMouseCursor(Engine::MouseCursor::SHOW);
    _gui.setFont("DejaVuSans-12");

    using namespace CEGUI;

    const glm::vec2 BUTTON_SIZE           = glm::vec2(150.0f, 35.0f);
    const glm::vec2 BUTTON_DISTANCE_Y     = glm::vec2(0, 20.0f) + glm::vec2(0.0f, BUTTON_SIZE.y);
    const glm::vec2 BUTTON_START_POSITION = glm::vec2(window_.width(2.0f), 260.0f) - BUTTON_SIZE / 2.0f;

    _playButton     = static_cast<PushButton*>(_gui.createWidget("WindowsLook/Button",
            glm::vec4(BUTTON_START_POSITION, BUTTON_SIZE), "play"));
    _editorButton   = static_cast<PushButton*>(_gui.createWidget("WindowsLook/Button",
            glm::vec4(BUTTON_START_POSITION + BUTTON_DISTANCE_Y, BUTTON_SIZE), "editor"));
    _settingsButton = static_cast<PushButton*>(_gui.createWidget("WindowsLook/Button", 
            glm::vec4(BUTTON_START_POSITION + BUTTON_DISTANCE_Y * 2.0f, BUTTON_SIZE), "settings"));
    _exitButton     = static_cast<PushButton*>(_gui.createWidget("WindowsLook/Button",
            glm::vec4(BUTTON_START_POSITION + BUTTON_DISTANCE_Y * 3.0f, BUTTON_SIZE), "exit"));

    if (_playButton == nullptr || _editorButton == nullptr || _settingsButton == nullptr || _exitButton == nullptr)
        return false;

    _playButton->setText(language_("START_GAME"));
    _editorButton->setText(language_("LEVEL_EDITOR"));
    _settingsButton->setText(language_("GAME_SETTINGS"));
    _exitButton->setText(language_("EXIT"));

    _playButton->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&MainMenu::newGameClicked, this));
    _editorButton->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&MainMenu::levelEditorClicked, this));
    _settingsButton->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&MainMenu::settingsClicked, this));
    _exitButton->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&MainMenu::exitGameClicked, this));

    return true;
}

bool MainMenu::newGameClicked(const CEGUI::EventArgs& e)
{
    _gameState = Engine::GameState::POP_STATE;
    _game->stack.pop_back();
    /* Stack before popping:    MainMenu */

    /* Stack now:   Gameplay */
    _game->stack.push_back("Gameplay");
    _game->nextGameState("Gameplay");
    return true;
}

bool MainMenu::levelEditorClicked(const CEGUI::EventArgs& e)
{
    _gameState = Engine::GameState::POP_STATE;
    _game->stack.pop_back();
    /* Stack before popping:    MainMenu */

    /* Stack now:   Editor */
    _game->stack.push_back("Editor");
    _game->nextGameState("Editor");
    return true;
}

bool MainMenu::settingsClicked(const CEGUI::EventArgs& e)
{
    _gameState = Engine::GameState::POP_STATE;
    /* Stack before pushing:    MainMenu */  

    /* Stack now:    MainMenu -> Settings */  
    _game->stack.push_back("Settings");
    _game->nextGameState("Settings");
    return true;
}

bool MainMenu::exitGameClicked(const CEGUI::EventArgs& e)
{
    _game->stack.pop_back();
    _gameState = Engine::GameState::EXIT_ALL;
    return true;
}

#undef GAME_MACROS_ENABLED