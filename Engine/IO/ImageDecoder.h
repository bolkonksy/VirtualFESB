#ifndef _IMAGEDECODER_H
#define _IMAGEDECODER_H

#include <vector>

namespace Engine
{
    /* picoPNG function used for decoding PNG image format */
    extern int decodePNG(std::vector<unsigned char>& out_image, unsigned long& image_width,
                         unsigned long& image_height, const unsigned char* in_png, size_t in_size,
                         bool convert_to_rgba32 = true);
}

#endif