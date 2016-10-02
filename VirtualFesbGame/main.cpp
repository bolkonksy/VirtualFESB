#include "Game.h"

int main(int argc, char* argv[])
{
    Game game;

    if (game.initGame("Virtual FESB", 800, 600, false) == true)
        game.gameloop();

    return 0;
}