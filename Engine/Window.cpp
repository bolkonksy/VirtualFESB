#include "Window.h"

#include <iostream>
#include <SDL/SDL_stdinc.h>
#include <SDL/SDL_Events.h>

namespace Engine
{
    Window::~Window()
    {
        SDL_GL_DeleteContext(_glContext);
        SDL_DestroyWindow(_SDLWindow);
        _glContext = nullptr;
        _SDLWindow = nullptr;
    }

    bool Window::create(const std::string& windowName, int screenWidth, int screenHeight, Uint32 myFlags)
    {
        _screenWidth = screenWidth;
        _screenHeight = screenHeight;

        /* Setup the flags for window creation */
        Uint32 flags = SDL_WINDOW_OPENGL;
        if (myFlags & HIDDEN)
            flags |= SDL_WINDOW_HIDDEN;
        if (myFlags & VISIBLE)
            flags |= SDL_WINDOW_SHOWN;
        if (myFlags & FULLSCREEN)
            flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
        if (myFlags & BORDERLESS)
            flags |= SDL_WINDOW_BORDERLESS;
        if (myFlags & MINIMIZED)
            flags |= SDL_WINDOW_MINIMIZED;
        if (myFlags & MAXIMIZED)
            flags |= SDL_WINDOW_MAXIMIZED;

        _SDLWindow = SDL_CreateWindow(windowName.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                      screenWidth, screenHeight, flags);
        if (_SDLWindow == nullptr) {
            std::cerr << "Window could not be created!" << std::endl;
            return false;
        }
        SDL_GL_GetDrawableSize(_SDLWindow, &_screenWidth, &_screenHeight);

        /* Hide default mouse cursor */
        SDL_ShowCursor(SDL_DISABLE);

        return initGL();
    }

    void Window::setFullscreen(bool value)
    {
        SDL_SetWindowFullscreen(_SDLWindow, value ? SDL_WINDOW_FULLSCREEN : 0);
        SDL_GL_GetDrawableSize(_SDLWindow, &_screenWidth, &_screenHeight);
    }

    void Window::setViewPort(GLint x, GLint y, GLsizei width, GLsizei height)
    {
        glViewport(x, y, width, height);
    }

    void Window::setVSync(int toggle)
    {
        if (toggle < -1 || toggle > 1)
            return;
        SDL_GL_SetSwapInterval(toggle);
    }

    void Window::warpMouseInPosition(GLint x, GLint y)
    {
        SDL_WarpMouseInWindow(_SDLWindow, x, y);
    }

    void Window::swapBuffer()
    {
        if (_SDLWindow != nullptr) {
            SDL_GL_SwapWindow(_SDLWindow);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glClearColor(_backgroundColor.r, _backgroundColor.g, _backgroundColor.b, _backgroundColor.a);
        }
    }

    bool Window::initGL()
    {
        _glContext = SDL_GL_CreateContext(_SDLWindow);
        if (_glContext == nullptr) {
            std::cerr << "SDL_GLContext could not be created!" << std::endl;
            return false;
        }

        GLenum error = glewInit();
        if (error != GLEW_OK) {
            std::cerr << "GlewInit failed to initialize!" << std::endl;
            return false;
        }

        /* Background color */
        glClearColor(0.0f, 0.0f, 1.0f, 1.0f);

        /* VSync */
        SDL_GL_SetSwapInterval(0);

        /* Aplha blending */
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        /* Set the default viewport */
        glViewport(0, 0, (GLsizei)_screenWidth, (GLsizei)_screenHeight);

        std::cout << " *** OpenGL " << glGetString(GL_VERSION) << " ***" << std::endl;

        return true;
    }
}