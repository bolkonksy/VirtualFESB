#ifndef _SPRITEBATCH_H
#define _SPRITEBATCH_H

#include <vector>
#include <GL/glew.h>
#include <GLM/glm.hpp>
#include "Vertex.h"

namespace Engine
{
    enum class SpriteSort {
        TEXTURE,
        BACK_TO_FRONT,
        FRONT_TO_BACK
    };

    class Sprite {
    public:
        Sprite(const glm::vec4& destRect, const glm::vec4& uvRect, GLuint Texture, 
               float Depth, const Color& color);
        Sprite(const glm::vec4& destRect, const glm::vec4& uvRect, GLuint Texture,
               float Depth, const Color& color, float angle);

        GLuint texture;
        float depth;

        Vertex topLeft;
        Vertex bottomLeft;
        Vertex topRight;
        Vertex bottomRight;

    private:
        glm::vec2 rotatePoint(const glm::vec2& position, float angle);
    };

    struct RenderBatch {
        RenderBatch(GLuint Offset, GLuint NumVertices, GLuint Texture) :
            offset(Offset),
            numVertices(NumVertices),
            texture(Texture) { }

        GLuint offset;
        GLuint numVertices;
        GLuint texture;
    };

    class SpriteBatch {
    public:
        bool init();
        void dispose();

        void begin(SpriteSort spriteSort = SpriteSort::TEXTURE);
        void end();

        /* No angle specified */
        void draw(const glm::vec4& destRect, const glm::vec4& uvRect, GLuint texture,
                  const Color& color, float depth = 0);
        /* Draw with angle */
        void draw(const glm::vec4& destRect, const glm::vec4& uvRect, GLuint texture,
                  const Color& color, float depth, float angle);
        /* Draw with angle, calculated with dir vector */
        void draw(const glm::vec4& destRect, const glm::vec4& uvRect, GLuint texture,
                  const Color& color, const glm::vec2& dir, float depth = 0);

    private:
        bool initVAO();
        void sortSprites();
        bool createRenderBatches();
        void render();

        GLuint _vbo = 0; /* Vertex Buffer Object */
        GLuint _vao = 0; /* Vertex Array Object */

        SpriteSort _spriteSort = SpriteSort::TEXTURE;

        std::vector<RenderBatch> _renderBatches;
        std::vector<Sprite*>     _spritePointers; /* Only used for optimized sprite sorting */
        std::vector<Sprite>      _sprites;
    };
}

#endif