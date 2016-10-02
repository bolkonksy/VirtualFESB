#ifndef _LEVEL_H
#define _LEVEL_H

#ifdef _LEVEL_MACROS_ENABLED

#define player_     _level.Player
#define walls_      _level.Walls
#define floors_     _level.Floors
#define world_      _level.World
#define questions_  _level.Questions
#define teleports_  _level.Teleports

#endif

#include <Box2d/Box2D.h>
#include "Objects/Player.h"
#include "Objects/WallBlock.h"
#include "Objects/FloorTile.h"
#include "Objects/TeleportPlatform.h"
#include "Objects/QuestionPlatform.h"
#include "Objects/ContactListener.h"

/* Player constants */
const float PLAYER_DRAW_RADIUS = 15.0f;

/* Game objects in the level */
enum class Object {
    NONE = -1,
    WALL,
    PLAYER,
    FLOOR,
    QUESTION_PLATFORM,
    TELEPORT_PLATFORM
};

/* Container for game objects - all in one place */
class Level {
public:
    bool init(const std::string& levelName = "");
    void dispose();

    /* Removes all level objects */
    void clearAllObjects();

    /* Saves current level to a text file */
    bool save(const std::string& levelName);
    /* Loads the level from the text file */
    bool load(const std::string& levelName);

    /* Returns index of the first active question it finds, or return -1 if none is active */
    int activeQuestion();
    /* Teleport player, if any teleport has been activated */
    void updateTeleports();

    /* * * Members * * */
    Player Player;
    std::unique_ptr   <b2World>   World;
    std::vector       <WallBlock> Walls;
    std::vector       <FloorTile> Floors;
    std::vector<QuestionPlatform> Questions;
    std::vector<TeleportPlatform> Teleports;

private:
    void reserveObjectsVectorSize();

    /* Save methods */
    void savePlayer    (std::ostream& file);
    void saveWalls     (std::ostream& file, size_t count);
    void saveFloors    (std::ostream& file, size_t count);
    void saveQuestions (std::ostream& file, size_t count);
    void saveTeleports (std::ostream& file, size_t count);

    /* Load methods */
    bool loadPlayer    (std::istream& file);
    bool loadWalls     (std::istream& file, size_t count);
    bool loadFloors    (std::istream& file, size_t count);
    bool loadQuestions (std::istream& file, size_t count);
    bool loadTeleports (std::istream& file, size_t count);

    /* For collision detection on platforms */
    ContactListener _contactListener;
};

#endif