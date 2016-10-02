#include "SpriteBatch.h"

#include <algorithm>
#include <iostream>
#include "../ErrorHandler.h"

namespace Engine
{
    Sprite::Sprite(const glm::vec4& destRect, const glm::vec4& uvRect, GLuint Texture, float Depth,
                   const Color& color) :
        texture(Texture), depth(Depth)
    {
        topLeft.color = color;
        topLeft.setPosition(destRect.x, destRect.y + destRect.w);
        topLeft.setUV(uvRect.x, uvRect.y + uvRect.w);

        bottomLeft.color = color;
        bottomLeft.setPosition(destRect.x, destRect.y);
        bottomLeft.setUV(uvRect.x, uvRect.y);

        bottomRight.color = color;
        bottomRight.setPosition(destRect.x + destRect.z, destRect.y);
        bottomRight.setUV(uvRect.x + uvRect.z, uvRect.y);

        topRight.color = color;
        topRight.setPosition(destRect.x + destRect.z, destRect.y + destRect.w);
        topRight.setUV(uvRect.x + uvRect.z, uvRect.y + uvRect.w);
    }

    Sprite::Sprite(const glm::vec4 & destRect, const glm::vec4 & uvRect, GLuint Texture, float Depth,
                   const Color & color, float angle) :
        texture(Texture), depth(Depth)
    {
        glm::vec2 halfDimensions(destRect.z / 2.0f, destRect.w / 2.0f);

        glm::vec2 tl(-halfDimensions.x, halfDimensions.y);
        glm::vec2 bl(-halfDimensions.x, -halfDimensions.y);
        glm::vec2 br(halfDimensions.x, -halfDimensions.y);
        glm::vec2 tr(halfDimensions.x, halfDimensions.y);

        tl = rotatePoint(tl, angle) + halfDimensions;
        bl = rotatePoint(bl, angle) + halfDimensions;
        br = rotatePoint(br, angle) + halfDimensions;
        tr = rotatePoint(tr, angle) + halfDimensions;

        topLeft.color = color;
        topLeft.setPosition(destRect.x + tl.x, destRect.y + tl.y);
        topLeft.setUV(uvRect.x, uvRect.y + uvRect.w);

        bottomLeft.color = color;
        bottomLeft.setPosition(destRect.x + bl.x, destRect.y + bl.y);
        bottomLeft.setUV(uvRect.x, uvRect.y);

        bottomRight.color = color;
        bottomRight.setPosition(destRect.x + br.x, destRect.y + br.y);
        bottomRight.setUV(uvRect.x + uvRect.z, uvRect.y);

        topRight.color = color;
        topRight.setPosition(destRect.x + tr.x, destRect.y + tr.y);
        topRight.setUV(uvRect.x + uvRect.z, uvRect.y + uvRect.w);
    }

    glm::vec2 Sprite::rotatePoint(const glm::vec2& position, float angle)
    {
        glm::vec2 point;
        point.x = position.x * cos(angle) - position.y * sin(angle);
        point.y = position.x * sin(angle) + position.y * cos(angle);
        return point;
    }

    bool SpriteBatch::init()
    {
        if (initVAO() == false) {
            std::cerr << "ERROR: intiVAO()" << std::endl;
            return false;
        }

        return true;
    }

    void SpriteBatch::dispose()
    {
        if (glIsVertexArray(_vao) == GL_TRUE) {
            glDeleteVertexArrays(1, &_vao);
            _vao = 0;
        }
        
        if (glIsBuffer(_vbo) == GL_TRUE) {
            glDeleteBuffers(1, &_vbo);
            _vbo = 0;
        }
        _renderBatches.clear();
        _spritePointers.clear();
        _sprites.clear();
    }

    /* Default paramter for sortType is SpriteSortType::TEXTURE */
    void SpriteBatch::begin(SpriteSort spriteSort)
    {
        _spriteSort = spriteSort;
        _renderBatches.clear();
        _sprites.clear();
        _spritePointers.clear();
    }

    void SpriteBatch::end()
    {   
        _spritePointers.resize(_sprites.size());
        for (size_t i = 0; i < _sprites.size(); i++)
            _spritePointers[i] = &_sprites[i];
        sortSprites();

        createRenderBatches();
        render();
    }

    /* Default parameter for depth is 0 */
    void SpriteBatch::draw(const glm::vec4& destRect, const glm::vec4& uvRect, GLuint texture,
                           const Color& color, float depth)
    {
        _sprites.emplace_back(destRect, uvRect, texture, depth, color);
    }

    void SpriteBatch::draw(const glm::vec4& destRect, const glm::vec4& uvRect, GLuint texture,
                           const Color& color, float depth, float angle)
    {
        _sprites.emplace_back(destRect, uvRect, texture, depth, color, angle);
    }

    /* Default parameter for depth is 0 */
    void SpriteBatch::draw(const glm::vec4& destRect, const glm::vec4& uvRect, GLuint texture,
                           const Color& color, const glm::vec2& direction, float depth)
    {
        float angle = acos(glm::dot(glm::vec2(1.0f, 0.0f), direction));
        if (direction.y < 0.0f)
            angle *= -1;

        _sprites.emplace_back(destRect, uvRect, texture, depth, color, angle);
    }

    bool SpriteBatch::initVAO()
    {
        /* Generate and bind VAO */
        if (glIsVertexArray(_vao) == GL_FALSE)
            glGenVertexArrays(1, &_vao);
        glBindVertexArray(_vao);

        /* Generate and bind VBO */
        if (glIsBuffer(_vbo) == GL_FALSE)
            glGenBuffers(1, &_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);    
        
        /* Position attribute pointer */
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));

        /* Color attribute pointer */
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (void*)offsetof(Vertex, color));

        /* UV attribute pointer */
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));

        /* Unbind VAO */
        glBindVertexArray(0);

        /* If everything went fine, return true */
        return Engine::errorCheckOpenGL();
    }

    void SpriteBatch::sortSprites()
    {
        switch (_spriteSort) {
        case SpriteSort::TEXTURE:
            std::stable_sort(_spritePointers.begin(), _spritePointers.end(),
                [](Sprite* a, Sprite* b) { return (a->texture < b->texture); });
            break;
        case SpriteSort::BACK_TO_FRONT:
            std::stable_sort(_spritePointers.begin(), _spritePointers.end(),
                [](Sprite* a, Sprite* b) { return (a->depth > b->depth); });
            break;
        case SpriteSort::FRONT_TO_BACK:
            std::stable_sort(_spritePointers.begin(), _spritePointers.end(),
                [](Sprite* a, Sprite* b) { return (a->depth < b->depth); });
            break;
        }
    }

    bool SpriteBatch::createRenderBatches()
    {
        std::vector<Vertex> vertices; /* Vertices that will be sent to GPU */
        vertices.resize(_spritePointers.size() * 6);

        if (_spritePointers.empty()) {
            /* If the map is empty, this will get printed, so its commented now */
            //std::cerr << "ERROR: failed to resize vector of vertices initVAO!" << std::endl;
            return false;
        }

        int vertex = 0;
        int offset = 0;
        /* Add batches */
        for (size_t i = 0; i < _spritePointers.size(); i++, offset += 6) {
            if (i == 0 || _spritePointers[i]->texture != _spritePointers[i - 1]->texture)
                _renderBatches.emplace_back(offset, 6, _spritePointers[i]->texture);
            else
                _renderBatches.back().numVertices += 6;

            vertices[vertex++] = _spritePointers[i]->topLeft;
            vertices[vertex++] = _spritePointers[i]->bottomLeft;
            vertices[vertex++] = _spritePointers[i]->bottomRight;
            vertices[vertex++] = _spritePointers[i]->bottomRight;
            vertices[vertex++] = _spritePointers[i]->topRight;
            vertices[vertex++] = _spritePointers[i]->topLeft;
        }

        /* Fill vertex buffer */
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(Vertex), vertices.data());

        /* Unbind buffer */
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        return true;
    }

    void SpriteBatch::render()
    {
        glBindVertexArray(_vao);
        for (auto& batch : _renderBatches) {
            glBindTexture(GL_TEXTURE_2D, batch.texture);
            glDrawArrays(GL_TRIANGLES, batch.offset, batch.numVertices);
        }

        glBindVertexArray(0);
    }
}
