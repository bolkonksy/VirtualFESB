#ifndef _CAMERA_H
#define _CAMERA_H

#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>

/* TODO: possible camera rotation */
namespace Engine
{
    /* How to use: 
     *  1. Init camera
     *  Use camera functions freely, camera updates itself automatically if needed.
     */
    class Camera {
    public:
        void init(size_t screenWidth, size_t screenHeight, float scale = 1.0f);

        void offsetPosition(const glm::vec2& offset) { setPosition(_position + offset); }
        void offsetScale(const float offset) { setScale(_zoom + offset); }

        glm::vec2 convertScreenToWorldCoords(const glm::vec2& screenCoords) const;

        bool isBoxInView(const glm::vec2& position, const glm::vec2& dimensions);

        /* Calling setters updates the camera */
        /* Setters */
        void setPosition(const glm::vec2& newPosition) { _position = newPosition; update(); }
        void setScale(float scale)
        {
            scale < 0.001f ? _zoom = 0.001f : _zoom = scale;
            update();
        }

        /* Getters */
        float aspectRatio() const { return (float)_screenWidth / (float)_screenHeight; }
        float scale() const { return _zoom; }
        const glm::mat4& cameraMatrix() const { return _cameraMatrix; }
        glm::vec2 position() const { return _position; } //mybe return a const refernecee

    private:
        size_t    _screenWidth  = 800;
        size_t    _screenHeight = 600;
        float     _zoom         = 1.0f;
        glm::vec2 _position     = glm::vec2(0.0f);
        glm::mat4 _cameraMatrix = glm::mat4(1.0f);
        glm::mat4 _orthoMatrix  = glm::mat4(1.0f);

        void update();
    };
}

#endif