#include "Level.h"

#include <iostream>
#include <fstream>
#include <sstream>

const b2Vec2 WORLD_NO_GRAVITY(0.0f, 0.0f);

enum class ReservedMemory : size_t {
    WALLS     = 200,
    FLOORS    = 200,
    QUESTIONS = 15,
    TELEPORTS = 50
};

size_t operator+(const ReservedMemory& a, const std::size_t& b) { return (size_t)a + b; }
size_t operator+(const std::size_t& b, const ReservedMemory& a) { return (size_t)a + b; }

/* Default value for levelName = "" */
bool Level::init(const std::string& levelName)
{
    _contactListener.init(this);
    clearAllObjects();

    if (levelName.empty()) {
        World = std::make_unique<b2World>(WORLD_NO_GRAVITY);
        World->SetContactListener(&_contactListener);
        if (World.get() == nullptr)
            return false;
    }
    else
        return load(levelName);
    
    reserveObjectsVectorSize();

    return true;
}

void Level::dispose()
{
    clearAllObjects();
    World.reset();
}

void Level::clearAllObjects()
{
    Player.dispose(World.get());
    for (auto& wall : Walls)
        wall.dispose(World.get());
    Walls.clear();
    for (auto& floor : Floors)
        floor.dispose(World.get());
    Floors.clear();
    for (auto& question : Questions)
        question.dispose(World.get());
    Questions.clear();
    for (auto& teleport : Teleports)
        teleport.dispose(World.get());
    Teleports.clear();
}

bool Level::save(const std::string& levelName)
{
    std::ofstream file("Levels/" + levelName, std::ios::binary);
    if (levelName.empty()) {
        std::cerr << "ERROR: Cannot save to noname text file!" << std::endl;
        return false;
    }
    else if (file.fail()) {
        std::cerr << "ERROR: " << levelName << " failed to open!" << std::endl; 
        return false;
    }

    if (Player.isInit())
        savePlayer(file);
    if (!Walls.empty())
        saveWalls(file, Walls.size());
    if (!Floors.empty())
        saveFloors(file, Floors.size());
    if (!Questions.empty())
        saveQuestions(file, Questions.size());
    if (!Teleports.empty())
        saveTeleports(file, Teleports.size());

    file.close();
    return true;
}

bool Level::load(const std::string& levelName)
{
    std::ifstream file("Levels/" + levelName, std::ios::binary);
    if (levelName.empty()) {
        std::cerr << "ERROR: Cannot load a noname text file!" << std::endl;
        return false;
    }
    else if (file.fail()) {
        std::cerr << "ERROR: " << levelName << " failed to open!" << std::endl; 
        return false;
    }

    clearAllObjects();
    /* After clearing everything, level is ready to be loaded */
    World = std::make_unique<b2World>(WORLD_NO_GRAVITY);
    World->SetContactListener(&_contactListener);

    /* Fill level with the objects */
    int    objectType = (int)Object::NONE;
    size_t objectCount = 0;
    std::string line;
    while (!file.eof()) {
        std::getline(file, line);

        /* Ignore empty lines */
        if (line.empty())
            continue;
        std::stringstream(line) >> objectType >> objectCount;

        switch ((Object)objectType) {
        case Object::PLAYER:
            if (!loadPlayer(file)) {
                std::cerr << "ERROR: Loading Player error!" << std::endl;
                file.close();
                return false;
            }
            break;
        case Object::WALL:
            if (!loadWalls(file, objectCount)) {
                std::cerr << "ERROR: Loading Walls error!" << std::endl;
                file.close();
                return false;
            }
            break;
        case Object::FLOOR:
            if (!loadFloors(file, objectCount)) {
                std::cerr << "ERROR: Loading Floors error!" << std::endl;
                file.close();
                return false;
            }
            break;
        case Object::QUESTION_PLATFORM:
            if (!loadQuestions(file, objectCount)) {
                std::cerr << "ERROR: Loading Questions error!" << std::endl;
                file.close();
                return false;
            }
            break;
        case Object::TELEPORT_PLATFORM:
            if (!loadTeleports(file, objectCount)) {
                std::cerr << "ERROR: Loading Teleports error!" << std::endl;
                file.close();
                return false;
            }
            break;

        default:
            std::cerr << "ERROR: Cannot load the file! File probably corrupted!" << std::endl;
            clearAllObjects();
            file.close();
            return false;
        }
    }

    file.close();
    return true;
}

int Level::activeQuestion()
{
    for (size_t i = 0; i < Questions.size(); i++) {
        if (Questions[i].getActive()) {
            if (Questions[i].isSolved())
                return -1;
            else
                return (int)i;
        }

    }

    return -1;
}

void Level::updateTeleports()
{
    for (auto& teleport : Teleports) {
        if (teleport.getActive()) {
            teleport.teleportPlayer(Player);
            teleport.setActive(false);
            return;
        }
    }
}

void Level::reserveObjectsVectorSize()
{
    if (Walls.empty())
        Walls.reserve((size_t)ReservedMemory::WALLS);
    if (Floors.empty())
        Floors.reserve((size_t)ReservedMemory::FLOORS);
    if (Questions.empty())
        Questions.reserve((size_t)ReservedMemory::QUESTIONS);
    if (Teleports.empty())
        Teleports.reserve((size_t)ReservedMemory::TELEPORTS);
}

void Level::savePlayer(std::ostream& file)
{
    /* Player indication line */
    file << (int)Object::PLAYER << ' ' << 1 << std::endl;

    /* Player properties in order: 
       Position -> RenderRadius -> ColisionRadius -> Color */
    file << ' ' << Player.position().x << ' ' << Player.position().y << ' ' << PLAYER_DRAW_RADIUS << ' '
         << PLAYER_DRAW_RADIUS << ' ' << Player.color().r << ' ' << Player.color().g << ' '
         << Player.color().b << ' ' << Player.color().a << std::endl;
}

void Level::saveWalls(std::ostream& file, size_t count)
{
    /* Walls indication line */
    file << (int)Object::WALL << ' ' << count << std::endl;

    /* WallBlock properties in order: 
       Position -> RenderDimensions -> ColisionDimensions -> Angle -> Color -> UV Info */
    for (auto& wall : Walls)
        file << ' ' << wall.position().x << ' ' << wall.position().y << ' ' << wall.renderDims().x << ' '
             << wall.renderDims().y << ' ' << wall.collisionDims().x << ' ' << wall.collisionDims().y << ' '
             << ' ' << wall.hitBox().body()->GetAngle() << ' ' << wall.color().r << ' ' << wall.color().g << ' '
             << wall.color().b << ' ' << wall.color().a << ' ' << wall.texture().filePath << ' ' 
             << wall.fixedUV() << ' ' << wall.uvScale() << std::endl;
}

void Level::saveFloors(std::ostream& file, size_t count)
{
    /* Floors indication line */
    file << (int)Object::FLOOR << ' ' << count << std::endl;

    /* FloorTile properties in order: 
       Position -> RenderDimensions -> ColisionDimensions -> Angle -> Color -> UV info */
    for (auto& floor : Floors)
        file << ' ' << floor.position().x << ' ' << floor.position().y << ' ' << floor.renderDims().x << ' '
             << floor.renderDims().y << ' ' << floor.collisionDims().x << ' ' << floor.collisionDims().y << ' '
             << ' ' << floor.hitBox().body()->GetAngle() << ' ' << floor.color().r << ' ' << floor.color().g << ' '
             << floor.color().b << ' ' << floor.color().a << ' ' << floor.texture().filePath << ' ' 
             << floor.fixedUV() << ' ' << floor.uvScale() << std::endl;
}

void Level::saveQuestions(std::ostream& file, size_t count)
{
    /* Questions indication line */
    file << (int)Object::QUESTION_PLATFORM << ' ' << count << std::endl;

    /* Question properties in order: 
       Position -> Dimensions -> Angle -> QuestionFile */
    for (auto& question : Questions)
        file << ' ' << question.position().x << ' ' << question.position().y << ' ' << question.dimensions().x << ' '
             << question.dimensions().y << ' ' << question.hitBox().body()->GetAngle() << ' '
             << question.questionFile() << std::endl;
}

void Level::saveTeleports(std::ostream& file, size_t count)
{
    /* Teleport indication line */
    file << (int)Object::TELEPORT_PLATFORM << ' ' << count << std::endl;

    /* Teleport properties in order: 
       Position -> Dimensions -> Angle -> HasSpawnPoint -> SpawnPoint */
    for (auto& teleport : Teleports)
        file << ' ' << teleport.position().x << ' ' << teleport.position().y << ' ' << teleport.dimensions().x << ' '
             << teleport.dimensions().y << ' ' << teleport.hitBox().body()->GetAngle() << ' '
             << teleport.hasSpawnPoint() << ' ' << teleport.getSpawnPoint().x << ' ' << teleport.getSpawnPoint().y << std::endl;
}

bool Level::loadPlayer(std::istream& file)
{
    glm::vec2 pos;
    float renderRadius, collisionRadius;
    Engine::Color color;
    
    std::string line;
    std::getline(file, line);
    std::stringstream(line) >> pos.x >> pos.y >> renderRadius >> collisionRadius >> color.r >> color.g >> color.b >> color.a;
    if (!Player.init(World.get(), pos, renderRadius, collisionRadius, color))
        return false;
    return true;
}

bool Level::loadWalls(std::istream& file, size_t count)
{
    glm::vec2 pos, renderDimensions, collisionDimensions;
    float angle, uvScale = 1.0f;
    bool fixedUV;
    Engine::Color color;
    std::string texturePath;

    Walls.reserve(ReservedMemory::WALLS + count);
    for (size_t i = 0; i < count; i++) {
        std::string line;
        std::getline(file, line);
        std::stringstream(line) >> pos.x >> pos.y >> renderDimensions.x >> renderDimensions.y >> collisionDimensions.x
                                >> collisionDimensions.y >> angle >> color.r >> color.g >> color.b >> color.a >> texturePath
                                >> fixedUV >> uvScale;

        WallBlock wall;
        if (!wall.init(World.get(), pos, renderDimensions, collisionDimensions, angle, b2_staticBody, color, texturePath,
                       fixedUV, uvScale))
            return false;
        Walls.push_back(wall);
    }

    return true;
}

bool Level::loadFloors(std::istream& file, size_t count)
{
    glm::vec2 pos, renderDimensions, collisionDimensions;
    float angle, uvScale = 1.0f;
    bool fixedUV;
    Engine::Color color;
    std::string texturePath;

    Floors.reserve(count + ReservedMemory::FLOORS);
    for (size_t i = 0; i < count; i++) {
        std::string line;
        std::getline(file, line);
        std::stringstream(line) >> pos.x >> pos.y >> renderDimensions.x >> renderDimensions.y >> collisionDimensions.x
                                >> collisionDimensions.y >> angle >> color.r >> color.g >> color.b >> color.a >> texturePath
                                >> fixedUV >> uvScale;

        FloorTile floor;
        if (!floor.init(World.get(), pos, renderDimensions, collisionDimensions, angle, b2_staticBody, color, texturePath,
                        fixedUV, uvScale))
            return false;
        Floors.push_back(floor);
    }

    return true;
}

bool Level::loadQuestions(std::istream& file, size_t count)
{
    glm::vec2 pos, dimensions;
    float angle;
    std::string questionFile;

    Questions.reserve(count + ReservedMemory::QUESTIONS);
    for (size_t i = 0; i < count; i++) {
        std::string line;
        std::getline(file, line);
        std::stringstream(line) >> pos.x >> pos.y >> dimensions.x >> dimensions.y >> angle >> questionFile;

        QuestionPlatform question;
        question.init(World.get(), pos, dimensions, angle, questionFile);
        Questions.push_back(question);
    }

    return true;
}

bool Level::loadTeleports(std::istream& file, size_t count)
{
    glm::vec2 pos, dimensions, spawn;
    float angle;
    bool hasSpawn;

    Teleports.reserve(count + ReservedMemory::TELEPORTS);
    for (size_t i = 0; i < count; i++) {
        std::string line;
        std::getline(file, line);
        std::stringstream(line) >> pos.x >> pos.y >> dimensions.x >> dimensions.y >> angle
                                >> hasSpawn >> spawn.x >> spawn.y;

        TeleportPlatform teleport;
        if (hasSpawn)
            teleport.init(World.get(), pos, dimensions, angle, spawn);
        else
            teleport.init(World.get(), pos, dimensions, angle);
        Teleports.push_back(teleport);
    }

    return true;
}
