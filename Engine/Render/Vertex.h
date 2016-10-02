#ifndef _VERTEX_H
#define _VERTEX_H

#include <GL/glew.h>
#include <GLM/glm.hpp>

namespace Engine
{
    struct Position {
        Position() : x(0.0f), y(0.0f) {}
        Position(float X, float Y) : 
            x(X), y(Y) {}
        Position(const glm::vec2& position) : 
            x(position.x), y(position.y) {}

        float x;
        float y;
    };

    struct Color {
        /* No need for error checking since values casted to GLubyte are in range 0-255 */
        Color() : r(0), g(0), b(0), a(255) {}
        Color(GLubyte R, GLubyte G, GLubyte B) :
            r(R), g(G), b(B), a(255) {}
        Color(GLubyte R, GLubyte G, GLubyte B, GLubyte A) :
            r(R), g(G), b(B), a(A) {}

        GLubyte r;
        GLubyte g;
        GLubyte b;
        GLubyte a;

        /* Converts RGBA values from 0-255 to 0.0f-1.0f */
        glm::vec4 Normalize() const { return glm::vec4(r / 256.0f, g / 256.0f, b / 256.0f, a / 256.0f); }

        static Color White()  { return Color(240, 240, 240, 255); }
        static Color Red()    { return Color(168, 13,  10,  255); }
        static Color Blue()   { return Color(13,  18,  147, 255); }
        static Color Green()  { return Color(11,  156, 49,  255); }
        static Color Purple() { return Color(190, 89,  184, 255); }
        static Color Yellow() { return Color(254, 226, 62,  255); }
        static Color Black()  { return Color(0,   0,   0,   255); }
    };

    struct UV {
        UV() : u(1.0f), v(1.0f) {}
        UV(float U, float V) : 
            u(U), v(V) {}
        UV(const glm::vec2& uv) : 
            u(uv.x), v(uv.y) {}

        float u;
        float v;
    };

    struct Vertex {
        Position position;
        Color    color;
        UV       uv;

        void setPosition(float x, float y)
        {
            position = Position(x, y);
        }

        inline void setColor(GLubyte r, GLubyte g, GLubyte b, GLubyte a)
        {
            color = Color(r, g, b, a);
        }
        void setColor(GLubyte r, GLubyte g, GLubyte b)
        {
            color = Color(r, g, b);
        }

        void setUV(float u, float v)
        {
            uv = UV(u, v);
        }
    };
}

#endif
