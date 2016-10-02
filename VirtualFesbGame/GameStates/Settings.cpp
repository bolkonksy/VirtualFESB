#include "Settings.h"

#define _GAME_MACROS_ENABLED 

#include <Engine/Render/Vertex.h>
#include <Engine/IGame.h>
#include "../Game.h"

Settings::Settings(Game* game) : _game(game) {}

bool Settings::init()
{
    _gameState = Engine::GameState::RUNNING;

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

void Settings::quit()
{
    _spriteBatch.dispose();
    _textureProgram.dispose();
    _gui.dispose();
}

void Settings::update()
{
    _gui.update();
    handleEvents();

    audio_.setVolume((int)_volumeSpinner->getCurrentValue());
}

void Settings::render()
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

void Settings::handleEvents()
{
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        _game->handleEvents(e);
        _gui.handleEvents(e);
    }

    /* If ESC key was hit, return to main menu */
    if (eventHandler_.keyPressed(SDLK_ESCAPE)) {
        CEGUI::EventArgs eventArgs;
        backClicked(eventArgs);
    }
}

bool Settings::initShaders()
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

bool Settings::initGUI()
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
    const glm::vec2 SPINNER_SIZE(60.0f, 35.0f);

    _changeLanguageButton = static_cast<PushButton*>(_gui.createWidget("WindowsLook/Button",
                            glm::vec4(BUTTON_START_POSITION, BUTTON_SIZE), "ChangeLanguage"));
    _changeFullscreen = static_cast<PushButton*>(_gui.createWidget("WindowsLook/Button",
                        glm::vec4(BUTTON_START_POSITION + BUTTON_DISTANCE_Y, BUTTON_SIZE), "ChangeFullscreen"));
    _volumeButton = static_cast<PushButton*>(_gui.createWidget("WindowsLook/Button",
                    glm::vec4(BUTTON_START_POSITION + BUTTON_DISTANCE_Y * 2.0f,
                    BUTTON_SIZE - glm::vec2(SPINNER_SIZE.x, 0.0f)), "VolumeButton"));
    _volumeSpinner = static_cast<Spinner*>(_gui.createWidget("WindowsLook/Spinner", 
                     glm::vec4(BUTTON_START_POSITION + BUTTON_DISTANCE_Y * 2.0f +
                     glm::vec2(BUTTON_SIZE.x - SPINNER_SIZE.x, 0.0f), SPINNER_SIZE), "VolumeSpinner"));
    _backButton = static_cast<PushButton*>(_gui.createWidget("WindowsLook/Button",
                  glm::vec4(BUTTON_START_POSITION + BUTTON_DISTANCE_Y * 3.0f, BUTTON_SIZE), "BackButton"));

    if (_changeLanguageButton == nullptr || _changeFullscreen == nullptr || _backButton == nullptr ||
        _volumeButton == nullptr || _volumeSpinner == nullptr)
        return false;

    setTextOnGUI();

    /* This button is used for its fancy text box only */
    _volumeButton->disable();
    _volumeButton->setProperty("DisabledTextColour", "FF000000"); /* Same color as for the enabled button */

    _volumeSpinner->setTextInputMode(CEGUI::Spinner::Integer);
    _volumeSpinner->setMaximumValue(100);
    _volumeSpinner->setMinimumValue(0);
    _volumeSpinner->setCurrentValue(round(audio_.getVolume()));

    _changeLanguageButton->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&Settings::changeLanguageClicked, this));
    _changeFullscreen->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&Settings::changeFullscreenClicked, this));
    _backButton->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&Settings::backClicked, this));

    return true;
}

void Settings::setTextOnGUI()
{
    _changeLanguageButton->setText(language_("GET_LANGUAGE"));
    std::string OnOrOff = window_.isFullscreen() ? language_("ON"): language_("OFF");
    _changeFullscreen->setText(language_("FULLSCREEN") + ": " + OnOrOff.c_str());
    _backButton->setText(language_("GO_BACK"));
    _volumeButton->setText(language_("VOLUME"));
}

bool Settings::changeLanguageClicked(const CEGUI::EventArgs& e)
{
    /* Toggle language */
    if      (getLanguage_ == Lang::ENG) setLanguage_(Lang::CRO);
    else if (getLanguage_ == Lang::CRO) setLanguage_(Lang::ENG);
    /* Now reset text on gui */
    setTextOnGUI();

    return true;
}

bool Settings::changeFullscreenClicked(const CEGUI::EventArgs& e)
{
    /* Toggle fullscreen */
    bool isFullscreen = window_.isFullscreen();
    window_.setFullscreen(isFullscreen ? false : true);

    quit();
    init();
    return true;
}

bool Settings::backClicked(const CEGUI::EventArgs& e)
{
    _gameState = Engine::GameState::POP_STATE;
    _game->stack.pop_back();

    /* Before popping Settings */
    /* Stack should have looked like this:    MainMenu -> Settings */
    if (_game->stack.back() == "MainMenu")
        _game->nextGameState("MainMenu");
    /* Or like this:    Gameplay -> Pause -> Settings */
    else if (_game->stack.back() == "Pause")
        _game->nextGameState("Pause");

    return true;
}

#undef GAME_MACROS_ENABLED