#ifndef _USERINTERFACE_H
#define _USERINTERFACE_H

#include <SDL/SDL_events.h>
#include <GLM/glm.hpp>
#include <CEGUI/RendererModules/OpenGL/GL3Renderer.h>
#include <CEGUI/CEGUI.h>

namespace Engine
{
    enum class MouseCursor {
        QUERY = -1,
        HIDE,
        SHOW,
        UNKNOWN
    };

    class UserInterface {
    public:
        void init(const std::string& resourceDirectory);
        void dispose();

        void update();
        void render();
        void handleEvents(const SDL_Event& e);
        void cleanCeguiGL();

        /* If parent is not specified, parent is automatically _root */
        CEGUI::Window* createWidget(const std::string& type, const glm::vec4& destRect,
                                    const std::string& name = "", CEGUI::Window* parent = nullptr);

        /* Mouse cursor controls */
        void setMouseCursor(const std::string& imageFile);
        void renderMouseCursor();
        int showMouseCursor(MouseCursor toggle = MouseCursor::QUERY);
        int showMouseCursor(int toggle = -1);

        /* _context setters */
        void setScheme(const std::string& schemeFilePath);
        void setFont(const std::string& fontFilePath);

        /* Getters */
        static CEGUI::OpenGL3Renderer* getRenderer()      { return _renderer; }
        CEGUI::GUIContext*             getContext() const { return _context; }

    private:
        static CEGUI::OpenGL3Renderer* _renderer;
        CEGUI::Window*     _root    = nullptr;
        CEGUI::GUIContext* _context = nullptr;

        static void setWidgetDestRect(CEGUI::Window* widget, const glm::vec4& destRect);
    };
}

#endif