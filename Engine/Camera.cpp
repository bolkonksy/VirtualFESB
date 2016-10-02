#include "Camera.h"

namespace Engine
{
    /* Default parameter for scale is 1.0f */
    void Camera::init(size_t screenWidth, size_t screenHeight, float scale)
    {
        _screenWidth = screenWidth;
        _screenHeight = screenHeight;
        setScale(scale);

        /* Init orthoMatrix */
        _orthoMatrix = glm::ortho(0.0f, (float)screenWidth, 0.0f, (float)screenHeight);
        update();
    }

    glm::vec2 Camera::convertScreenToWorldCoords(const glm::vec2& screenCoords) const
    {
        glm::vec2 newScreenCoords = screenCoords;

        /* Invert the Y axis */
        newScreenCoords.y = _screenHeight - screenCoords.y;
        /* Screen center is now at (0, 0) */
        newScreenCoords -= glm::vec2(_screenWidth / 2, _screenHeight / 2);
        /* Zooming the coords */
        newScreenCoords /= _zoom;
        /* And finally, translate with the camera position */
        newScreenCoords += _position;
        /* Order of the above operations shouldn't change */
        return newScreenCoords;
    }

    bool Camera::isBoxInView(const glm::vec2& position, const glm::vec2& dimensions)
    {
        glm::vec2 zoomedScreenDimensions = glm::vec2(_screenWidth, _screenHeight) / _zoom;

        const float MIN_DISTANCE_X = dimensions.x / 2.0f + zoomedScreenDimensions.x / 2.0f;
        const float MIN_DISTANCE_Y = dimensions.y / 2.0f + zoomedScreenDimensions.y / 2.0f;

        glm::vec2 centerPos = position + dimensions / 2.0f;
        glm::vec2 centerCameraPos = _position;

        glm::vec2 distVec = centerCameraPos - centerPos;

        float xDepth = MIN_DISTANCE_X - abs(distVec.x);
        float yDepth = MIN_DISTANCE_Y - abs(distVec.y);

        if (xDepth > 0 && yDepth > 0)
            return true;
        return false;
    }

    void Camera::update()
    {
        /* Camera translation */
        glm::vec3 translate(-_position.x + _screenWidth / 2, -_position.y + _screenHeight / 2, 0.0f);
        _cameraMatrix = glm::translate(_orthoMatrix, translate);

        /* Camera scale */
        glm::vec3 scale(_zoom, _zoom, 0.0f);
        _cameraMatrix = glm::scale(glm::mat4(1.0f), scale) * _cameraMatrix;
    }
}