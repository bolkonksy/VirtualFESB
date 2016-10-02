#include "Editor.h"

#define _HIDE_SAVE_WINDOW   _saveWindow->hide(); \
                            _saveWindow->disable();
#define _SHOW_SAVE_WINDOW   _saveWindow->show(); \
                            _saveWindow->enable();
#define _HIDE_LOAD_WINDOW   _loadWindow->hide(); \
                            _loadWindow->disable();
#define _SHOW_LOAD_WINDOW   _loadWindow->show(); \
                            _loadWindow->enable();
#define _HIDE_NEW_LEVEL_WINDOW   _newLevelWindow->hide(); \
                                 _newLevelWindow->disable();
#define _SHOW_NEW_LEVEL_WINDOW   _newLevelWindow->show(); \
                                 _newLevelWindow->enable();

#define _SET_SELECT_MODE    _mouseMode = MouseMode::SELECT; \
                            _object = Object::NONE;         \
                            _objectID = NO_ID;              \
                            _isPlacingSpawn = false;

#define _ENABLE_ALL_WIGDETS _colorGroup->enable(); \
                            _rectSizeGroup->enable(); \
                            _fixedUV->enable(); \
                            _uvScaleSpinner->enable(); \
                            _texture_Combobox->enable(); \
                            _teleportGroup->enable(); \
                            _questionGroup->enable();

#define _TO_RADIANS     (float)M_PI / 180.0f
#define _TO_DEGREES     180.0f / (float)M_PI

#define _GET_SLIDERS_POSITION_VALUE \
glm::vec2(_xPosSpinner->getCurrentValue(), _yPosSpinner->getCurrentValue())

#define _GET_SLIDERS_COLOR \
Engine::Color((GLubyte)_rColor->getCurrentValue(), (GLubyte)_gColor->getCurrentValue(), \
              (GLubyte)_bColor->getCurrentValue(), (GLubyte)_aColor->getCurrentValue())

#define _SELECTED_OBJECT_DEBUG_COLOR \
Engine::Color((GLubyte)255, (GLubyte)255, (GLubyte)0, (GLubyte)200)




#include <SDL/SDL.h>
#include <Engine/IGame.h>
#include <Engine/IO/ResourceManager.h>
#include <Engine/IO/IOManager.h>
#include <Engine/Render/Vertex.h>
#include "../Game.h"

/* _colorGroup constants */
const float MAX_COLOR_VALUE = 255.0f;
const float SIZE_SPINNER_DEFAULT = 30.0f;
const float ROTATION_SPINNER_DEFAULT = 0.0f;
const glm::vec2 COLOR_PICKER_SIZE(45.0f);

const std::string PLAYER_TEXTURE_PATH = "Textures/static/Human.png";
const std::string WALL_TEXTURE_PATH   = "Textures/blank.png";
const std::string FLOOR_TEXTURE_PATH  = "Textures/blank.png";

Editor::Editor(Game* game) : _game(game) {}

bool Editor::init()
{
    _gameState = Engine::GameState::RUNNING;
    window_.setBackgroundColor(Engine::Color(76, 76, 76));

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

    _SET_SELECT_MODE
    return true;
}

void Editor::quit()
{
    _debugRenderer.dispose();
    _spriteBatch.dispose();
    _textureProgram.dispose();
    _gui.dispose();

    for (auto& item : _saveWindow_ListBoxItems)
        delete item;
    _saveWindow_ListBoxItems.clear();
    for (auto& item : _texture_ListBoxItems)
        delete item;
    _texture_ListBoxItems.clear();
    for (auto& item : _question_ListBoxItems)
        delete item;
    _question_ListBoxItems.clear();

    _level.clearAllObjects();
}

void Editor::update()
{
    _gui.update();
    handleEvents();

    updateMouseActivity();
}

void Editor::render()
{
    renderLevel();
    renderGUI();
}

bool Editor::initLevel()
{
    if (_level.init("fesb.txt") == false && _level.init() == false)
        return false;

    _camera.init(window_.width(), window_.height(), 1.0f);
    _guiCamera.init(window_.width(), window_.height(), 1.0f);
    _camera.setPosition(player_.position());
    _guiCamera.setPosition(glm::vec2(window_.width(2.0f), -window_.height(2.0f)));
    
    return true;
}

bool Editor::initShaders()
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

bool Editor::initGUI()
{
    _gui.init("GUI");
    _gui.setScheme("TaharezLook");
    _gui.setMouseCursor("TaharezLook/MouseArrow");
    _gui.showMouseCursor(Engine::MouseCursor::SHOW);
    _gui.setFont("DejaVuSans-12");

    using namespace CEGUI;

    /* Button Constants */
    const glm::vec2 BUTTON_SIZE       = glm::vec2(150.0f, 28.0f);
    const glm::vec2 BUTTON_DISTANCE_X = glm::vec2(10 + BUTTON_SIZE.x, 0.0f);
    const glm::vec2 BUTTON_POSITION   = glm::vec2(2.0f);
    
    const glm::vec2 EDITOR_SETTINGS_SIZE(310.0f, 522.0f);

    /* Add frame window back panel */
    _editorSettings = static_cast<FrameWindow*>(_gui.createWidget("TaharezLook/FrameWindow",
                      glm::vec4(glm::vec2(5.0f, 5.0f + BUTTON_SIZE.y), EDITOR_SETTINGS_SIZE), "EditorSetttingsWindow"));

    if (_editorSettings == nullptr)
        return false;
    _editorSettings->setAlwaysOnTop(false);
    _editorSettings->setRollupEnabled(false);
    _editorSettings->setSizingEnabled(false);
    _editorSettings->setAlpha(0.8f);
    _editorSettings->moveToBack();
    _editorSettings->setText(language_("TOOLS"));
    _editorSettings->enable();
    _editorSettings->subscribeEvent(FrameWindow::EventCloseClicked, Event::Subscriber(&Editor::showEditorClicked, this));

    const glm::vec2 MENUBAR_SIZE(EDITOR_SETTINGS_SIZE.x , 24.0f);
    const glm::vec2 POPUP_SIZE(60.0f, 30.0f);

    /* Menu */
    {
        /* Menu bar*/
        _menuBar = static_cast<Menubar*>(_gui.createWidget("TaharezLook/Menubar",
                   glm::vec4(0.0f, 0.0f, MENUBAR_SIZE), "MenuBar", _editorSettings));
        /* Popup menus */
        _levelPopupItem = static_cast<MenuItem*>(_gui.createWidget("TaharezLook/MenuItem",
                          glm::vec4(0.0f, 0.0f, POPUP_SIZE), "LevelPopUpItem", _menuBar));
        _levelPopupMenu = static_cast<PopupMenu*>(_gui.createWidget("TaharezLook/PopupMenu",
                          glm::vec4(0.0f, 0.0f, POPUP_SIZE), "LevelPopUpMenu", _levelPopupItem));

        _addPopupItem = static_cast<MenuItem*>(_gui.createWidget("TaharezLook/MenuItem",
                        glm::vec4(0.0f, 0.0f, POPUP_SIZE), "AddPopUpItem", _menuBar));
        _addPopupMenu = static_cast<PopupMenu*>(_gui.createWidget("TaharezLook/PopupMenu",
                        glm::vec4(0.0f, 0.0f, POPUP_SIZE), "AddPopUpMenu", _addPopupItem));
        /* "Level" popup menu entries */
        _newLevelItem = static_cast<MenuItem*>(_gui.createWidget("TaharezLook/MenuItem",
                        glm::vec4(0.0f, 0.0f, POPUP_SIZE), "NewLevelItem", _levelPopupMenu));
        _saveLevelItem = static_cast<MenuItem*>(_gui.createWidget("TaharezLook/MenuItem",
                         glm::vec4(0.0f, 0.0f, POPUP_SIZE), "SaveLevelItem", _levelPopupMenu));
        _loadLevelItem = static_cast<MenuItem*>(_gui.createWidget("TaharezLook/MenuItem",
                         glm::vec4(0.0f, 0.0f, POPUP_SIZE), "LoadLevelItem", _levelPopupMenu));
        /* "Add" popup menu entries */
        _addWallItem = static_cast<MenuItem*>(_gui.createWidget("TaharezLook/MenuItem",
                       glm::vec4(0.0f, 0.0f, POPUP_SIZE), "AddWallItem", _addPopupMenu));
        _addFloorItem = static_cast<MenuItem*>(_gui.createWidget("TaharezLook/MenuItem",
                        glm::vec4(0.0f, 0.0f, POPUP_SIZE), "AddFloorItem", _addPopupMenu));
        _addTeleportItem = static_cast<MenuItem*>(_gui.createWidget("TaharezLook/MenuItem",
                           glm::vec4(0.0f, 0.0f, POPUP_SIZE), "AddTeleportItem", _addPopupMenu));
        _addQuestionItem = static_cast<MenuItem*>(_gui.createWidget("TaharezLook/MenuItem",
                           glm::vec4(0.0f, 0.0f, POPUP_SIZE), "AddQuestionItem", _addPopupMenu));
        _addPlayerItem = static_cast<MenuItem*>(_gui.createWidget("TaharezLook/MenuItem",
                         glm::vec4(0.0f, 0.0f, POPUP_SIZE), "AddPlayerItem", _addPopupMenu));

        if (_menuBar == nullptr || _addPopupItem == nullptr || _addPopupMenu == nullptr || _addWallItem == nullptr ||
            _addPlayerItem == nullptr || _addFloorItem == nullptr || _addTeleportItem == nullptr || _addQuestionItem == nullptr)
            return false;

        /* Level menu */
        _levelPopupItem->setText(language_("LEVEL"));
        _newLevelItem->setText(language_("NEW"));
        _saveLevelItem->setText(language_("SAVE"));
        _loadLevelItem->setText(language_("LOAD"));
        /* Add menu */
        _addPopupItem->setText(language_("PLACE_OBJECT"));
        _addWallItem->setText(language_("WALL"));
        _addPlayerItem->setText(language_("PLAYER"));
        _addFloorItem->setText(language_("FLOOR"));
        _addTeleportItem->setText(language_("PLACE_TELEPORT_PLATFORM"));
        _addQuestionItem->setText(language_("PLACE_QUESTION_PLATFORM"));
        
        _newLevelItem->subscribeEvent(MenuItem::EventMouseClick, Event::Subscriber(&Editor::openNewWindowClicked, this));
        _saveLevelItem->subscribeEvent(MenuItem::EventMouseClick, Event::Subscriber(&Editor::openSaveWindowClicked, this));
        _loadLevelItem->subscribeEvent(MenuItem::EventMouseClick, Event::Subscriber(&Editor::openLoadWindowClicked, this));
        _addWallItem->subscribeEvent(MenuItem::EventMouseClick, Event::Subscriber(&Editor::addWallClicked, this));
        _addPlayerItem->subscribeEvent(MenuItem::EventMouseClick, Event::Subscriber(&Editor::addPlayerClicked, this));
        _addFloorItem->subscribeEvent(MenuItem::EventMouseClick, Event::Subscriber(&Editor::addFloorClicked, this));
        _addTeleportItem->subscribeEvent(MenuItem::EventMouseClick, Event::Subscriber(&Editor::addTeleportClicked, this));
        _addQuestionItem->subscribeEvent(MenuItem::EventMouseClick, Event::Subscriber(&Editor::addQuestionClicked, this));
    }

    const glm::vec2 SLIDER_POS(5.0f, 5.0f);
    const glm::vec2 SLIDER_DIM(10.0f, 50.0f);
    const glm::vec2 SLIDER_DIST(10.0f + SLIDER_DIM.x, 0.0f);

    const glm::vec2 COLOR_GROUP_SIZE(glm::vec2(SLIDER_DIST.x * 5.0f + COLOR_PICKER_SIZE.x, 80.0f));

    /* RGBA spliders */
    {
        _colorGroup = static_cast<GroupBox*>(_gui.createWidget("TaharezLook/GroupBox",
                      glm::vec4(8.0f, 3.0f + MENUBAR_SIZE.y,
                      COLOR_GROUP_SIZE + glm::vec2(5.0f)),
                      "ColorGroupBox", _editorSettings));

        _rColor = static_cast<Slider*>(_gui.createWidget("TaharezLook/Slider", 
                  glm::vec4(SLIDER_POS, SLIDER_DIM), "RedSlider", _colorGroup));
        _gColor = static_cast<Slider*>(_gui.createWidget("TaharezLook/Slider", 
                  glm::vec4(SLIDER_POS + SLIDER_DIST, SLIDER_DIM), "GreenSlider", _colorGroup));
        _bColor = static_cast<Slider*>(_gui.createWidget("TaharezLook/Slider", 
                  glm::vec4(SLIDER_POS + SLIDER_DIST * 2.0f, SLIDER_DIM), "BlueSlider", _colorGroup));
        _aColor = static_cast<Slider*>(_gui.createWidget("TaharezLook/Slider", 
                  glm::vec4(SLIDER_POS + SLIDER_DIST * 3.0f, SLIDER_DIM), "AlphaSlider", _colorGroup));
        if (_colorGroup == nullptr || _rColor == nullptr || _gColor == nullptr || _bColor == nullptr || _aColor == nullptr)
            return false;

        _colorGroup->setText("RGBA");

        _rColor->setMaxValue(MAX_COLOR_VALUE);
        _gColor->setMaxValue(MAX_COLOR_VALUE);
        _bColor->setMaxValue(MAX_COLOR_VALUE);
        _aColor->setMaxValue(MAX_COLOR_VALUE);

        _rColor->setClickStep(1.0f);
        _gColor->setClickStep(1.0f);
        _bColor->setClickStep(1.0f);
        _aColor->setClickStep(1.0f);

        const GLubyte MAX_VALUE = (GLubyte)MAX_COLOR_VALUE;
        setRGBASliders(Engine::Color(MAX_VALUE, MAX_VALUE, MAX_VALUE, MAX_VALUE));

        /* Load texture for colorPicker */
        _colorPicker = Engine::ResourceManager::getTexture("Textures/blank.png");
    }

    const glm::vec2 DEBUG_TB_SIZE(125.0f, 20.0f);
    const glm::vec2 OPTIONS_GROUP_POS(COLOR_GROUP_SIZE.x + 20.0f, 3.0f + MENUBAR_SIZE.y);
    const glm::vec2 OPTIONS_GROUP_SIZE(130.0f, 210.0f);
    const glm::vec2 LABEL_SIZE(60.0f, 30.0f);
    const glm::vec2 TEXTURE_COMBOBOX_SIZE(OPTIONS_GROUP_SIZE.x - 15.0f, BUTTON_SIZE.y * 4.0f + 10.0f);
    const glm::vec2 SPINNER_SIZE(60.0f, 25.0f);
    const glm::vec2 SPINNER_POS(5.0f, 5.0f);
    
    /* Options */
    {
        _optionsGroup = static_cast<GroupBox*>(_gui.createWidget("TaharezLook/GroupBox",
                      glm::vec4(OPTIONS_GROUP_POS, OPTIONS_GROUP_SIZE),
                      "OptionsGroupBox", _editorSettings));
        if (_optionsGroup == nullptr)
            return false;

        _debugMode = static_cast<ToggleButton*>(_gui.createWidget("TaharezLook/Checkbox",
                     glm::vec4(glm::vec2(5.0f, 5.0f), DEBUG_TB_SIZE), "DebugModeToggleButton", _optionsGroup));
        _textureLabel = static_cast<Window*>(_gui.createWidget("TaharezLook/StaticText", 
                        glm::vec4(glm::vec2(DEBUG_TB_SIZE.x / 2.0f - LABEL_SIZE.x / 2.0f, 10.0f + DEBUG_TB_SIZE.y), LABEL_SIZE),
                        "TextureLabel", _optionsGroup));
        _texture_Combobox = static_cast<Combobox*>(_gui.createWidget("TaharezLook/Combobox",
                            glm::vec4(glm::vec2(0.0f, 10.0f + DEBUG_TB_SIZE.y + LABEL_SIZE.y), TEXTURE_COMBOBOX_SIZE),
                            "TextureCombobox", _optionsGroup));
        const glm::vec2 FIXED_UV_POS(5.0f, 45.0f + DEBUG_TB_SIZE.y + LABEL_SIZE.y);
        _fixedUV = static_cast<ToggleButton*>(_gui.createWidget("TaharezLook/Checkbox",
                   glm::vec4(FIXED_UV_POS, DEBUG_TB_SIZE), "FixedUVToggleButton", _optionsGroup));
        const glm::vec2 UV_SCALE_SPINNER_SIZE = SPINNER_SIZE - glm::vec2(5.0f, 0.0f);
        const glm::vec2 UV_SCALE_LABEL_SIZE = LABEL_SIZE + glm::vec2(20.0f, 0.0f);
        const glm::vec2 UV_SCALE_LABEL_POS(20.0f, 45.0f + DEBUG_TB_SIZE.y * 2.0f + LABEL_SIZE.y + 5.0f);
        const glm::vec2 UV_SCALE_SPINNER_POS(20.0f, UV_SCALE_LABEL_POS.y + LABEL_SIZE.y);
        _uvScaleSpinner = static_cast<Spinner*>(_gui.createWidget("TaharezLook/Spinner", 
                        glm::vec4(UV_SCALE_SPINNER_POS, UV_SCALE_SPINNER_SIZE), "UVScaleSpinner", _optionsGroup));
        _uvScaleLabel = static_cast<Window*>(_gui.createWidget("TaharezLook/StaticText", 
                        glm::vec4(UV_SCALE_LABEL_POS, UV_SCALE_LABEL_SIZE), "UVScaleLabel", _optionsGroup));
        if (_debugMode == nullptr || _textureLabel == nullptr || _texture_Combobox == nullptr || _fixedUV == nullptr ||
            _uvScaleLabel == nullptr || _uvScaleSpinner == nullptr)
            return false;

        std::vector<Engine::File> ls;
        Engine::IOManager::listFiles("Textures", ls);
        _texture_ListBoxItems.clear();

        /* Add all textures to ListBoxItems */
        for (auto& file : ls) {
            /* Do not add directories to list */
            if (file.directoryFlag == true)
                continue;

            /* Remove "Textures/" substring and image extension */
            file.path.erase(0, std::string("Textures/").size());
            _texture_ListBoxItems.push_back(new CEGUI::ListboxTextItem(file.path));
            _texture_Combobox->addItem(_texture_ListBoxItems.back());
        }

        _texture_Combobox->clearAllSelections();
        _texture_Combobox->getEditbox()->setReadOnly(true);
        
        _uvScaleSpinner->setTextInputMode(CEGUI::Spinner::FloatingPoint);
        const float MAX_UV_SCALE = 100.0f;
        _uvScaleSpinner->setMaximumValue(MAX_UV_SCALE);
        _uvScaleSpinner->setMinimumValue(1.0f / MAX_UV_SCALE);
        _uvScaleSpinner->setStepSize(MAX_UV_SCALE / 1000.0f);
        _uvScaleSpinner->setCurrentValue(1.0f);

        _textureLabel->setProperty("FrameEnabled", "false");
        _textureLabel->setProperty("BackgroundEnabled", "false");
        _uvScaleLabel->setProperty("FrameEnabled", "false");
        _uvScaleLabel->setProperty("BackgroundEnabled", "false");

        _optionsGroup->setText(language_("OPTIONS"));
        _debugMode->setText(language_("DEBUG"));
        _fixedUV->setText(language_("FIXED_UV"));
        _textureLabel->setText(language_("TEXTURE"));
        _uvScaleLabel->setText(language_("UV_SCALE"));
    }

    const glm::vec2 RECT_GROUP_SIZE(150.0f, SPINNER_SIZE.y * 5.0f);

    /* Rectangle size settings */
    {
        _rectSizeGroup = static_cast<GroupBox*>(_gui.createWidget("TaharezLook/GroupBox",
                         glm::vec4(glm::vec2(0.0f, 80.0f + MENUBAR_SIZE.y) + glm::vec2(5.0f, 10.0f), RECT_GROUP_SIZE),
                         "RectangleSizeGroup", _editorSettings));
        if (_rectSizeGroup == nullptr)
            return false;

        /* Spinners */
        _widthSpinner = static_cast<Spinner*>(_gui.createWidget("TaharezLook/Spinner", 
                        glm::vec4(SPINNER_POS, SPINNER_SIZE), "WidthSpinner", _rectSizeGroup));
        _heightSpinner = static_cast<Spinner*>(_gui.createWidget("TaharezLook/Spinner", 
                         glm::vec4(SPINNER_POS + glm::vec2(0.0f, SPINNER_SIZE.y + 5.0f), SPINNER_SIZE),
                         "HeightSpinner", _rectSizeGroup));
        _rotationSpinner = static_cast<Spinner*>(_gui.createWidget("TaharezLook/Spinner", 
                           glm::vec4(SPINNER_POS + glm::vec2(0.0f, 2.0f * (SPINNER_SIZE.y + 5.0f)), SPINNER_SIZE),
                           "RotationSpinner", _rectSizeGroup));

        /* Labels */
        _widthLabel = static_cast<Window*>(_gui.createWidget("TaharezLook/StaticText", 
                      glm::vec4(SPINNER_POS + glm::vec2(SPINNER_SIZE.x + 5.0f, 0.0f), LABEL_SIZE),
                      "WidthLabel", _rectSizeGroup));
        _heightLabel = static_cast<Window*>(_gui.createWidget("TaharezLook/StaticText", 
                       glm::vec4(SPINNER_POS + glm::vec2(SPINNER_SIZE.x + 5.0f, SPINNER_SIZE.y + 5.0f), LABEL_SIZE),
                       "HeightLabel", _rectSizeGroup));
        _rotationLabel = static_cast<Window*>(_gui.createWidget("TaharezLook/StaticText", 
                         glm::vec4(SPINNER_POS + glm::vec2(SPINNER_SIZE.x + 5.0f, (SPINNER_SIZE.y + 5.0f) * 2.0f), LABEL_SIZE),
                         "RotationLabel", _rectSizeGroup));

        if (_widthSpinner == nullptr || _heightSpinner == nullptr || _rotationSpinner == nullptr ||
            _widthLabel == nullptr || _heightLabel == nullptr || _rotationLabel == nullptr)
            return false;

        _rectSizeGroup->setText(language_("SIZE"));
        _widthLabel->setText(language_("WIDTH"));
        _heightLabel->setText(language_("HEIGHT"));
        _rotationLabel->setText(language_("ROTATION"));

        _widthLabel->setProperty("FrameEnabled", "false");
        _widthLabel->setProperty("BackgroundEnabled", "false");
        _heightLabel->setProperty("FrameEnabled", "false");
        _heightLabel->setProperty("BackgroundEnabled", "false");
        _rotationLabel->setProperty("FrameEnabled", "false");
        _rotationLabel->setProperty("BackgroundEnabled", "false");

        _widthSpinner->setTextInputMode(CEGUI::Spinner::FloatingPoint);
        _heightSpinner->setTextInputMode(CEGUI::Spinner::FloatingPoint);
        _rotationSpinner->setTextInputMode(CEGUI::Spinner::FloatingPoint);

        const float MAX_ROTATION = 560.0f;
        _widthSpinner->setMaximumValue(10000.0f);
        _heightSpinner->setMaximumValue(10000.0f);
        _rotationSpinner->setMaximumValue(MAX_ROTATION);

        _widthSpinner->setMinimumValue(0.01f);
        _heightSpinner->setMinimumValue(0.01f);
        _rotationSpinner->setMinimumValue(-MAX_ROTATION);

        _widthSpinner->setStepSize(10.0f);
        _heightSpinner->setStepSize(10.0f);
        _rotationSpinner->setStepSize(1.0f);

        setSizeSpinners(SIZE_SPINNER_DEFAULT, SIZE_SPINNER_DEFAULT, ROTATION_SPINNER_DEFAULT);
    }

    const glm::vec2 POSITION_GROUP_SIZE(105.0f, SPINNER_SIZE.y * 4.0f - 5.0f);

    /* Position settings */
    {
        _positionGroup = static_cast<GroupBox*>(_gui.createWidget("TaharezLook/GroupBox",
                         glm::vec4(glm::vec2(5.0f, 90.0f + MENUBAR_SIZE.y + RECT_GROUP_SIZE.y), POSITION_GROUP_SIZE),
                         "PositionGroup", _editorSettings));
        if (_positionGroup == nullptr)
            return false;

        /* Spinners */
        _xPosSpinner = static_cast<Spinner*>(_gui.createWidget("TaharezLook/Spinner", 
                       glm::vec4(SPINNER_POS, SPINNER_SIZE), "xPosSpinner", _positionGroup));
        _yPosSpinner = static_cast<Spinner*>(_gui.createWidget("TaharezLook/Spinner", 
                       glm::vec4(SPINNER_POS + glm::vec2(0.0f, SPINNER_SIZE.y + 5.0f), SPINNER_SIZE),
                       "yPosSpinner", _positionGroup));

        /* Labels */
        _xPosLabel = static_cast<Window*>(_gui.createWidget("TaharezLook/StaticText", 
                     glm::vec4(SPINNER_POS + glm::vec2(SPINNER_SIZE.x + 5.0f, 0.0f), LABEL_SIZE),
                     "xPosLabel", _positionGroup));
        _yPosLabel = static_cast<Window*>(_gui.createWidget("TaharezLook/StaticText", 
                     glm::vec4(SPINNER_POS + glm::vec2(SPINNER_SIZE.x + 5.0f, SPINNER_SIZE.y + 5.0f), LABEL_SIZE),
                     "yPosLabel", _positionGroup));

        if (_xPosLabel == nullptr || _xPosSpinner == nullptr || _yPosLabel == nullptr || _yPosSpinner == nullptr)
            return false;

        _positionGroup->setText(language_("POSITION"));
        _xPosLabel->setText(language_("POS_X"));
        _yPosLabel->setText(language_("POS_Y"));

        _xPosLabel->setProperty("FrameEnabled", "false");
        _xPosLabel->setProperty("BackgroundEnabled", "false");
        _yPosLabel->setProperty("FrameEnabled", "false");
        _yPosLabel->setProperty("BackgroundEnabled", "false");

        _xPosSpinner->setTextInputMode(CEGUI::Spinner::FloatingPoint);
        _yPosSpinner->setTextInputMode(CEGUI::Spinner::FloatingPoint);

        const float MAX_POS = 1000000.0f;
        _xPosSpinner->setMaximumValue(MAX_POS);
        _yPosSpinner->setMaximumValue(MAX_POS);

        _xPosSpinner->setMinimumValue(-MAX_POS);
        _yPosSpinner->setMinimumValue(-MAX_POS);

        _xPosSpinner->setStepSize(1.0f);
        _yPosSpinner->setStepSize(1.0f);

        setPositionSpinners(0.0f, 0.0f);
    }

    const glm::vec2 TELEPORT_GROUP_SIZE(EDITOR_SETTINGS_SIZE.x - POSITION_GROUP_SIZE.x - 25.0f, SPINNER_SIZE.y * 4.0f - 5.0f);

    /* Teleport settings */
    {
        _teleportGroup = static_cast<GroupBox*>(_gui.createWidget("TaharezLook/GroupBox",
                         glm::vec4(glm::vec2(10.0f + POSITION_GROUP_SIZE.x, 90.0f + MENUBAR_SIZE.y + RECT_GROUP_SIZE.y), 
                         TELEPORT_GROUP_SIZE), "TeleportGroup", _editorSettings));
        if (_teleportGroup == nullptr)
            return false;

        /* Spinners */
        _xSpawnSpinner = static_cast<Spinner*>(_gui.createWidget("TaharezLook/Spinner", 
                         glm::vec4(SPINNER_POS, SPINNER_SIZE), "xSpawnSpinner", _teleportGroup));
        _ySpawnSpinner = static_cast<Spinner*>(_gui.createWidget("TaharezLook/Spinner", 
                         glm::vec4(SPINNER_POS + glm::vec2(0.0f, SPINNER_SIZE.y + 5.0f), SPINNER_SIZE),
                         "ySpawnSpinner", _teleportGroup));

        /* Labels */
        _xSpawnLabel = static_cast<Window*>(_gui.createWidget("TaharezLook/StaticText", 
                       glm::vec4(SPINNER_POS + glm::vec2(SPINNER_SIZE.x + 5.0f, 0.0f), LABEL_SIZE),
                       "xSpawnLabel", _teleportGroup));
        _ySpawnLabel = static_cast<Window*>(_gui.createWidget("TaharezLook/StaticText", 
                       glm::vec4(SPINNER_POS + glm::vec2(SPINNER_SIZE.x + 5.0f, SPINNER_SIZE.y + 5.0f), LABEL_SIZE),
                       "ySpawnLabel", _teleportGroup));

        /* PushButton */
        _placeSpawn = static_cast<PushButton*>(_gui.createWidget("TaharezLook/Button",
                      glm::vec4(glm::vec2(90.0f, 15.0f), glm::vec2(70.0f, 40.0f)), "PlaceSpawnButton", _teleportGroup));

        if (_xSpawnLabel == nullptr || _xSpawnSpinner == nullptr || _ySpawnLabel == nullptr || _ySpawnSpinner == nullptr ||
            _placeSpawn == nullptr)
            return false;

        _teleportGroup->setText(language_("TELEPORT"));
        _xSpawnLabel->setText(language_("POS_X"));
        _ySpawnLabel->setText(language_("POS_Y"));
        _placeSpawn->setText(language_("PLACE_SPAWN"));

        _xSpawnLabel->setProperty("FrameEnabled", "false");
        _xSpawnLabel->setProperty("BackgroundEnabled", "false");
        _ySpawnLabel->setProperty("FrameEnabled", "false");
        _ySpawnLabel->setProperty("BackgroundEnabled", "false");

        _xSpawnSpinner->setTextInputMode(CEGUI::Spinner::FloatingPoint);
        _ySpawnSpinner->setTextInputMode(CEGUI::Spinner::FloatingPoint);

        const float MAX_POS = 1000000.0f;
        _xSpawnSpinner->setMaximumValue(MAX_POS);
        _ySpawnSpinner->setMaximumValue(MAX_POS);

        _xSpawnSpinner->setMinimumValue(-MAX_POS);
        _ySpawnSpinner->setMinimumValue(-MAX_POS);

        _xSpawnSpinner->setStepSize(1.0f);
        _ySpawnSpinner->setStepSize(1.0f);

        _placeSpawn->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&Editor::placeTeleportSpawnClicked, this));
        setSpawnSpinners(0.0f, 0.0f);
        _teleportGroup->disable();
    }

    const glm::vec2 QUESTION_GROUP_SIZE(EDITOR_SETTINGS_SIZE.x - 20.0f, 155.0f);
    const glm::vec2 QUESTION_GROUP_POS(5.0, 90.0f + MENUBAR_SIZE.y + RECT_GROUP_SIZE.y + TELEPORT_GROUP_SIZE.y);

    /* Question Group */
    {
        _questionGroup = static_cast<GroupBox*>(_gui.createWidget("TaharezLook/GroupBox", glm::vec4(QUESTION_GROUP_POS,
                         QUESTION_GROUP_SIZE), "QuestionGroup", _editorSettings));
        if (_questionGroup == nullptr)
            return false;

        _question_Combobox = static_cast<Combobox*>(_gui.createWidget("TaharezLook/Combobox",
                      glm::vec4(glm::vec2(5.0f, 5.0f), QUESTION_GROUP_SIZE - glm::vec2(25.0f, 40.0f)), 
                      "QuestionEditbox", _questionGroup));
        if (_question_Combobox == nullptr)
            return false;

        std::vector<Engine::File> ls;
        Engine::IOManager::listFiles("Questions", ls);
        _question_ListBoxItems.clear();

        /* Add all questions to ListBoxItems */
        for (auto& file : ls) {
            /* Do not add directories to list */
            if (file.directoryFlag == true)
                continue;

            /* Remove "Questions/" substring */
            file.path.erase(0, std::string("Questions/").size());
            _question_ListBoxItems.push_back(new CEGUI::ListboxTextItem(file.path));
            _question_Combobox->addItem(_question_ListBoxItems.back());
        }

        _question_Combobox->clearAllSelections();
        _question_Combobox->getEditbox()->setReadOnly(true);

        _questionGroup->setText(language_("QUESTION"));
        _questionGroup->disable();
    }

    /* Main buttons on top */
    {
        _toMainMenu = static_cast<PushButton*>(_gui.createWidget("TaharezLook/Button",
                      glm::vec4(BUTTON_POSITION, BUTTON_SIZE), "ToMainMenuButton"));
        _showEditorButton = static_cast<PushButton*>(_gui.createWidget("TaharezLook/Button",
                            glm::vec4(BUTTON_POSITION + BUTTON_DISTANCE_X, BUTTON_SIZE), "ShowEditorButton"));
        _saveButton = static_cast<PushButton*>(_gui.createWidget("TaharezLook/Button", 
                      glm::vec4(BUTTON_POSITION + BUTTON_DISTANCE_X * 2.0f, BUTTON_SIZE), "SaveButton"));
        _loadButton = static_cast<PushButton*>(_gui.createWidget("TaharezLook/Button",
                      glm::vec4(BUTTON_POSITION + BUTTON_DISTANCE_X * 3.0f, BUTTON_SIZE), "LoadButton"));

        if (_toMainMenu == nullptr || _showEditorButton == nullptr || _saveButton == nullptr || _loadButton == nullptr)
            return false;

        _toMainMenu->setAlwaysOnTop(true);
        _showEditorButton->setAlwaysOnTop(true);
        _saveButton->setAlwaysOnTop(true);
        _loadButton->setAlwaysOnTop(true);

        _toMainMenu->setText(language_("MAIN_MENU"));
        _showEditorButton->setText(language_("SHOW_EDITOR"));
        _saveButton->setText(language_("SAVE_LEVEL"));
        _loadButton->setText(language_("LOAD_LEVEL"));

        _toMainMenu->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&Editor::toMainMenuClicked, this));
        _showEditorButton->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&Editor::showEditorClicked, this));
        _saveButton->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&Editor::openSaveWindowClicked, this));
        _loadButton->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&Editor::openLoadWindowClicked, this));
    }
    
    /* Constants are used for load window also */
    const glm::vec2 SAVE_WINDOW_SIZE(220.0f, 200.f);
    const glm::vec2 SAVE_WINDOW_POSITION = glm::vec2(window_.width(2.0f), window_.height(2.0f)) - SAVE_WINDOW_SIZE / 2.0f;
    const glm::vec2 SAVE_WINDOW_BUTTON_POS = SAVE_WINDOW_SIZE / 2.0f - BUTTON_SIZE / 2.0f - glm::vec2(0.0f, 80.0f);
    const glm::vec2 SAVE_WINDOW_COMBOBOX_POS = SAVE_WINDOW_SIZE / 2.0f - BUTTON_SIZE / 2.0f - glm::vec2(0.0f, 45.0f);
    const glm::vec2 SAVE_WINDOW_COMBOBOX_SIZE(BUTTON_SIZE.x, 120.0f);

    /* Save window */
    {
        _saveWindow = static_cast<FrameWindow*>(_gui.createWidget("TaharezLook/FrameWindow",
                      glm::vec4(SAVE_WINDOW_POSITION, SAVE_WINDOW_SIZE), "SaveWindow"));
        if (_saveWindow == nullptr)
            return false;
        _saveWindow->setDragMovingEnabled(false);
        _saveWindow->setSizingEnabled(false);
        _saveWindow->setRollupEnabled(false);
        _saveWindow->setAlwaysOnTop(true);
        _saveWindow->subscribeEvent(FrameWindow::EventCloseClicked, Event::Subscriber(&Editor::closeWindowClicked, this));

        _saveWindow_SaveButton = static_cast<PushButton*>(_gui.createWidget("TaharezLook/Button",
                                 glm::vec4(SAVE_WINDOW_BUTTON_POS, BUTTON_SIZE), "SaveWindowButton", _saveWindow));
        _saveWindow_Combobox = static_cast<Combobox*>(_gui.createWidget("TaharezLook/Combobox",
                               glm::vec4(SAVE_WINDOW_COMBOBOX_POS, SAVE_WINDOW_COMBOBOX_SIZE),
                               "SaveWindowComboBox", _saveWindow));
        if (_saveWindow_SaveButton == nullptr || _saveWindow_Combobox == nullptr)
            return false;

        _saveWindow->setText(language_("SAVE_LEVEL"));
        _saveWindow_SaveButton->setText(language_("SELECT"));

        _saveWindow_SaveButton->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&Editor::saveButtonClicked, this));
        _HIDE_SAVE_WINDOW
    }

    /* Load window */
    {
        _loadWindow = static_cast<FrameWindow*>(_gui.createWidget("TaharezLook/FrameWindow",
                      glm::vec4(SAVE_WINDOW_POSITION, SAVE_WINDOW_SIZE), "LoadWindow"));
        if (_loadWindow == nullptr)
            return false;
        _loadWindow->setDragMovingEnabled(false);
        _loadWindow->setSizingEnabled(false);
        _loadWindow->setRollupEnabled(false);
        _loadWindow->setAlwaysOnTop(true);
        _loadWindow->subscribeEvent(FrameWindow::EventCloseClicked, Event::Subscriber(&Editor::closeWindowClicked, this));

        _loadWindow_LoadButton = static_cast<PushButton*>(_gui.createWidget("TaharezLook/Button",
                                 glm::vec4(SAVE_WINDOW_BUTTON_POS, BUTTON_SIZE), "LoadWindowButton", _loadWindow));
        _loadWindow_Combobox = static_cast<Combobox*>(_gui.createWidget("TaharezLook/Combobox",
                               glm::vec4(SAVE_WINDOW_COMBOBOX_POS, SAVE_WINDOW_COMBOBOX_SIZE),
                               "LoadWindowComboBox", _loadWindow));
        if (_loadWindow_LoadButton == nullptr || _loadWindow_Combobox == nullptr)
            return false;

        _loadWindow->setText(language_("LOAD_LEVEL"));
        _loadWindow_LoadButton->setText(language_("SELECT"));

        _loadWindow_LoadButton->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&Editor::loadButtonClicked, this));
        _HIDE_LOAD_WINDOW
    }

    /* New level window */
    {
        _newLevelWindow = static_cast<FrameWindow*>(_gui.createWidget("TaharezLook/FrameWindow",
                      glm::vec4(SAVE_WINDOW_POSITION, SAVE_WINDOW_SIZE), "NewLevelWindow"));
        if (_newLevelWindow == nullptr)
            return false;
        _newLevelWindow->setDragMovingEnabled(false);
        _newLevelWindow->setSizingEnabled(false);
        _newLevelWindow->setRollupEnabled(false);
        _newLevelWindow->setAlwaysOnTop(true);
        _newLevelWindow->subscribeEvent(FrameWindow::EventCloseClicked, Event::Subscriber(&Editor::closeWindowClicked, this));

        const glm::vec2 OFFSET(18.0f, 18.0f);
        const glm::vec2 LABEL_SIZE(SAVE_WINDOW_SIZE.x - 38.0f, 100.0f);
        _newLevelWindow_Label = static_cast<Window*>(_gui.createWidget("TaharezLook/StaticText", 
                         glm::vec4(OFFSET, LABEL_SIZE),
                         "NewLevelWindowLabel", _newLevelWindow));
        _newLevelConfirmButton = static_cast<PushButton*>(_gui.createWidget("TaharezLook/Button",
                                 glm::vec4(SAVE_WINDOW_BUTTON_POS + glm::vec2(0.0f, 124.0f), BUTTON_SIZE),
                                 "NewLevelConfimButton", _newLevelWindow));
        if (_newLevelConfirmButton == nullptr || _newLevelWindow_Label == nullptr)
            return false;

        _newLevelWindow_Label->setProperty("VertFormatting", "CentreAligned");
        _newLevelWindow_Label->setProperty("HorzFormatting", "WordWrapCentreAligned");

        _newLevelWindow->setText(language_("NEW") + ' ' + language_("LEVEL"));
        _newLevelConfirmButton->setText(language_("CONFIRM"));
        _newLevelWindow_Label->setText(language_("CLEAR_LEVEL_WARNING"));

        _newLevelConfirmButton->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&Editor::newLevelButtonClicked, this));
        _HIDE_NEW_LEVEL_WINDOW
    }

    return true;
}

void Editor::handleEvents()
{
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        _game->handleEvents(e);
        _gui.handleEvents(e);
    }

    if (eventHandler_.keyPressed(SDLK_ESCAPE) || eventHandler_.keyPressed(SDL_BUTTON_RIGHT)) {
        _SET_SELECT_MODE
        refreshWidgets();
    }
    else if (eventHandler_.keyPressed(SDLK_l))
        addWallClicked(CEGUI::EventArgs());
    else if (eventHandler_.keyPressed(SDLK_o))
        addFloorClicked(CEGUI::EventArgs());
    else if (eventHandler_.keyPressed(SDLK_p))
        addPlayerClicked(CEGUI::EventArgs());
    else if (eventHandler_.keyPressed(SDLK_j))
        addQuestionClicked(CEGUI::EventArgs());
    else if (eventHandler_.keyPressed(SDLK_u))
        addTeleportClicked(CEGUI::EventArgs());
    else if (eventHandler_.keyPressed(SDLK_DELETE))
        removeSelectedObject();
    else if (eventHandler_.keyPressed(SDLK_F5))
        openSaveWindowClicked(CEGUI::EventArgs());
    else if (eventHandler_.keyPressed(SDLK_F9))
        openLoadWindowClicked(CEGUI::EventArgs());
    else if (eventHandler_.keyPressed(SDLK_F3))
        _debugMode->setSelected(!_debugMode->isSelected());
    else if (eventHandler_.keyPressed(SDLK_RETURN)) {
        if (_saveWindow->isVisible())
            saveButtonClicked(CEGUI::EventArgs());
        else if (_loadWindow->isVisible())
            loadButtonClicked(CEGUI::EventArgs());
    }

    /* Control shortcuts for spinners */
    float SPEED = 0.1f; /* Hold shift to increase speed */
    if (eventHandler_.keyDown(SDLK_LSHIFT))
        SPEED = 1.0f;

    /* Control size with A S D W */
    if (eventHandler_.keyDown(SDLK_a))
        _widthSpinner->setCurrentValue(_widthSpinner->getCurrentValue() - _widthSpinner->getStepSize() * SPEED);
    if (eventHandler_.keyDown(SDLK_d))
        _widthSpinner->setCurrentValue(_widthSpinner->getCurrentValue() + _widthSpinner->getStepSize() * SPEED);
    if (eventHandler_.keyDown(SDLK_s))
        _heightSpinner->setCurrentValue(_heightSpinner->getCurrentValue() - _heightSpinner->getStepSize() * SPEED);
    if (eventHandler_.keyDown(SDLK_w))
        _heightSpinner->setCurrentValue(_heightSpinner->getCurrentValue() + _heightSpinner->getStepSize() * SPEED);

    /* Control rotation with Q E*/
    if (eventHandler_.keyDown(SDLK_q))
        setRotationSlider((float)_rotationSpinner->getCurrentValue() - (float)_rotationSpinner->getStepSize() * (0.1f / SPEED));
    if (eventHandler_.keyDown(SDLK_e))
        setRotationSlider((float)_rotationSpinner->getCurrentValue() + (float)_rotationSpinner->getStepSize() * (0.1f / SPEED));
    /* Rotate for 45deg or hold shift for 4.5deg */
    if (eventHandler_.keyPressed(SDLK_z))
        setRotationSlider((float)_rotationSpinner->getCurrentValue() - 45.0f * (0.1f / SPEED));
    if (eventHandler_.keyPressed(SDLK_c))
        setRotationSlider((float)_rotationSpinner->getCurrentValue() + 45.0f * (0.1f / SPEED));

    /* Control position with arrow keys */
    if (eventHandler_.keyDown(SDLK_LEFT))
        _xPosSpinner->setCurrentValue(_xPosSpinner->getCurrentValue() - _xPosSpinner->getStepSize() * SPEED);
    if (eventHandler_.keyDown(SDLK_RIGHT))
        _xPosSpinner->setCurrentValue(_xPosSpinner->getCurrentValue() + _xPosSpinner->getStepSize() * SPEED);
    if (eventHandler_.keyDown(SDLK_DOWN))
        _yPosSpinner->setCurrentValue(_yPosSpinner->getCurrentValue() - _yPosSpinner->getStepSize() * SPEED);
    if (eventHandler_.keyDown(SDLK_UP))
        _yPosSpinner->setCurrentValue(_yPosSpinner->getCurrentValue() + _yPosSpinner->getStepSize() * SPEED);

    /* Apply spinner changes to selected object */
    if (_objectID != NO_ID && (_mouseMode == MouseMode::EDIT || _mouseMode == MouseMode::DRAG))
        refreshSelectedObject();
}

void Editor::removeSelectedObject()
{
    switch (_object) {
    case Object::NONE:
        return;
    case Object::WALL:
        walls_[_objectID].dispose(world_.get());
        walls_.erase(walls_.begin() + _objectID);
        break;
    case Object::FLOOR:
        floors_[_objectID].dispose(world_.get());
        floors_.erase(floors_.begin() + _objectID);
        break;
    case Object::QUESTION_PLATFORM:
        questions_[_objectID].dispose(world_.get());
        questions_.erase(questions_.begin() + _objectID);
        break;
    case Object::TELEPORT_PLATFORM:
        teleports_[_objectID].dispose(world_.get());
        teleports_.erase(teleports_.begin() + _objectID);
        break;
    case Object::PLAYER:
        player_.dispose(world_.get());
        break;
    }
    _SET_SELECT_MODE
    setSizeSpinners(SIZE_SPINNER_DEFAULT, SIZE_SPINNER_DEFAULT, ROTATION_SPINNER_DEFAULT);
    setPositionSpinners(0.0f, 0.0f);
    _fixedUV->setSelected(false);
}

void Editor::updateMouseActivity()
{
    /* Mouse scrolling */
    const float CAMERA_ZOOM_SCALE = 0.2f;
    if (eventHandler_.mouseWheel() != 0)
        _camera.offsetScale(eventHandler_.mouseWheel() * CAMERA_ZOOM_SCALE * _camera.scale());

    /* Moving camera while holding right mouse button */
    if (eventHandler_.keyDown(SDL_BUTTON_MIDDLE)) {
        const float SPEED = 0.7f;
        glm::ivec2 relPos;
        eventHandler_.getRelativeMousePosition(&relPos.x, &relPos.y);
        _camera.offsetPosition(glm::vec2(-relPos.x, relPos.y * _camera.aspectRatio()) * SPEED / _camera.scale());
    }

    if (isMouseInGUI())
        return;

    /* Placing objects */
    static glm::vec2 DRAG_OFFSET;
    const glm::vec2 mousePos = _camera.convertScreenToWorldCoords(eventHandler_.mouseCoords());
    if (_mouseMode == MouseMode::PLACE && eventHandler_.keyPressed(SDL_BUTTON_LEFT)) {
        switch (_object) {
        case Object::FLOOR:
            placeFloor(mousePos);
            break;
        case Object::WALL:
            placeWall(mousePos);
            break;
        case Object::PLAYER:
            placePlayer(mousePos);
            break;
        case Object::QUESTION_PLATFORM:
            placeQuestion(mousePos);
            break;
        case Object::TELEPORT_PLATFORM:
            if (_isPlacingSpawn) {
                setSpawnSpinners(mousePos.x, mousePos.y);
                teleports_[_objectID].setSpawnPoint(mousePos);
                _mouseMode = MouseMode::EDIT;
                refreshWidgets();
                return;
            }
            else
                placeTeleport(mousePos);
            break;
        }
        _SET_SELECT_MODE
    }

    /* Selecting objects */
    else if ((_mouseMode == MouseMode::DRAG || _mouseMode == MouseMode::EDIT) && eventHandler_.keyReleased(SDL_BUTTON_LEFT)) {
        if (_isPlacingSpawn) {
            _isPlacingSpawn = false;
            return;
        }


        /* Reset for the moment */
        _SET_SELECT_MODE

        /* If player is clicked */
        if (player_.testPoint(mousePos.x, mousePos.y)) {
            _mouseMode = MouseMode::EDIT;
            _object = Object::PLAYER;
            _objectID = PLAYER_ID;
        }
        /* Questions */
        for (int i = (int)questions_.size() - 1; i >= 0 && _mouseMode != MouseMode::EDIT; i--) {
            if (questions_[i].testPoint(mousePos.x, mousePos.y)) {
                _mouseMode = MouseMode::EDIT;
                _object = Object::QUESTION_PLATFORM;
                _objectID = i;
                break;
            }
        }
        /* Teleports */
        for (int i = (int)teleports_.size() - 1; i >= 0 && _mouseMode != MouseMode::EDIT; i--) {
            if (teleports_[i].testPoint(mousePos.x, mousePos.y)) {
                _mouseMode = MouseMode::EDIT;
                _object = Object::TELEPORT_PLATFORM;
                _objectID = i;
                break;
            }
        }
        /* Next, test walls */
        for (int i = (int)walls_.size() - 1; i >= 0 && _mouseMode != MouseMode::EDIT; i--) {
            if (walls_[i].testPoint(mousePos.x, mousePos.y)) {
                _mouseMode = MouseMode::EDIT;
                _object = Object::WALL;

                /* Bring selected object on top */
                _objectID = walls_.size() - 1;
                WallBlock tmp = walls_[i];
                walls_[i] = walls_[_objectID];
                walls_[_objectID] = tmp;
                break;
            }
        }
        /* Floors test */
        for (int i = (int)floors_.size() - 1; i >= 0 && _mouseMode != MouseMode::EDIT; i--) {
            if (floors_[i].testPoint(mousePos.x, mousePos.y)) {
                _mouseMode = MouseMode::EDIT;
                _object = Object::FLOOR;
                _objectID = i;
                break;
            }
        }
        refreshWidgets();
    }

    /* Avoiding automatically selecting an object below the mouse after placing a spawn */
    if (_isPlacingSpawn)
        return;

    /* Already dragging */
    else if (_mouseMode == MouseMode::DRAG && eventHandler_.keyDown(SDL_BUTTON_LEFT)) {
        setPositionSpinners(mousePos.x - DRAG_OFFSET.x, mousePos.y - DRAG_OFFSET.y);
        switch (_object) {
        case Object::FLOOR:
            placeFloor(mousePos - DRAG_OFFSET, true);
            return;
        case Object::WALL:
            placeWall(mousePos - DRAG_OFFSET, true);
            return;
        case Object::PLAYER:
            placePlayer(mousePos - DRAG_OFFSET);
            return;
        case Object::QUESTION_PLATFORM:
            placeQuestion(mousePos - DRAG_OFFSET, true);
            return;
        case Object::TELEPORT_PLATFORM:
            placeTeleport(mousePos - DRAG_OFFSET, true);
            return;
        }
    }

    /* Check for dragging objects */
    else if ((_mouseMode == MouseMode::SELECT || _mouseMode == MouseMode::EDIT || _mouseMode == MouseMode::DRAG) &&
             eventHandler_.keyDown(SDL_BUTTON_LEFT)) {
        if (player_.testPoint(mousePos.x, mousePos.y)) {
            _mouseMode = MouseMode::DRAG;
            _object = Object::PLAYER;
            _objectID = PLAYER_ID;
            refreshWidgets();

            DRAG_OFFSET = mousePos - player_.position();
            placePlayer(mousePos - DRAG_OFFSET);
            return;
        }

        for (int i = (int)questions_.size() - 1; i >= 0; i--) {
            if (questions_[i].testPoint(mousePos.x, mousePos.y)) {
                _mouseMode = MouseMode::DRAG;
                _object = Object::QUESTION_PLATFORM;
                _objectID = i;

                refreshWidgets();

                DRAG_OFFSET = mousePos - questions_[_objectID].position();
                placeQuestion(mousePos - DRAG_OFFSET, true);
                return;
            }
        }

        for (int i = (int)teleports_.size() - 1; i >= 0; i--) {
            if (teleports_[i].testPoint(mousePos.x, mousePos.y)) {
                _mouseMode = MouseMode::DRAG;
                _object = Object::TELEPORT_PLATFORM;
                _objectID = i;

                refreshWidgets();

                DRAG_OFFSET = mousePos - teleports_[_objectID].position();
                placeTeleport(mousePos - DRAG_OFFSET, true);
                return;
            }
        }

        for (int i = (int)walls_.size() - 1; i >= 0; i--) {
            if (walls_[i].testPoint(mousePos.x, mousePos.y)) {
                _mouseMode = MouseMode::DRAG;
                _object = Object::WALL;

                /* Bring selected object on top */
                _objectID = walls_.size() - 1;
                WallBlock tmp = walls_[i];
                walls_[i] = walls_[_objectID];
                walls_[_objectID] = tmp;

                refreshWidgets();

                DRAG_OFFSET = mousePos - walls_[_objectID].position();
                placeWall(mousePos - DRAG_OFFSET, true);
                return;
            }
        }

        for (int i = (int)floors_.size() - 1; i >= 0; i--) {
            if (floors_[i].testPoint(mousePos.x, mousePos.y)) {
                _mouseMode = MouseMode::DRAG;
                _object = Object::FLOOR;
                _objectID = i;

                refreshWidgets();

                DRAG_OFFSET = mousePos - floors_[_objectID].position();
                placeFloor(mousePos - DRAG_OFFSET, true);
                return;
            }
        }
    }
}

bool Editor::isMouseInGUI()
{
    /* Check top buttons, saveWindow, loadWindow and editorGroup */
    if (_toMainMenu->isMouseContainedInArea() || _showEditorButton->isMouseContainedInArea() ||
        _saveButton->isMouseContainedInArea() || _loadButton->isMouseContainedInArea() ||
        _saveWindow->isMouseContainedInArea() || _loadWindow->isMouseContainedInArea() ||
        _editorSettings->isMouseContainedInArea() || _newLevelWindow->isMouseContainedInArea())
        return true;
    return false;
}

/* Default parameter for replaceWall is false 
   False value - replacing existing wall at _ObjectID index of the walls_ vector
   True value - putting new wall into walls_ vector */
void Editor::placeWall(const glm::vec2& position, bool replaceWall)
{
    glm::vec2 dimensions(_widthSpinner->getCurrentValue(), _heightSpinner->getCurrentValue());
    float rotation = (float)_rotationSpinner->getCurrentValue() * _TO_RADIANS;

    /* If empty, put default wall texture */
    if (getTextureEditbox() == "Textures/")
        setTextureEditbox(FLOOR_TEXTURE_PATH);

    WallBlock wall;
    wall.init(world_.get(), position, dimensions, dimensions, rotation, b2_staticBody, 
              _GET_SLIDERS_COLOR, getTextureEditbox(), _fixedUV->isSelected(), (float)_uvScaleSpinner->getCurrentValue());

    if (replaceWall) {
        walls_[_objectID].dispose(world_.get());
        walls_[_objectID] = wall;
    }
    else
        walls_.push_back(wall);
}

/* Default parameter for replaceFloor is false */
void Editor::placeFloor(const glm::vec2& position, bool replaceFloor)
{
    glm::vec2 dimensions(_widthSpinner->getCurrentValue(), _heightSpinner->getCurrentValue());
    float rotation = (float)_rotationSpinner->getCurrentValue() * _TO_RADIANS;

    /* If empty, put default floor texture */
    if (getTextureEditbox() == "Textures/")
        setTextureEditbox(FLOOR_TEXTURE_PATH);

    FloorTile floor;
    floor.init(world_.get(), position, dimensions, dimensions, rotation, b2_staticBody,
               _GET_SLIDERS_COLOR, getTextureEditbox(), _fixedUV->isSelected(), (float)_uvScaleSpinner->getCurrentValue());

    if (replaceFloor) {
        floors_[_objectID].dispose(world_.get());
        floors_[_objectID] = floor;
    }
    else
        floors_.push_back(floor);
}

void Editor::placePlayer(const glm::vec2& position)
{
    /* First remove old player and then init player again */
    if (player_.isInit())
        player_.dispose(world_.get());
    player_.init(world_.get(), position, PLAYER_DRAW_RADIUS, PLAYER_DRAW_RADIUS, _GET_SLIDERS_COLOR);
}

/* Default parameter for replaceTeleport is false */
void Editor::placeTeleport(const glm::vec2& position, bool replaceTeleport)
{
    glm::vec2 dimensions(_widthSpinner->getCurrentValue(), _heightSpinner->getCurrentValue());
    float rotation = (float)_rotationSpinner->getCurrentValue() * _TO_RADIANS;

    TeleportPlatform teleport;
    teleport.init(world_.get(), position, dimensions, rotation);

    if (replaceTeleport) {
        if (teleports_[_objectID].hasSpawnPoint())
            teleport.setSpawnPoint(glm::vec2(_xSpawnSpinner->getCurrentValue(), _ySpawnSpinner->getCurrentValue()));
        teleports_[_objectID].dispose(world_.get());
        teleports_[_objectID] = teleport;
    }
    else
        teleports_.push_back(teleport);
}

/* Default parameter for replaceQuestion is false */
void Editor::placeQuestion(const glm::vec2& position, bool replaceQuestion)
{
    glm::vec2 dimensions(_widthSpinner->getCurrentValue(), _heightSpinner->getCurrentValue());
    float rotation = (float)_rotationSpinner->getCurrentValue() * _TO_RADIANS;

    QuestionPlatform question;

    if (replaceQuestion) {
        question.init(world_.get(), position, dimensions, rotation, getQuestionEditbox());
        questions_[_objectID].dispose(world_.get());
        questions_[_objectID] = question;
    }
    else {
        question.init(world_.get(), position, dimensions, rotation);
        questions_.push_back(question);
    }
}

void Editor::refreshSelectedObject()
{
    switch (_object) {
    case Object::WALL:
        placeWall(_GET_SLIDERS_POSITION_VALUE, true);
        break;
    case Object::FLOOR:
        placeFloor(_GET_SLIDERS_POSITION_VALUE, true);
        break;
    case Object::PLAYER:
        placePlayer(_GET_SLIDERS_POSITION_VALUE);
        break;
    case Object::QUESTION_PLATFORM:
        placeQuestion(_GET_SLIDERS_POSITION_VALUE, true);
        break;
    case Object::TELEPORT_PLATFORM:
        placeTeleport(_GET_SLIDERS_POSITION_VALUE, true);
        break;
    }
}

void Editor::refreshWidgets()
{
    _ENABLE_ALL_WIGDETS

    /* Unique */
    _teleportGroup->disable();
    _questionGroup->disable();

    /* I could't put this in the switch statement below */
    if (_object != Object::QUESTION_PLATFORM) {
        _question_Combobox->clearAllSelections();
        _question_Combobox->getEditbox()->setText(CEGUI::String(""));
    }

    switch (_object) {
    /* If placing new object, just disable the widgets, dont try to update them*/
    case Object::WALL:
        if (_objectID == (int)walls_.size()) {
            //setTextureEditbox(WALL_TEXTURE_PATH);
            return;
        }

        setRGBASliders(walls_[_objectID].color());
        setSizeSpinners(walls_[_objectID].hitBox().dimensions().x, walls_[_objectID].hitBox().dimensions().y,
                        walls_[_objectID].hitBox().body()->GetAngle() * _TO_DEGREES);
        setTextureEditbox(walls_[_objectID].texture().filePath);
        setPositionSpinners(walls_[_objectID].position().x, walls_[_objectID].position().y);
        _uvScaleSpinner->setCurrentValue((float)walls_[_objectID].uvScale());
        _fixedUV->setSelected(walls_[_objectID].fixedUV());
        return;
    case Object::FLOOR:
        if (_objectID == (int)floors_.size()) {
            //setTextureEditbox(FLOOR_TEXTURE_PATH);
            return;
        }

        setRGBASliders(floors_[_objectID].color());
        setSizeSpinners(floors_[_objectID].hitBox().dimensions().x, floors_[_objectID].hitBox().dimensions().y,
                        floors_[_objectID].hitBox().body()->GetAngle() * _TO_DEGREES);
        setTextureEditbox(floors_[_objectID].texture().filePath);
        setPositionSpinners(floors_[_objectID].position().x, floors_[_objectID].position().y);
        _uvScaleSpinner->setCurrentValue((float)floors_[_objectID].uvScale());
        _fixedUV->setSelected(floors_[_objectID].fixedUV());
        return;
    case Object::PLAYER:
        _rectSizeGroup->disable();
        _fixedUV->disable();
        _uvScaleSpinner->disable();
        _texture_Combobox->disable();

        if (player_.isInit()) {
            setRGBASliders(player_.color());
            setTextureEditbox(player_.texture().filePath);
            setPositionSpinners(player_.position().x, player_.position().y);
        }
        else
            setTextureEditbox(PLAYER_TEXTURE_PATH);
        return;
    case Object::QUESTION_PLATFORM:
        _questionGroup->enable();

        _fixedUV->disable();
        _uvScaleSpinner->disable();
        _colorGroup->disable();
        _texture_Combobox->disable();
        if (_objectID == (int)questions_.size())
            return;

        setSizeSpinners(questions_[_objectID].dimensions().x, questions_[_objectID].dimensions().y,
                        questions_[_objectID].hitBox().body()->GetAngle() * _TO_DEGREES);
        setPositionSpinners(questions_[_objectID].position().x, questions_[_objectID].position().y);
        setQuestionEditbox(questions_[_objectID].questionFile());
        return;
    case Object::TELEPORT_PLATFORM:
        _teleportGroup->enable();

        _fixedUV->disable();
        _uvScaleLabel->disable();
        _colorGroup->disable();
        _texture_Combobox->disable();
        _xSpawnSpinner->disable();
        _ySpawnSpinner->disable();
        if (_objectID == (int)teleports_.size())
            return;
        if (teleports_[_objectID].hasSpawnPoint()) {
            _xSpawnSpinner->enable();
            _ySpawnSpinner->enable();
            _xSpawnSpinner->setCurrentValue((double)teleports_[_objectID].getSpawnPoint().x);
            _ySpawnSpinner->setCurrentValue((double)teleports_[_objectID].getSpawnPoint().y);
        }

        setSizeSpinners(teleports_[_objectID].dimensions().x, teleports_[_objectID].dimensions().y,
                        teleports_[_objectID].hitBox().body()->GetAngle() * _TO_DEGREES);
        setPositionSpinners(teleports_[_objectID].position().x, teleports_[_objectID].position().y);
        return;
    case Object::NONE:
        setRGBASliders(Engine::Color(255, 255, 255, 255));
        setSizeSpinners(SIZE_SPINNER_DEFAULT, SIZE_SPINNER_DEFAULT, ROTATION_SPINNER_DEFAULT);
        setPositionSpinners(0.0f, 0.0f);
        _fixedUV->setSelected(false);
        _uvScaleSpinner->setCurrentValue(1.0f);
        return;
    }
}

void Editor::setRGBASliders(const Engine::Color& color)
{
    _rColor->setCurrentValue((float)color.r);
    _gColor->setCurrentValue((float)color.g);
    _bColor->setCurrentValue((float)color.b);
    _aColor->setCurrentValue((float)color.a);
}

void Editor::setSizeSpinners(float width, float height, float rotation)
{
    _widthSpinner->setCurrentValue(width);
    _heightSpinner->setCurrentValue(height);
    setRotationSlider(rotation);
}

void Editor::setPositionSpinners(float x, float y)
{
    _xPosSpinner->setCurrentValue(x);
    _yPosSpinner->setCurrentValue(y);
}

void Editor::setSpawnSpinners(float x, float y)
{
    _xSpawnSpinner->setCurrentValue(x);
    _ySpawnSpinner->setCurrentValue(y);
}

void Editor::setRotationSlider(float rotation)
{
    if (rotation < 0.0f)
        rotation += 360.0f;
    else if (rotation > 359.99f)
        rotation -= 360.0f;
    _rotationSpinner->setCurrentValue(rotation);
}

/* Default parameter is an empty string */
void Editor::setTextureEditbox(const std::string& textureName)
{
    std::string name(textureName);
    name.erase(0, std::string("Textures/").size());

    CEGUI::String tmp(name.c_str());
    _texture_Combobox->clearAllSelections();
    for (auto& item : _texture_ListBoxItems) {
        if (tmp == item->getText()) {
            _texture_Combobox->getEditbox()->setText(item->getText());
            return;
        }
    }
    _texture_Combobox->getEditbox()->setText(CEGUI::String(""));
}

void Editor::setQuestionEditbox(const std::string& question)
{
    std::string name(question);
    name.erase(0, std::string("Questions/").size());

    CEGUI::String tmp(name.c_str());
    _question_Combobox->clearAllSelections();
    for (auto& item : _question_ListBoxItems) {
        if (tmp == item->getText()) {
            _question_Combobox->getEditbox()->setText(item->getText());
            return;
        }
    }
    _question_Combobox->getEditbox()->setText(CEGUI::String(""));
}

std::string Editor::getTextureEditbox()
{
    CEGUI::String tmp = _texture_Combobox->getEditbox()->getText();
    std::string temp = tmp.c_str();
    std::string editBoxString = "Textures/" + temp;
    return editBoxString;
}

std::string Editor::getQuestionEditbox()
{
    CEGUI::String tmp = _question_Combobox->getEditbox()->getText();
    std::string temp = tmp.c_str();
    std::string editBoxString = "Questions/" + temp;
    return editBoxString;
}

void Editor::renderLevel()
{
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

    renderDebug(projectionMatrix);
}

void Editor::renderDebug(const glm::mat4& projectionMatrix)
{
    /* Debug mode for editor */
    if (_debugMode->isSelected())
    {
        /* X-Y axis */
        const float AXIS_LENGTH = 100000.0f;
        _debugRenderer.drawLine(glm::vec2(-AXIS_LENGTH, 0.0f), glm::vec2(AXIS_LENGTH, 0.0f), Engine::Color(255, 0, 0, 200));
        _debugRenderer.drawLine(glm::vec2(0.0f, -AXIS_LENGTH), glm::vec2(0.0f, AXIS_LENGTH), Engine::Color(0, 255, 0, 200));

        for (auto& floor : floors_)
            floor.renderDebug(_debugRenderer);
        for (auto& wall : walls_)
            wall.renderDebug(_debugRenderer);
        player_.renderDebug(_debugRenderer);
    }

    /* Draw outlines when placing object */
    if (_mouseMode == MouseMode::PLACE && isMouseInGUI() == false) {
        const glm::vec2 mousePos = _camera.convertScreenToWorldCoords(eventHandler_.mouseCoords());
        const Engine::Color whiteColor(255, 255, 255, 127);
        float rotation = (float)_rotationSpinner->getCurrentValue() * _TO_RADIANS;
        glm::vec2 dimensions;
        glm::vec2 pos;

        switch (_object) {
        case Object::WALL:
        case Object::FLOOR:
        case Object::QUESTION_PLATFORM:
        case Object::TELEPORT_PLATFORM:
            if (_isPlacingSpawn) {
                _debugRenderer.drawCircle(mousePos, whiteColor, 5.0f);
                break;
            }

            dimensions = glm::vec2(_widthSpinner->getCurrentValue(), _heightSpinner->getCurrentValue());
            pos = mousePos - dimensions / 2.0f;

            _debugRenderer.drawBox(glm::vec4(pos, dimensions), whiteColor, rotation);
            break;
        case Object::PLAYER:
            _debugRenderer.drawCircle(mousePos, whiteColor, PLAYER_DRAW_RADIUS);
        }
    }

    /* Since Platforms don't use texture, always call renderDebug */
    for (auto& question : questions_)
        question.renderDebug(_debugRenderer, QUESTION_COLOR);
    for (auto& teleport : teleports_)
        teleport.renderDebug(_debugRenderer, TELEPORT_COLOR);

    /* If in edit mode, atleast draw the outlines of the selected object */
    if (_mouseMode == MouseMode::EDIT || _mouseMode == MouseMode::DRAG) {
        switch (_object) {
        case Object::FLOOR:
            floors_[_objectID].renderDebug(_debugRenderer, _SELECTED_OBJECT_DEBUG_COLOR);
            break;
        case Object::WALL:
            walls_[_objectID].renderDebug(_debugRenderer, _SELECTED_OBJECT_DEBUG_COLOR);
            break;
        case Object::PLAYER:
            player_.renderDebug(_debugRenderer, _SELECTED_OBJECT_DEBUG_COLOR);
            break;
        case Object::QUESTION_PLATFORM:
            questions_[_objectID].renderDebug(_debugRenderer, _SELECTED_OBJECT_DEBUG_COLOR);
            break;
        case Object::TELEPORT_PLATFORM:
            teleports_[_objectID].renderDebug(_debugRenderer, _SELECTED_OBJECT_DEBUG_COLOR);
            break;
        }
    }

    _debugRenderer.setProjectionMatrix(projectionMatrix);
    _debugRenderer.end();
}

void Editor::renderGUI()
{
    /* Render GUI first and then over GUI render ColorPicker */
    _gui.render();

    _textureProgram.use();

    glm::mat4 projectionMatrix = _guiCamera.cameraMatrix();
    GLint pUniform;
    if (_textureProgram.getUniformLocation("P", pUniform) == true)
        glUniformMatrix4fv(pUniform, 1, GL_FALSE, &projectionMatrix[0][0]);

    /* If editorSettings is hidden, don't render ui stuff */
    if (_editorSettings->isVisible() == false)
        return;

    _spriteBatch.begin();

    /* Draw ColorPicker */
    {
        /* Position of ColorPicker is binded to the position of _colorGroup */
        CEGUI::UVector2 pos = _editorSettings->getPosition();
        const glm::vec2 POSITION(pos.d_x.d_offset, -pos.d_y.d_offset - COLOR_PICKER_SIZE.y);
        const glm::vec4 uv(0.0f, 0.0f, 1.0f, 1.0f);
        const Engine::Color color((GLubyte)_rColor->getCurrentValue(), (GLubyte)_gColor->getCurrentValue(),
                                  (GLubyte)_bColor->getCurrentValue(), (GLubyte)(_aColor->getCurrentValue() * 0.7f));
        pos = _colorGroup->getPosition();
        const glm::vec2 OFFSET(pos.d_x.d_offset + 97.0f, -pos.d_y.d_offset -50.0f);
        _spriteBatch.draw(glm::vec4(POSITION + OFFSET, COLOR_PICKER_SIZE), uv, _colorPicker.id, color);
    }

    _spriteBatch.end();
    _textureProgram.unuse();

    /* Mouse always needs to be in front */
    _gui.renderMouseCursor();
}

bool Editor::addWallClicked(const CEGUI::EventArgs& e)
{
    _mouseMode = MouseMode::PLACE;
    _object = Object::WALL;
    _objectID = (int)walls_.size();

    refreshWidgets();
    return true;
}

bool Editor::addFloorClicked(const CEGUI::EventArgs& e)
{
    _mouseMode = MouseMode::PLACE;
    _object = Object::FLOOR;
    _objectID = (int)floors_.size();

    refreshWidgets();
    return true;
}

bool Editor::addPlayerClicked(const CEGUI::EventArgs& e)
{
    _mouseMode = MouseMode::PLACE;
    _object = Object::PLAYER;
    _objectID = PLAYER_ID;

    refreshWidgets();
    return true;
}

bool Editor::addTeleportClicked(const CEGUI::EventArgs& e)
{
    _mouseMode = MouseMode::PLACE;
    _object = Object::TELEPORT_PLATFORM;
    _objectID = (int)teleports_.size();

    refreshWidgets();
    return true;
}

bool Editor::addQuestionClicked(const CEGUI::EventArgs& e)
{
    _mouseMode = MouseMode::PLACE;
    _object = Object::QUESTION_PLATFORM;
    _objectID = (int)questions_.size();

    refreshWidgets();
    return true;
}

bool Editor::placeTeleportSpawnClicked(const CEGUI::EventArgs& e)
{
    if (_object == Object::TELEPORT_PLATFORM) {
        _mouseMode = MouseMode::PLACE;
        _isPlacingSpawn = true;
    }
    return true;
}

bool Editor::toMainMenuClicked(const CEGUI::EventArgs& e)
{
    _gameState = Engine::GameState::POP_STATE;
    _game->stack.pop_back();

    /* Before popping Settings */
    _game->stack.push_back("MainMenu");
    _game->nextGameState("MainMenu");

    return true;
}

bool Editor::showEditorClicked(const CEGUI::EventArgs& e)
{
    /* Toggle window visibility */
    _editorSettings->isVisible() ? _editorSettings->hide() : _editorSettings->show();

    _SET_SELECT_MODE
    return true;
}

bool Editor::openSaveWindowClicked(const CEGUI::EventArgs& e)
{
    /* Refresh list */
    {
        /* Delete previous list */
        for (auto& item : _saveWindow_ListBoxItems) {
            _loadWindow_Combobox->removeItem(item);
            _saveWindow_Combobox->removeItem(item);
        }
        _saveWindow_ListBoxItems.clear();

        std::vector<Engine::File> ls;
        Engine::IOManager::listFiles("Levels", ls);

        /* Add all files to ListBoxItems */
        for (auto& file : ls) {
            /* Do not add directories to list */
            if (file.directoryFlag == true)
                continue;

            /* Remove "Levels/" substring */
            file.path.erase(0, std::string("Levels/").size());
            _saveWindow_ListBoxItems.push_back(new CEGUI::ListboxTextItem(file.path));
            _saveWindow_Combobox->addItem(_saveWindow_ListBoxItems.back());
        }
        _saveWindow_Combobox->clearAllSelections();
    }

    _SET_SELECT_MODE
    _HIDE_LOAD_WINDOW
    _HIDE_NEW_LEVEL_WINDOW
    _SHOW_SAVE_WINDOW
    return true;
}

bool Editor::openLoadWindowClicked(const CEGUI::EventArgs& e)
{
    /* Refresh list */
    {
        /* Delete previous list */
        for (auto& item : _saveWindow_ListBoxItems) {
            _loadWindow_Combobox->removeItem(item);
            _saveWindow_Combobox->removeItem(item);
        }
        _saveWindow_ListBoxItems.clear();

        std::vector<Engine::File> ls;
        Engine::IOManager::listFiles("Levels", ls);

        /* Add all files to ListBoxItems */
        for (auto& file : ls) {
            /* Do not add directories to list */
            if (file.directoryFlag == true)
                continue;

            /* Remove "Levels/" substring */
            file.path.erase(0, std::string("Levels/").size());
            _saveWindow_ListBoxItems.push_back(new CEGUI::ListboxTextItem(file.path));
            _loadWindow_Combobox->addItem(_saveWindow_ListBoxItems.back());
        }
        
        /* Select first item */
        _loadWindow_Combobox->clearAllSelections();
        if (!_saveWindow_ListBoxItems.empty())
            _loadWindow_Combobox->setItemSelectState(_saveWindow_ListBoxItems.front(), true);
    }

    _SET_SELECT_MODE
    _HIDE_SAVE_WINDOW
    _HIDE_NEW_LEVEL_WINDOW
    _SHOW_LOAD_WINDOW
    return true;
}

bool Editor::openNewWindowClicked(const CEGUI::EventArgs & e)
{
    _SET_SELECT_MODE
    _HIDE_SAVE_WINDOW
    _HIDE_LOAD_WINDOW
    _SHOW_NEW_LEVEL_WINDOW
    return true;
}

bool Editor::closeWindowClicked(const CEGUI::EventArgs& e)
{
    _SET_SELECT_MODE
    _HIDE_NEW_LEVEL_WINDOW
    _HIDE_SAVE_WINDOW
    _HIDE_LOAD_WINDOW
    return true;
}

bool Editor::saveButtonClicked(const CEGUI::EventArgs& e)
{
    std::string name = _saveWindow_Combobox->getEditbox()->getText().c_str();
    if (_level.save(name) == false)
        return false;

    _SET_SELECT_MODE
    _HIDE_SAVE_WINDOW
    return true;
}

bool Editor::loadButtonClicked(const CEGUI::EventArgs& e)
{
    std::string name = _loadWindow_Combobox->getSelectedItem()->getText().c_str();
    if (_level.load(name) == false)
        return false;

    _SET_SELECT_MODE
    _HIDE_LOAD_WINDOW
    return true;
}

bool Editor::newLevelButtonClicked(const CEGUI::EventArgs& e)
{
    _level.clearAllObjects();

    _SET_SELECT_MODE
    _HIDE_NEW_LEVEL_WINDOW
    return true;
}




#undef _GAME_MACROS_ENABLED
#undef _LEVEL_MACROS_ENABLED
#undef _HIDE_SAVE_WINDOW
#undef _SHOW_SAVE_WINDOW
#undef _HIDE_LOAD_WINDOW
#undef _SHOW_LOAD_WINDOW
#undef _HIDE_NEW_LEVEL_WINDOW
#undef _SHOW_NEW_LEVEL_WINDOW
#undef _TO_RADIANS
#undef _TO_DEGREES
#undef _SET_SELECT_MODE
#undef _GET_SLIDERS_POSITION_VALUE
#undef _GET_SLIDERS_COLOR
#undef _SELECTED_OBJECT_DEBUG_COLOR
#undef _ENABLE_ALL_WIGDETS