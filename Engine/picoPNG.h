#ifndef _PICOPNG_H
#define _PICOPNG_H

#include <vector>

namespace Engine
{
    extern int decodePNG(std::vector<unsigned char>& out_image, unsigned long& image_width,
                         unsigned long& image_height, const unsigned char* in_png, size_t in_size,
                         bool convert_to_rgba32 = true);
}

#endif