#ifndef _DEBUGRENDERER_H
#define _DEBUGRENDERER_H

#include <vector>
#include <GLM/glm.hpp>
#include "ShaderProgram.h"
#include "Vertex.h"

namespace Engine
{
    /* Verticies in debug mode doesn't require uv coords */
    struct DebugVertex {
        Position position;
        Color    color;
    };

    /* Class for drawing shapes of the objects in the game */
    class DebugRenderer {
    public:
        /* Before using call init() once
         *  1. Recommended: set the projection matrix
         *  2. Draw Lines, Circles or Boxes
         *  3. end()
         * On exit, call dispose()
         */
        ~DebugRenderer();

        bool init();
        void dispose();

        void drawBox(const glm::vec4& destRect, const Color& color, float angle);
        void drawLine(const glm::vec2& pointA, const glm::vec2& pointB, const Color& color);
        void drawCircle(const glm::vec2& center, const Color& color, float radius);

        void end();

        /* Setters */
        inline void setProjectionMatrix(const glm::mat4& matrix) { _projectionMatrix = matrix; }
        inline void setLineWidth(const float lineWidth)          { _lineWidth = (GLfloat)lineWidth; }

    private:
        GLuint _vao = 0; /* Vertex Array Object */
        GLuint _vbo = 0; /* Vertex Buffer Object */
        GLuint _ibo = 0; /* Index Buffer Object */

        GLfloat   _lineWidth        = 1.5f;
        glm::mat4 _projectionMatrix = glm::mat4(1.0f);

        std::vector<DebugVertex> _verts;
        std::vector<GLuint>      _indices;

        ShaderProgram _program;

        /* Functions */
        void render();
        glm::vec2 rotatePoint(const glm::vec2& position, float angle);
        inline void colorVertices(const size_t& offset, const size_t& count, const Color& color);
    };
}

#endif