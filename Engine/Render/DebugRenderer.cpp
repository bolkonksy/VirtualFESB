#include "DebugRenderer.h"

#include <SDL/SDL_stdinc.h> /* For M_PI constant */
#include "Shaders/EngineShaders.h"

namespace Engine
{
    DebugRenderer::~DebugRenderer()
    {
        /* Safe to call twice */
        dispose();
    }

    bool DebugRenderer::init()
    {
        /* Initializing shader program */
        if (_program.compileShadersFromSource(DEBUG_VERT, DEBUG_FRAG) == false)
            return false;
        _program.bindAttribute("vertexPosition");
        _program.bindAttribute("vertexColor");
        if (_program.linkShaders() == false)
            return false;

        /* Create vertex array object */
        glGenVertexArrays(1, &_vao);
        /* Create buffers */
        glGenBuffers(1, &_vbo);
        glGenBuffers(1, &_ibo);

        /* Bind all */
        glBindVertexArray(_vao);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(DebugVertex),
                              (void*)offsetof(DebugVertex, position));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(DebugVertex),
                              (void*)offsetof(DebugVertex, color));

        /* Unbind _vao */
        glBindVertexArray(0);

        return true;
    }

    void DebugRenderer::dispose()
    {
        _program.dispose();

        if (glIsVertexArray(_vao) == GL_TRUE)
            glDeleteVertexArrays(1, &_vao);
        if (glIsBuffer(_vbo) == GL_TRUE)
            glDeleteBuffers(1, &_vbo);
        if (glIsBuffer(_ibo) == GL_TRUE)
            glDeleteBuffers(1, &_ibo);
    }

    void DebugRenderer::drawBox(const glm::vec4 & destRect, const Color & color, float angle)
    {
        static const size_t NUM_VERTS = 4;

        const size_t offset = _verts.size();
        _verts.resize(offset + NUM_VERTS);

        colorVertices(offset, NUM_VERTS, color);

        glm::vec2 halfDimensions(destRect.z / 2.0f, destRect.w / 2.0f);

        glm::vec2 tl(-halfDimensions.x, halfDimensions.y);
        glm::vec2 bl(-halfDimensions.x, -halfDimensions.y);
        glm::vec2 br(halfDimensions.x, -halfDimensions.y);
        glm::vec2 tr(halfDimensions.x, halfDimensions.y);

        glm::vec2 positionOffset(destRect.x, destRect.y);

        _verts[offset].position     = rotatePoint(tl, angle) + halfDimensions + positionOffset;
        _verts[offset + 1].position = rotatePoint(bl, angle) + halfDimensions + positionOffset;
        _verts[offset + 2].position = rotatePoint(br, angle) + halfDimensions + positionOffset;
        _verts[offset + 3].position = rotatePoint(tr, angle) + halfDimensions + positionOffset;

        _indices.reserve(_indices.size() + NUM_VERTS * 2);

        /* Connect four lines */
        _indices.push_back(offset);
        _indices.push_back(offset + 1);

        _indices.push_back(offset + 1);
        _indices.push_back(offset + 2);

        _indices.push_back(offset + 2);
        _indices.push_back(offset + 3);

        _indices.push_back(offset + 3);
        _indices.push_back(offset);
    }

    void DebugRenderer::drawLine(const glm::vec2& pointA, const glm::vec2& pointB, const Color& color)
    {
        static const size_t NUM_VERTS = 2;

        const size_t offset = _verts.size();
        _verts.resize(offset + NUM_VERTS);

        colorVertices(offset, NUM_VERTS, color);

        _verts[offset].position = pointA;
        _verts[offset + 1].position = pointB;
        
        _indices.reserve(_indices.size() + NUM_VERTS * 2);
        
        _indices.push_back(offset);
        _indices.push_back(offset + 1);
    }

    void DebugRenderer::drawCircle(const glm::vec2& center, const Color& color, float radius)
    {
        /* Circle estimation */
        static const size_t NUM_VERTS = 128;

        const size_t offset = _verts.size();
        _verts.resize(offset + NUM_VERTS);

        colorVertices(offset, NUM_VERTS, color);

        for (size_t i = 0; i < NUM_VERTS; i++) {
            float angle = ((float)i / NUM_VERTS) * (float)M_PI * 2.0f;
            _verts[offset + i].position.x = cos(angle) * radius + center.x;
            _verts[offset + i].position.y = sin(angle) * radius + center.y; 
        }
        
        _indices.reserve(_indices.size() + NUM_VERTS * 2);

        /* From first to last vertex */
        for (size_t i = 0; i < NUM_VERTS - 1; i++) {
            _indices.push_back(offset + i);
            _indices.push_back(offset + i + 1);
        }
        
        /* Last to first */
        _indices.push_back(offset + NUM_VERTS - 1);
        _indices.push_back(offset);
    }

    void DebugRenderer::end()
    {
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        /* Create vertex buffer object */
        glBufferData(GL_ARRAY_BUFFER, _verts.size() * sizeof(DebugVertex), nullptr, GL_DYNAMIC_DRAW);
        /* Upload the vertex buffer data */
        glBufferSubData(GL_ARRAY_BUFFER, 0, _verts.size() * sizeof(DebugVertex), _verts.data());
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);
        /* Create vertex buffer object for indices */
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(GLuint), nullptr, GL_DYNAMIC_DRAW);
        /* Upload the vertex buffer indicies data */
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, _indices.size() * sizeof(GLuint), _indices.data());
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        _verts.clear();
        /* Can't clear _indicies yet. Still need _indicies.size() for render() */

        render();
    }

    void DebugRenderer::render()
    {
        _program.use();

        GLint pUniform;
        if (_program.getUniformLocation("P", pUniform) == false)
            return;
        glUniformMatrix4fv(pUniform, 1, GL_FALSE, &_projectionMatrix[0][0]);

        /* Draw */
        glBindVertexArray(_vao);
        glLineWidth(_lineWidth);
        glDrawElements(GL_LINES, _indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        _indices.clear();

        _program.unuse();
    }

    glm::vec2 DebugRenderer::rotatePoint(const glm::vec2& position, float angle)
    {
        glm::vec2 point;
        point.x = position.x * cos(angle) - position.y * sin(angle);
        point.y = position.x * sin(angle) + position.y * cos(angle);
        return point;
    }

    inline void DebugRenderer::colorVertices(const size_t& offset, const size_t& count, const Color& color)
    {
        for (size_t i = 0; i < count; i++)
            _verts[offset + i].color = color;
    }
}