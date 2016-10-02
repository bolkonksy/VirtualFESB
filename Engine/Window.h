#ifndef _WINDOW_H
#define _WINDOW_H

#include <string>
#include <SDL/SDL_video.h>
#include <GL/glew.h>
#include "Render/Vertex.h"

namespace Engine
{
    enum WindowFlag {
        HIDDEN = 0x1,
        VISIBLE = 0x2,
        FULLSCREEN = 0x4,
        BORDERLESS = 0x8,
        MINIMIZED = 0x10,
        MAXIMIZED = 0x20
    };

    class Window {
    public:
        /* Deletes SDL_Window and SDL_GLContext */
        ~Window();

        bool create(const std::string& windowName, int screenWidth, int screenHeight, Uint32 myFlags);
        
        void setFullscreen(bool value);
        void setViewPort(GLint x, GLint y, GLsizei width, GLsizei height);
        void setVSync(int toggle);
        void warpMouseInPosition(GLint x, GLint y);

        /* Swap the OpenGL buffers for a window, if double buffering is supported */
        void swapBuffer();

        /* Setters */
        inline void setBackgroundColor(const Color& color) { _backgroundColor = color.Normalize(); }

        /* Getters */
        bool isFullscreen() const { return SDL_GetWindowFlags(_SDLWindow) & SDL_WINDOW_FULLSCREEN; };
        int  width()        const { return _screenWidth; }
        int  height()       const { return _screenHeight; }
        /* Used for easier and shorter operations, example: width(2.0f) returns half of the screenWidth */
        float width(float fraction) const 
        { 
            if (fraction <= 0.0f)
                fraction = 1.0f;
            return _screenWidth / fraction;
        }
        /* Used for easier and shorter operations, example: height(2.0f) returns half of the screenHeight */
        float height(float fraction) const
        { 
            if (fraction <= 0.0f)
                fraction = 1.0f;
            return _screenHeight / fraction;
        }

    private:
        int _screenWidth  = 800;
        int _screenHeight = 600;

        /* Using glm::vec4 for normalized color */
        glm::vec4 _backgroundColor = glm::vec4(1.0f); 

        SDL_Window*   _SDLWindow = nullptr;
        SDL_GLContext _glContext = nullptr;

        bool initGL();
    };
}

#endif