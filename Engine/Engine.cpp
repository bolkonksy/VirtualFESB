#include "Engine.h"

#include <iostream>
#include <SDL/SDL.h>

namespace Engine
{
    bool initEngine()
    {
        if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
            std::cerr << "SDL could not be initialized - " << SDL_GetError() << std::endl;
            system("pause");
            return false;
        }

        /* Print SDL version */
        SDL_version linked;
        SDL_GetVersion(&linked);
        std::cout << " *** SDL " << (int)linked.major << "." << (int)linked.minor << "."
                  << (int)linked.patch << " ***" << std::endl;

        /* Use double buffer for OpenGL renderning to avoid any flickering */
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

        /* Hide mouse cursor */
        SDL_ShowCursor(0);

        return true;
    }

    void quitEngine()
    {
        std::cout << "Quiting engine..." << std::endl;
        if (SDL_WasInit (SDL_INIT_EVERYTHING))
            SDL_Quit();
    }
}