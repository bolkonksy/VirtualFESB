#ifndef _IGAMESTATE_H
#define _IGAMESTATE_H

namespace Engine
{
    enum class GameState {
        NONE,
        IDLE,
        RUNNING,
        PUSH_STATE, /* No resources cleanup */
        POP_STATE,  /* Resource cleanup for the current state */
        EXIT_ALL
    };

    class IGameState {
    public:
        virtual ~IGameState() {}

        virtual bool init() = 0;
        virtual void quit() = 0;

        /* Gameloop functions: handleEvents, update, render */
        virtual void update() = 0;
        virtual void render() = 0;

        /* Getters */
        inline GameState getState() const { return _gameState; }

        /* Setters */
        inline void setState(GameState gameState) { _gameState = gameState; }

    protected:
        GameState _gameState = GameState::NONE;

        /* Should be called within virtual void update() */
        virtual void handleEvents() = 0;
    };
}

#endif