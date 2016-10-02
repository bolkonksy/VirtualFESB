#include "Pause.h"

#define _GAME_MACROS_ENABLED 

#include <Engine/Render/Vertex.h>
#include <Engine/IGame.h>
#include "../Game.h"

Pause::Pause(Game* game) : _game(game) {}

bool Pause::init()
{
    _gameState = Engine::GameState::RUNNING;
    window_.setBackgroundColor(Engine::Color::Green());

    _guiCamera.init(window_.width(), window_.height(), 1.0f);
    _guiCamera.setPosition(glm::vec2(window_.width(2.0f), window_.height(2.0f)));

    if (_spriteBatch.init() == false)
        return false;
    if (initShaders() == false)
        return false;
    if (initGUI() == false)
        return false;

    return true;
}

void Pause::quit()
{
    _spriteBatch.dispose();
    _textureProgram.dispose();
    _gui.dispose();
}

void Pause::update()
{
    _gui.update();
    handleEvents();
}

void Pause::render()
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

void Pause::handleEvents()
{
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        _game->handleEvents(e);
        _gui.handleEvents(e);
    }

    /* If ESC key was hit, return to main menu */
    if (eventHandler_.keyPressed(SDLK_ESCAPE)) {
        CEGUI::EventArgs eventArgs;
        backToGameClicked(eventArgs);
    }
}

bool Pause::initShaders()
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

bool Pause::initGUI()
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

    _backToGame     = static_cast<PushButton*>(_gui.createWidget("WindowsLook/Button",
            glm::vec4(BUTTON_START_POSITION, BUTTON_SIZE), "backToGame"));
    _settingsButton = static_cast<PushButton*>(_gui.createWidget("WindowsLook/Button",
            glm::vec4(BUTTON_START_POSITION + BUTTON_DISTANCE_Y, BUTTON_SIZE), "settingsButton"));
    _toMainMenu     = static_cast<PushButton*>(_gui.createWidget("WindowsLook/Button",
            glm::vec4(BUTTON_START_POSITION + BUTTON_DISTANCE_Y * 2.0f, BUTTON_SIZE), "toMainMenu"));

    if (_backToGame == nullptr || _settingsButton == nullptr || _toMainMenu == nullptr)
        return false;

    setTextOnGUI();

    _backToGame->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&Pause::backToGameClicked, this));
    _settingsButton->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&Pause::settingsClicked, this));
    _toMainMenu->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&Pause::toMainMenuClicked, this));

    return true;
}

void Pause::setTextOnGUI()
{
    _backToGame->setText(language_("BACK_TO_GAME"));
    _settingsButton->setText(language_("GAME_SETTINGS"));
    _toMainMenu->setText(language_("MAIN_MENU"));
}

bool Pause::backToGameClicked(const CEGUI::EventArgs& e)
{
    _gameState = Engine::GameState::POP_STATE;
    _game->nextGameState("Gameplay");

    /* Stack before popping:    Gameplay -> Pause */
    _game->stack.pop_back(); /* Pop Pause */

    /* Stack now:    Gameplay */  
    window_.setBackgroundColor(Engine::Color::Black());
    return true;
}

bool Pause::settingsClicked(const CEGUI::EventArgs& e)
{
    /* POP_STATE is not an error here, rather a hacky solution, 
        _game->stack still keeps logical a gamestates path */
    _gameState = Engine::GameState::POP_STATE;
    _game->nextGameState("Settings");
    /* Stack before pushing:    MainMenu */  

    /* Stack now:    MainMenu -> Settings */  
    _game->stack.push_back("Settings");
    return true;
}

bool Pause::toMainMenuClicked(const CEGUI::EventArgs& e)
{
    _gameState = Engine::GameState::POP_STATE;
    /* In this case, Gameplay will pop itself from its init function */
    _game->nextGameState("Gameplay");

    /* Stack before popping:    GamePlay -> Pause */
    _game->stack.pop_back(); /* Pop Pause */
    _game->stack.pop_back(); /* Pop Gameplay */

    /* Stack now:    MainMenu*/
    _game->stack.push_back("MainMenu");

    /* Avoid coloring screen to green in transition between switching states */
    render();

    return true;
}

#undef GAME_MACROS_ENABLED