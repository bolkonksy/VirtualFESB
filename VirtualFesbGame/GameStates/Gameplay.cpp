#include "Gameplay.h"

#include <Engine/Render/Vertex.h>
#include <Engine/IGame.h>
#include "../Game.h"

Gameplay::Gameplay(Game* game) : _game(game) {}

bool Gameplay::init()
{
    _gameState = Engine::GameState::RUNNING;
    window_.setBackgroundColor(Engine::Color::Black());

    if (_debugRenderer.init() == false)
        return false;
    if (_spriteBatch.init() == false)
        return false;

    if (initLevel() == false)
        return false;
    if (initShaders() == false)
        return false;
    if (initGUI() == false)
        return false;

    _examGUI.init(_gui, questions_.size(), _game);

    return true;
}

void Gameplay::quit()
{
    _debugRenderer.dispose();
    _spriteBatch.dispose();
    _textureProgram.dispose();
    _gui.dispose();

    _level.dispose();
}

void Gameplay::update()
{
    _gui.update();
    handleEvents();

    /* No world update during the exam */
    if (_examGUI.isActive()) {
        _examGUI.update();
        return;
    }

    player_.update(eventHandler_);
    _camera.setPosition(player_.position());

    world_->Step(1.0f / 60.0f, 6, 2);
    _level.updateTeleports();
}

void Gameplay::render()
{
    /* Exception when returning to MainMenu from Pause */
    if (_game->stack.back() == "MainMenu")
        return;
    /* If language was changed during Pause GameState, refresh texts */
    else if (_refreshLanguage == true) {
        initGUIText();
        _refreshLanguage = false;
    }

    _textureProgram.use();
    
    GLint textureUniform;
    if (_textureProgram.getUniformLocation("mySampler", textureUniform) == true) {
        glUniform1i(textureUniform, 0);
        glActiveTexture(GL_TEXTURE0);
    }

    glm::mat4 projectionMatrix = _camera.cameraMatrix();
    GLint pUniform;
    if (_textureProgram.getUniformLocation("P", pUniform) == true)
        glUniformMatrix4fv(pUniform, 1, GL_FALSE, &projectionMatrix[0][0]);

    _spriteBatch.begin(Engine::SpriteSort::BACK_TO_FRONT);

    for (auto& floor : floors_)
        floor.render(_spriteBatch);
    for (auto& wall : walls_)
        wall.render(_spriteBatch);
    player_.render(_spriteBatch);

    _spriteBatch.end();
    _textureProgram.unuse();

    if (_debugMode)
        renderDebug(projectionMatrix);
    renderGUI();
}

bool Gameplay::initLevel()
{
    if (_level.init("fesb.txt") == false && _level.init() == false)
        return false;

    _camera.init(window_.width(), window_.height(), 1.0f);
    _camera.setPosition(player_.position());

    return true;
}

bool Gameplay::initShaders()
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

bool Gameplay::initGUI()
{
    _gui.init("GUI");
    _gui.setScheme("TaharezLook");
    _gui.setMouseCursor("TaharezLook/MouseArrow");
    _gui.showMouseCursor(Engine::MouseCursor::SHOW);
    _gui.setFont("DejaVuSans-12");

    using namespace CEGUI;

    /* Button Constants */
    const glm::vec2 BUTTON_SIZE(100.0f, 24.0f);
    const glm::vec2 SCREEN_SIZE(window_.width(), window_.height());

    /* Notify button */
    {
        const glm::vec2 NOTIFY_BUTTON_POS = SCREEN_SIZE - BUTTON_SIZE - glm::vec2(2.0f);

        _notifyButton = static_cast<PushButton*>(_gui.createWidget("TaharezLook/Button",
                        glm::vec4(NOTIFY_BUTTON_POS, BUTTON_SIZE), "NotifyButton"));
        if (_notifyButton == nullptr)
            return false;

        _notifyButton->setAlpha(0.5f);
        _notifyButton->setDisabled(true);
        _notifyButton->setAlwaysOnTop(true);
        _notifyButton->setMouseInputPropagationEnabled(false);
        _notifyButton->setMousePassThroughEnabled(true);
        _notifyButton->setProperty("NormalTextColour", "FFFF4444");
    }

    return initGUIText();
}

bool Gameplay::initGUIText()
{
    if (_notifyButton == nullptr)
        return false;

    _notifyButton->setText(language_("PRESS_BUTTON_E"));

    return true;
}

void Gameplay::handleEvents()
{
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        _game->handleEvents(e);
        _gui.handleEvents(e);
    }

    if (eventHandler_.keyPressed(SDLK_F3))
        _debugMode = !_debugMode;
    else if (eventHandler_.keyPressed(SDLK_e) && _level.activeQuestion() != -1)
        _examGUI.activate(_level.Questions[_level.activeQuestion()], getLanguage_);

    /***** This is a special condition when player decides do quit to the MainMenu from Pause GameState *****/
    /* If that's the case, on top of the gamestates stack should be "MainMenu" instead of the usual "Gameplay" */
    /* --See Pause::toTheMainMenuClicked function for more info */
    if (_game->stack.back() == "MainMenu") {
        _gameState = Engine::GameState::POP_STATE;
        _game->nextGameState("MainMenu");
    }
    /*** endif ***/

    else if (eventHandler_.keyPressed(SDLK_ESCAPE)) {
        /* If ESC key was hit during the exam, cancel the exam */
        if (_examGUI.isActive()) {
            _examGUI.cancelExam();
            return;
        }

        /* If ESC key was hit during the gameplay, go to Pause GameState */
        _refreshLanguage = true;

        _game->stack.push_back("Pause");
        _game->nextGameState("Pause");
        _gameState = Engine::GameState::PUSH_STATE;
    }
}

void Gameplay::renderDebug(const glm::mat4& projectionMatrix)
{
    for (auto& floor : floors_)
        floor.renderDebug(_debugRenderer);
    for (auto& wall : walls_)
        wall.renderDebug(_debugRenderer);
    for (auto& question : questions_)
        question.renderDebug(_debugRenderer, QUESTION_COLOR);
    for (auto& teleport : teleports_)
        teleport.renderDebug(_debugRenderer, TELEPORT_COLOR);
    player_.renderDebug(_debugRenderer);

    _debugRenderer.setProjectionMatrix(projectionMatrix);
    _debugRenderer.end();
}

void Gameplay::renderGUI()
{
    if (_examGUI.isActive()) {
        _notifyButton->hide();
    }
    else {
        _notifyButton->show();

        /* Render _notifyButton blinking animation ("Press E") */
        static float alpha = 0.5f;
        if (_level.activeQuestion() != -1) {
            const float ALPHA_INCREASE = 0.015f;

            alpha += ALPHA_INCREASE;
            /* Alpha always increases from 0.5f to 1.0f */
            if (alpha > 1.0f) {
                _notifyButton->setProperty("NormalTextColour", "DDDD4444");
                alpha = 0.5f;
            }
            else if (alpha > 0.8f)
                _notifyButton->setProperty("NormalTextColour", "DDDD2222");
            else if (alpha > 0.6f)
                _notifyButton->setProperty("NormalTextColour", "DDDD0000");

            _notifyButton->setAlpha(alpha);
            _notifyButton->setEnabled(true);
        }
        else {
            _notifyButton->setEnabled(false);
            alpha = 0.5f;
            _notifyButton->setAlpha(alpha);
        }
    }

    _gui.render();
}

#undef _GAME_MACROS_ENABLED
#undef _LEVEL_MACROS_ENABLED