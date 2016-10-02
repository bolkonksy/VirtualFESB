#include "Language.h"

#include <map>

enum LangCode {
    UNDEFINED = -1,  /* UNDEFINED always needs to be the first element */
    START_GAME,
    LEVEL_EDITOR,
    GAME_SETTINGS,
    EXIT_GAME,
    GO_BACK,
    GET_LANGUAGE,
    MAIN_MENU,
    BACK_TO_GAME,
    SHOW_EDITOR,
    SAVE_LEVEL,
    LOAD_LEVEL,
    TOOLS,
    PLAYER,
    PLACE_OBJECT,
    WALL,
    FLOOR,
    PLACE_TELEPORT_PLATFORM,
    PLACE_QUESTION_PLATFORM,
    TELEPORT,
    QUESTION,
    SIZE,
    WIDTH,
    HEIGHT,
    ROTATION,
    SELECT,
    OPTIONS,
    DEBUG,
    TEXTURE,
    LEVEL,
    NEW,
    SAVE,
    LOAD,
    CONFIRM,
    CLEAR_LEVEL_WARNING,
    FIXED_UV,
    PRESS_BUTTON_E,
    FULLSCREEN,
    ON,
    OFF,
    POSITION,
    POS_X,
    POS_Y,
    PLACE_SPAWN,
    VOLUME,
    UV_SCALE,
    CHOOSE_ANSWER,
    CANCEL,
    NEXT_QUESTION
};

static std::map<std::string, LangCode> mapLangCode;

Language::Language()
{
    mapLangCode["START_GAME"]                   = LangCode::START_GAME;
    mapLangCode["LEVEL_EDITOR"]                 = LangCode::LEVEL_EDITOR;
    mapLangCode["GAME_SETTINGS"]                = LangCode::GAME_SETTINGS;
    mapLangCode["EXIT"]                         = LangCode::EXIT_GAME;
    mapLangCode["GO_BACK"]                      = LangCode::GO_BACK;
    mapLangCode["GET_LANGUAGE"]                 = LangCode::GET_LANGUAGE;
    mapLangCode["MAIN_MENU"]                    = LangCode::MAIN_MENU;
    mapLangCode["BACK_TO_GAME"]                 = LangCode::BACK_TO_GAME;
    mapLangCode["SHOW_EDITOR"]                  = LangCode::SHOW_EDITOR;
    mapLangCode["SAVE_LEVEL"]                   = LangCode::SAVE_LEVEL;
    mapLangCode["LOAD_LEVEL"]                   = LangCode::LOAD_LEVEL;
    mapLangCode["TOOLS"]                        = LangCode::TOOLS;
    mapLangCode["PLAYER"]                       = LangCode::PLAYER;
    mapLangCode["PLACE_OBJECT"]                 = LangCode::PLACE_OBJECT;
    mapLangCode["WALL"]                         = LangCode::WALL;
    mapLangCode["FLOOR"]                        = LangCode::FLOOR;
    mapLangCode["SIZE"]                         = LangCode::SIZE;
    mapLangCode["WIDTH"]                        = LangCode::WIDTH;
    mapLangCode["HEIGHT"]                       = LangCode::HEIGHT;
    mapLangCode["ROTATION"]                     = LangCode::ROTATION;
    mapLangCode["SELECT"]                       = LangCode::SELECT;
    mapLangCode["OPTIONS"]                      = LangCode::OPTIONS;
    mapLangCode["DEBUG"]                        = LangCode::DEBUG;
    mapLangCode["TEXTURE"]                      = LangCode::TEXTURE;
    mapLangCode["LEVEL"]                        = LangCode::LEVEL;
    mapLangCode["NEW"]                          = LangCode::NEW;
    mapLangCode["SAVE"]                         = LangCode::SAVE;
    mapLangCode["LOAD"]                         = LangCode::LOAD;
    mapLangCode["CONFIRM"]                      = LangCode::CONFIRM;
    mapLangCode["CLEAR_LEVEL_WARNING"]          = LangCode::CLEAR_LEVEL_WARNING;
    mapLangCode["FIXED_UV"]                     = LangCode::FIXED_UV;
    mapLangCode["PRESS_BUTTON_E"]               = LangCode::PRESS_BUTTON_E;
    mapLangCode["PLACE_TELEPORT_PLATFORM"]      = LangCode::PLACE_TELEPORT_PLATFORM;
    mapLangCode["PLACE_QUESTION_PLATFORM"]      = LangCode::PLACE_QUESTION_PLATFORM;
    mapLangCode["TELEPORT"]                     = LangCode::TELEPORT;
    mapLangCode["QUESTION"]                     = LangCode::QUESTION;
    mapLangCode["FULLSCREEN"]                   = LangCode::FULLSCREEN;
    mapLangCode["ON"]                           = LangCode::ON;
    mapLangCode["OFF"]                          = LangCode::OFF;
    mapLangCode["POSITION"]                     = LangCode::POSITION;
    mapLangCode["POS_X"]                        = LangCode::POS_X;
    mapLangCode["POS_Y"]                        = LangCode::POS_Y;
    mapLangCode["PLACE_SPAWN"]                  = LangCode::PLACE_SPAWN;
    mapLangCode["VOLUME"]                       = LangCode::VOLUME;
    mapLangCode["UV_SCALE"]                     = LangCode::UV_SCALE;
    mapLangCode["CHOOSE_ANSWER"]                = LangCode::CHOOSE_ANSWER;
    mapLangCode["CANCEL"]                       = LangCode::CANCEL;
    mapLangCode["NEXT_QUESTION"]                = LangCode::NEXT_QUESTION;
}

#define English     (_language == Lang::ENG)
#define Croatian    (_language == Lang::CRO)
const std::string Language::getText(const std::string& text)
{
    switch (mapLangCode[text]) {
    case LangCode::START_GAME:
        if (English)  return "Start Game";
        if (Croatian) return "Pokreni igru";
    case LangCode::LEVEL_EDITOR:
        if (English)  return "Level Editor";
        if (Croatian) return "Uredivac nivoa";
    case LangCode::GAME_SETTINGS:
        if (English)  return "Settings";
        if (Croatian) return "Postavke";
    case LangCode::EXIT_GAME:
        if (English)  return "Exit Game";
        if (Croatian) return "Izadi iz igre";
    case LangCode::GO_BACK:
        if (English)  return "Go back";
        if (Croatian) return "Nazad";
    case LangCode::GET_LANGUAGE:
        if (English)  return "Language: English";
        if (Croatian) return "Jezik: Hrvatski";
    case LangCode::MAIN_MENU:
        if (English)  return "Main Menu";
        if (Croatian) return "Glavni izbornik";
    case LangCode::BACK_TO_GAME:
        if (English)  return "Return to game";
        if (Croatian) return "Povratak u igru";
    case LangCode::SHOW_EDITOR:
        if (English)  return "Show editor";
        if (Croatian) return "Prikazi alate";
    case LangCode::SAVE_LEVEL:
        if (English)  return "Save Level";
        if (Croatian) return "Sacuvaj Nivo";
    case LangCode::LOAD_LEVEL:
        if (English)  return "Load Level";
        if (Croatian) return "Ucitaj Nivo";
    case LangCode::TOOLS:
        if (English)  return "Main tools";
        if (Croatian) return "Glavni alati";
    case LangCode::PLAYER:
        if (English)  return "Player (P)";
        if (Croatian) return "Igrac (P)";
    case LangCode::PLACE_OBJECT:
        if (English)  return "Place object";
        if (Croatian) return "Postavi objekt";
    case LangCode::WALL:
        if (English)  return "Wall (L)";
        if (Croatian) return "Zid (L)";
    case LangCode::FLOOR:
        if (English)  return "Floor (O)";
        if (Croatian) return "Pod (O)";
    case LangCode::PLACE_TELEPORT_PLATFORM:
        if (English)  return "Teleport (U)";
        if (Croatian) return "Teleport (U)";
    case LangCode::PLACE_QUESTION_PLATFORM:
        if (English)  return "QuestionPlatform (J)";
        if (Croatian) return "Pitanje (J)";
    case LangCode::TELEPORT:
        if (English)  return "Teleport";
        if (Croatian) return "Teleport";
    case LangCode::QUESTION:
        if (English)  return "Question";
        if (Croatian) return "Pitanje";
    case LangCode::SIZE:
        if (English)  return "Size";
        if (Croatian) return "Velicina";
    case LangCode::WIDTH:
        if (English)  return "Width";
        if (Croatian) return "Sirina";
    case LangCode::HEIGHT:
        if (English)  return "Height";
        if (Croatian) return "Visina";
    case LangCode::ROTATION:
        if (English)  return "Rotation";
        if (Croatian) return "Rotacija";
    case LangCode::SELECT:
        if (English)  return "Select";
        if (Croatian) return "Odaberi";
    case LangCode::OPTIONS:
        if (English)  return "Options";
        if (Croatian) return "Opcije";
    case LangCode::DEBUG:
        if (English)  return "Debug mode";
        if (Croatian) return "Debug mod";
    case LangCode::TEXTURE:
        if (English)  return "Texture";
        if (Croatian) return "Tekstura";
    case LangCode::LEVEL:
        if (English)  return "Level";
        if (Croatian) return "Nivo";
    case LangCode::NEW:
        if (English)  return "New";
        if (Croatian) return "Novi";
    case LangCode::SAVE:
        if (English)  return "Save";
        if (Croatian) return "Sacuvaj";
    case LangCode::LOAD:
        if (English)  return "Load";
        if (Croatian) return "Ucitaj";
    case LangCode::CONFIRM:
        if (English)  return "Confirm";
        if (Croatian) return "Potvrdi";
    case LangCode::CLEAR_LEVEL_WARNING:
        if (English)  return "Any unsaved changes will be lost";
        if (Croatian) return "Sve nespremljene promjene biti ce izgubljene";
    case LangCode::FIXED_UV:
        if (English)  return "Fixed UV";
        if (Croatian) return "Fiksni UV";
    case LangCode::PRESS_BUTTON_E:
        if (English)  return "Press E";
        if (Croatian) return "Stisni E";
    case LangCode::FULLSCREEN:
        if (English)  return "Fullscreen";
        if (Croatian) return "Puni zaslon";
    case LangCode::ON:
        if (English)  return "On";
        if (Croatian) return "Ukljucen";
    case LangCode::OFF:
        if (English)  return "Off";
        if (Croatian) return "Iskljucen";
    case LangCode::POSITION:
        if (English)  return "Position";
        if (Croatian) return "Pozicija";
    case LangCode::POS_X:
        if (English)  return "X";
        if (Croatian) return "X";
    case LangCode::POS_Y:
        if (English)  return "Y";
        if (Croatian) return "Y";
    case LangCode::PLACE_SPAWN:
        if (English)  return "Spawn";
        if (Croatian) return "Spawn";
    case LangCode::VOLUME:
        if (English)  return "Volume";
        if (Croatian) return "Zvuk";
    case LangCode::UV_SCALE:
        if (English)  return "Scale UV";
        if (Croatian) return "Skaliraj UV";
    case LangCode::CHOOSE_ANSWER:
        if (English)  return "Choose correct answer";
        if (Croatian) return "Odaberi tocan odgovor";
    case LangCode::CANCEL:
        if (English)  return "Cancel";
        if (Croatian) return "Odustani";
    case LangCode::NEXT_QUESTION:
        if (English)  return "Next Question";
        if (Croatian) return "Sljedece Pitanje";
    default:
        mapLangCode.erase(text);
        if (English) return "LANGUAGE ERROR";
                     return "GRESKA U JEZIKU";
    }
}

#undef English
#undef Croatian