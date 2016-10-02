#include "TextureLoader.h"

#include "IOManager.h"
#include "ImageDecoder.h"
#include "../ErrorHandler.h"

namespace Engine
{
    Texture TextureLoader::loadPNG(const std::string& filePath)
    {
        unsigned long width, height;
        Texture texture = {};

        /* Input data for decodePNG() */
        std::vector<unsigned char> in;
        /* Output data from decodePNG(), used as pixel data for our textures */
        std::vector<unsigned char> out;

        if (IOManager::readFileToBuffer(filePath, in) == false)
            applicationError("Failed to load PNG file " + filePath + " to input buffer!");

        /* Decode image */
        int errorCode = decodePNG(out, width, height, &(in[0]), in.size());
        if (errorCode != 0)
            applicationError("decodePNG failed with error code: " + std::to_string(errorCode));

        /* Generate a new texture name */
        glGenTextures(1, &(texture.id));

        /* Bind a named texture */
        glBindTexture(GL_TEXTURE_2D, texture.id);

        /* Define a texture image */
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &(out[0]));

        /* Texture paramteres */
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

        /* Generate mipmap for the target of the active texture */
        glGenerateMipmap(GL_TEXTURE_2D);

        /* Unbind the texture */
        glBindTexture(GL_TEXTURE_2D, 0);

        texture.width    = width;
        texture.height   = height;
        texture.filePath = filePath;
        return texture;
    }
}