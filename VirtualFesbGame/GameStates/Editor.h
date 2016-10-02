#ifndef _EDITOR_H
#define _EDITOR_H

#define _GAME_MACROS_ENABLED  /* Defined in Game.h */
#define _LEVEL_MACROS_ENABLED /* Defined in Level.h */

#include <Box2d/Box2D.h>
#include <Engine/Render/DebugRenderer.h>
#include <Engine/Render/ShaderProgram.h>
#include <Engine/Render/SpriteBatch.h>
#include <Engine/UserInterface.h>
#include <Engine/IGameState.h>
#include <Engine/Camera.h>
#include "../Level.h"

class Game;

enum class MouseMode {
    SELECT,
    PLACE,
    DRAG,
    EDIT
};

/* Constants for _objectID */
const int NO_ID     = -1; /* No object selected */
const int PLAYER_ID = -2; /* Since player is not contained in a vector, this is its ID */

class Editor : public Engine::IGameState {
public:
    Editor(Game* game);

    virtual bool init() override;
    virtual void quit() override;

    virtual void update() override;
    virtual void render() override;

private:
    /* Init functions */
    bool initLevel();
    bool initShaders();
    bool initGUI();


    /* Update functions */
    virtual void handleEvents() override;   /* IGameState abstract function */
    void removeSelectedObject();
    /* Mouse */
    void updateMouseActivity();
    bool isMouseInGUI();
    /* Placing Objects */
    void placeWall      (const glm::vec2& position, bool replaceWall = false);
    void placeFloor     (const glm::vec2& position, bool replaceFloor = false);
    void placePlayer    (const glm::vec2& position);
    void placeTeleport  (const glm::vec2& position, bool replaceTeleport = false);
    void placeQuestion  (const glm::vec2& position, bool replaceQuestion = false);
    /* Refresh changes and widgets */
    void refreshSelectedObject();
    void refreshWidgets();

    /* Widgets setters */
    void setRGBASliders      (const Engine::Color& color);
    void setSizeSpinners     (float width, float height, float rotation);
    void setPositionSpinners (float x, float y);
    void setSpawnSpinners    (float x, float y);
    void setRotationSlider   (float rotation);
    void setTextureEditbox   (const std::string& textureName = "");
    void setQuestionEditbox  (const std::string& question = "");

    /* Widget getters */
    std::string getTextureEditbox();
    std::string getQuestionEditbox();


    /* Render functions */
    void renderLevel();
    void renderDebug(const glm::mat4& projectionMatrix);
    void renderGUI();


    /* CEGUI Events */
    bool addWallClicked             (const CEGUI::EventArgs& e);
    bool addFloorClicked            (const CEGUI::EventArgs& e);
    bool addPlayerClicked           (const CEGUI::EventArgs& e);
    bool addTeleportClicked         (const CEGUI::EventArgs& e);
    bool addQuestionClicked         (const CEGUI::EventArgs& e);
    bool placeTeleportSpawnClicked  (const CEGUI::EventArgs& e);
    bool toMainMenuClicked          (const CEGUI::EventArgs& e);
    bool showEditorClicked          (const CEGUI::EventArgs& e);
    bool openSaveWindowClicked      (const CEGUI::EventArgs& e);
    bool openLoadWindowClicked      (const CEGUI::EventArgs& e);
    bool openNewWindowClicked       (const CEGUI::EventArgs& e);
    bool closeWindowClicked         (const CEGUI::EventArgs& e);
    bool saveButtonClicked          (const CEGUI::EventArgs& e);
    bool loadButtonClicked          (const CEGUI::EventArgs& e);
    bool newLevelButtonClicked      (const CEGUI::EventArgs& e);


    /* * * Members * * */
    Game* _game;
    Level _level;
    
    Engine::UserInterface _gui;
    Engine::Camera        _camera;
    Engine::Camera        _guiCamera;

    /* Mouse working modes */
    MouseMode _mouseMode      = MouseMode::SELECT;
    Object    _object         = Object::NONE;  /* Selected object */
    int       _objectID       = NO_ID;         /* Selected object index */
    bool      _isPlacingSpawn = false;         /* Used for placing teleportSpawn */

    /* SpriteBatch and ShaderProgram */
    Engine::SpriteBatch   _spriteBatch;
    Engine::ShaderProgram _textureProgram;
    Engine::DebugRenderer _debugRenderer;
    Engine::Texture       _colorPicker;


    /* * * * *  CEGUI  * * * * */
    CEGUI::FrameWindow* _editorSettings = nullptr;

    /* Menu bar*/
    CEGUI::Menubar* _menuBar = nullptr;
    /* Level */
    CEGUI::MenuItem*  _levelPopupItem = nullptr;
    CEGUI::PopupMenu* _levelPopupMenu = nullptr;
    CEGUI::MenuItem*  _newLevelItem   = nullptr;
    CEGUI::MenuItem*  _saveLevelItem  = nullptr;
    CEGUI::MenuItem*  _loadLevelItem  = nullptr;
    /* Add */
    CEGUI::MenuItem*  _addPopupItem    = nullptr;
    CEGUI::PopupMenu* _addPopupMenu    = nullptr;
    CEGUI::MenuItem*  _addWallItem     = nullptr;
    CEGUI::MenuItem*  _addFloorItem    = nullptr;
    CEGUI::MenuItem*  _addQuestionItem = nullptr;
    CEGUI::MenuItem*  _addTeleportItem = nullptr;
    CEGUI::MenuItem*  _addPlayerItem   = nullptr;
    
    /* Options settings */
    CEGUI::GroupBox*     _optionsGroup     = nullptr;
    CEGUI::ToggleButton* _debugMode        = nullptr;
    CEGUI::Window*       _textureLabel     = nullptr;
    CEGUI::Combobox*     _texture_Combobox = nullptr;
    CEGUI::ToggleButton* _fixedUV          = nullptr;
    CEGUI::Spinner*      _uvScaleSpinner   = nullptr;
    CEGUI::Window*       _uvScaleLabel     = nullptr;
    std::vector<CEGUI::ListboxTextItem*> _texture_ListBoxItems;

    /* RGBA Color settings */
    CEGUI::GroupBox* _colorGroup = nullptr;
    CEGUI::Slider*   _rColor     = nullptr;
    CEGUI::Slider*   _gColor     = nullptr;
    CEGUI::Slider*   _bColor     = nullptr;
    CEGUI::Slider*   _aColor     = nullptr;

    /* Size settings for rectangles */
    CEGUI::GroupBox* _rectSizeGroup   = nullptr;
    CEGUI::Window*   _widthLabel      = nullptr;
    CEGUI::Spinner*  _widthSpinner    = nullptr;
    CEGUI::Window*   _heightLabel     = nullptr;
    CEGUI::Spinner*  _heightSpinner   = nullptr;
    CEGUI::Window*   _rotationLabel   = nullptr;
    CEGUI::Spinner*  _rotationSpinner = nullptr;

    /* Position settings */
    CEGUI::GroupBox* _positionGroup = nullptr;
    CEGUI::Spinner*  _xPosSpinner   = nullptr;
    CEGUI::Spinner*  _yPosSpinner   = nullptr;
    CEGUI::Window*   _xPosLabel     = nullptr;
    CEGUI::Window*   _yPosLabel     = nullptr;

    /* Teleporter settings */
    CEGUI::GroupBox*   _teleportGroup = nullptr;
    CEGUI::Spinner*    _xSpawnSpinner = nullptr;
    CEGUI::Spinner*    _ySpawnSpinner = nullptr;
    CEGUI::Window*     _xSpawnLabel   = nullptr;
    CEGUI::Window*     _ySpawnLabel   = nullptr;
    CEGUI::PushButton* _placeSpawn    = nullptr;

    /* Question settings */
    CEGUI::GroupBox* _questionGroup     = nullptr;
    CEGUI::Combobox* _question_Combobox = nullptr;
    std::vector<CEGUI::ListboxTextItem*> _question_ListBoxItems;

    /* Main buttons on top */
    CEGUI::PushButton* _toMainMenu       = nullptr;
    CEGUI::PushButton* _showEditorButton = nullptr;
    CEGUI::PushButton* _saveButton       = nullptr;
    CEGUI::PushButton* _loadButton       = nullptr;

    /* Save Window and Load Window */
    CEGUI::FrameWindow* _saveWindow            = nullptr;
    CEGUI::PushButton*  _saveWindow_SaveButton = nullptr;
    CEGUI::Combobox*    _saveWindow_Combobox   = nullptr;
    CEGUI::FrameWindow* _loadWindow            = nullptr;
    CEGUI::PushButton*  _loadWindow_LoadButton = nullptr;
    CEGUI::Combobox*    _loadWindow_Combobox   = nullptr;
    /* Used for both save and load window */
    std::vector<CEGUI::ListboxTextItem*> _saveWindow_ListBoxItems;

    /* New Level Window */
    CEGUI::FrameWindow* _newLevelWindow        = nullptr;
    CEGUI::Window*      _newLevelWindow_Label  = nullptr;
    CEGUI::PushButton*  _newLevelConfirmButton = nullptr;
};

#endif