#include "UserInterface.h"

#include <iostream>
#include <SDL\SDL_timer.h>

CEGUI::Key::Scan SDLKeyToCEGUIKey(SDL_Keycode key);
CEGUI::MouseButton SDLButtonToCEGUIButton(Uint8 sdlButton);

namespace Engine
{
    CEGUI::OpenGL3Renderer* UserInterface::_renderer = nullptr;

    void UserInterface::init(const std::string& resourceDirectory)
    {
        if (_renderer == nullptr)
            _renderer = &CEGUI::OpenGL3Renderer::bootstrapSystem();

        CEGUI::DefaultResourceProvider* rp =
            static_cast<CEGUI::DefaultResourceProvider*>(CEGUI::System::getSingleton().getResourceProvider());
        
        rp->setResourceGroupDirectory("imagesets", resourceDirectory + "/imagesets/");
        rp->setResourceGroupDirectory("looknfeels", resourceDirectory + "/looknfeel/");
        rp->setResourceGroupDirectory("lua_scripts", resourceDirectory + "/lua_scripts/");
        rp->setResourceGroupDirectory("layouts", resourceDirectory + "/layouts/");
        rp->setResourceGroupDirectory("schemes", resourceDirectory + "/schemes/");
        rp->setResourceGroupDirectory("fonts", resourceDirectory + "/fonts/");

        CEGUI::ImageManager::setImagesetDefaultResourceGroup("imagesets");
        CEGUI::WidgetLookManager::setDefaultResourceGroup("looknfeels");
        CEGUI::ScriptModule::setDefaultResourceGroup("lua_scripts");
        CEGUI::WindowManager::setDefaultResourceGroup("layouts");
        CEGUI::Scheme::setDefaultResourceGroup("schemes");
        CEGUI::Font::setDefaultResourceGroup("fonts");

        _context = &CEGUI::System::getSingleton().createGUIContext(_renderer->getDefaultRenderTarget());
        _root = CEGUI::WindowManager::getSingleton().createWindow("DefaultWindow", "root");
        _context->setRootWindow(_root);
    }

    void UserInterface::dispose()
    {
        CEGUI::System::getSingleton().destroyGUIContext(*_context);
        CEGUI::WindowManager::getSingleton().destroyWindow(_root);
        _context = nullptr;
        _root = nullptr;
    }

    /* send delta from FPScounter */
    void UserInterface::update() 
    {
        static Uint32 previousTime = SDL_GetTicks();

        Uint32 currentTime = SDL_GetTicks();
        Uint32 deltaTime = currentTime - previousTime;
        _context->injectTimePulse((float)deltaTime / 1000.0f);

        previousTime = currentTime;
    }

    void UserInterface::render()
    {
        glDisable(GL_DEPTH_TEST);
        _renderer->beginRendering();
        _context->draw();
        _renderer->endRendering();

        /* CEGUI Cleanup */
        cleanCeguiGL();
    }

    void UserInterface::handleEvents(const SDL_Event& e)
    {
        CEGUI::String ceguiString;
        switch (e.type) {
        case SDL_MOUSEMOTION:
            _context->injectMousePosition((float)e.motion.x, (float)e.motion.y);
            break;
        case SDL_KEYDOWN:
            _context->injectKeyDown(SDLKeyToCEGUIKey(e.key.keysym.sym));
            break;
        case SDL_KEYUP:
            _context->injectKeyUp(SDLKeyToCEGUIKey(e.key.keysym.sym));
            break;
        case SDL_TEXTINPUT:
            ceguiString = CEGUI::String((CEGUI::utf8*) e.text.text);
            for (size_t i = 0; i < ceguiString.size(); i++)
                _context->injectChar(ceguiString[i]);
            break;
        case SDL_MOUSEBUTTONDOWN:
            _context->injectMouseButtonDown(SDLButtonToCEGUIButton(e.button.button));
            break;
        case SDL_MOUSEBUTTONUP:
            _context->injectMouseButtonUp(SDLButtonToCEGUIButton(e.button.button));
            break;
        }
    }

    void UserInterface::cleanCeguiGL()
    {
        glDisable(GL_SCISSOR_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisableClientState(GL_COLOR_ARRAY);
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }

    /* Default parameters: 
     * CEGUI::Window* parent = nullptr */
    CEGUI::Window* UserInterface::createWidget(const std::string& type, const glm::vec4& destRect,
                                               const std::string& name, CEGUI::Window* parent)
    {
        CEGUI::Window* window = CEGUI::WindowManager::getSingleton().createWindow(type, name); 
        setWidgetDestRect(window, destRect);

        (parent != nullptr) ? parent->addChild(window) : _root->addChild(window);
        return window;
    }

    void UserInterface::setWidgetDestRect(CEGUI::Window* widget, const glm::vec4& destRect)
    {
        widget->setPosition(CEGUI::UVector2(CEGUI::UDim(0.0f, destRect.x),
                            CEGUI::UDim(0.0f, destRect.y)));
        widget->setSize(CEGUI::USize(CEGUI::UDim(0.0f, destRect.z),
                        CEGUI::UDim(0.0f, destRect.w)));
    }

    void UserInterface::setMouseCursor(const std::string& imageFile)
    {
        _context->getMouseCursor().setDefaultImage(imageFile);
        glm::ivec2 pos;
        SDL_GetMouseState(&(pos.x), &(pos.y));
        _context->getMouseCursor().setPosition(CEGUI::Vector2f((float)pos.x, (float)pos.y));
    }

    void UserInterface::renderMouseCursor()
    {
        _renderer->beginRendering();
        _context->getMouseCursor().draw();
        _renderer->endRendering();
        cleanCeguiGL();
    }

    /* Default parameter for toggle is MouseCursor::QUERY */
    int UserInterface::showMouseCursor(MouseCursor toggle)
    {
        switch (toggle) {
        case MouseCursor::QUERY:
            if (_context->getMouseCursor().isVisible())
                return 1;
            return 0;
        case MouseCursor::HIDE:
            _context->getMouseCursor().hide();
            return 0;
        case MouseCursor::SHOW:
            _context->getMouseCursor().show();
            return 1;
        default:
            std::cerr << "showMouseCursor ERROR: Unknown toggle call!" << std::endl;
            return -1;
        }
    }

    /* Default parametr for toggle is -1 or MouseCursor::QUERY */
    int UserInterface::showMouseCursor(int toggle)
    {
        if (toggle < -1 || toggle > 1)
            return showMouseCursor(MouseCursor::UNKNOWN); 
        return showMouseCursor((MouseCursor)toggle); 
    }

    void UserInterface::setScheme(const std::string& schemeFilePath)
    {
        CEGUI::SchemeManager::getSingleton().createFromFile(schemeFilePath + ".scheme");
    }

    void UserInterface::setFont(const std::string& fontFilePath)
    {
        CEGUI::FontManager::getSingleton().createFromFile(fontFilePath + ".font");
        _context->setDefaultFont(fontFilePath);
    }
}

CEGUI::Key::Scan SDLKeyToCEGUIKey(SDL_Keycode keyCode)
{
    using namespace CEGUI;
    switch (keyCode) {
    case SDLK_BACKSPACE:    return Key::Backspace;
    case SDLK_TAB:          return Key::Tab;
    case SDLK_RETURN:       return Key::Return;
    case SDLK_PAUSE:        return Key::Pause;
    case SDLK_ESCAPE:       return Key::Escape;
    case SDLK_SPACE:        return Key::Space;
    case SDLK_COMMA:        return Key::Comma;
    case SDLK_MINUS:        return Key::Minus;
    case SDLK_PERIOD:       return Key::Period;
    case SDLK_SLASH:        return Key::Slash;
    case SDLK_0:            return Key::Zero;
    case SDLK_1:            return Key::One;
    case SDLK_2:            return Key::Two;
    case SDLK_3:            return Key::Three;
    case SDLK_4:            return Key::Four;
    case SDLK_5:            return Key::Five;
    case SDLK_6:            return Key::Six;
    case SDLK_7:            return Key::Seven;
    case SDLK_8:            return Key::Eight;
    case SDLK_9:            return Key::Nine;
    case SDLK_COLON:        return Key::Colon;
    case SDLK_SEMICOLON:    return Key::Semicolon;
    case SDLK_EQUALS:       return Key::Equals;
    case SDLK_LEFTBRACKET:  return Key::LeftBracket;
    case SDLK_BACKSLASH:    return Key::Backslash;
    case SDLK_RIGHTBRACKET: return Key::RightBracket;
    case SDLK_a:            return Key::A;
    case SDLK_b:            return Key::B;
    case SDLK_c:            return Key::C;
    case SDLK_d:            return Key::D;
    case SDLK_e:            return Key::E;
    case SDLK_f:            return Key::F;
    case SDLK_g:            return Key::G;
    case SDLK_h:            return Key::H;
    case SDLK_i:            return Key::I;
    case SDLK_j:            return Key::J;
    case SDLK_k:            return Key::K;
    case SDLK_l:            return Key::L;
    case SDLK_m:            return Key::M;
    case SDLK_n:            return Key::N;
    case SDLK_o:            return Key::O;
    case SDLK_p:            return Key::P;
    case SDLK_q:            return Key::Q;
    case SDLK_r:            return Key::R;
    case SDLK_s:            return Key::S;
    case SDLK_t:            return Key::T;
    case SDLK_u:            return Key::U;
    case SDLK_v:            return Key::V;
    case SDLK_w:            return Key::W;
    case SDLK_x:            return Key::X;
    case SDLK_y:            return Key::Y;
    case SDLK_z:            return Key::Z;
    case SDLK_DELETE:       return Key::Delete;
    case SDLK_KP_PERIOD:    return Key::Decimal;
    case SDLK_KP_DIVIDE:    return Key::Divide;
    case SDLK_KP_MULTIPLY:  return Key::Multiply;
    case SDLK_KP_MINUS:     return Key::Subtract;
    case SDLK_KP_PLUS:      return Key::Add;
    case SDLK_KP_ENTER:     return Key::NumpadEnter;
    case SDLK_KP_EQUALS:    return Key::NumpadEquals;
    case SDLK_UP:           return Key::ArrowUp;
    case SDLK_DOWN:         return Key::ArrowDown;
    case SDLK_RIGHT:        return Key::ArrowRight;
    case SDLK_LEFT:         return Key::ArrowLeft;
    case SDLK_INSERT:       return Key::Insert;
    case SDLK_HOME:         return Key::Home;
    case SDLK_END:          return Key::End;
    case SDLK_PAGEUP:       return Key::PageUp;
    case SDLK_PAGEDOWN:     return Key::PageDown;
    case SDLK_F1:           return Key::F1;
    case SDLK_F2:           return Key::F2;
    case SDLK_F3:           return Key::F3;
    case SDLK_F4:           return Key::F4;
    case SDLK_F5:           return Key::F5;
    case SDLK_F6:           return Key::F6;
    case SDLK_F7:           return Key::F7;
    case SDLK_F8:           return Key::F8;
    case SDLK_F9:           return Key::F9;
    case SDLK_F10:          return Key::F10;
    case SDLK_F11:          return Key::F11;
    case SDLK_F12:          return Key::F12;
    case SDLK_F13:          return Key::F13;
    case SDLK_F14:          return Key::F14;
    case SDLK_F15:          return Key::F15;
    case SDLK_RSHIFT:       return Key::RightShift;
    case SDLK_LSHIFT:       return Key::LeftShift;
    case SDLK_RCTRL:        return Key::RightControl;
    case SDLK_LCTRL:        return Key::LeftControl;
    case SDLK_RALT:         return Key::RightAlt;
    case SDLK_LALT:         return Key::LeftAlt;
    case SDLK_SYSREQ:       return Key::SysRq;
    case SDLK_MENU:         return Key::AppMenu;
    case SDLK_POWER:        return Key::Power;
    default:                return Key::Unknown;
    }
}

CEGUI::MouseButton SDLButtonToCEGUIButton(Uint8 sdlButton)
{
    switch (sdlButton) {
    case SDL_BUTTON_LEFT:    return CEGUI::MouseButton::LeftButton;
    case SDL_BUTTON_MIDDLE:  return CEGUI::MouseButton::MiddleButton;
    case SDL_BUTTON_RIGHT:   return CEGUI::MouseButton::RightButton;
    case SDL_BUTTON_X1:      return CEGUI::MouseButton::X1Button;
    case SDL_BUTTON_X2:      return CEGUI::MouseButton::X2Button;
    default:                 return CEGUI::MouseButton::NoButton;
    }
}