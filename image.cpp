#include "image.h"

color_t color_t::white(255, 255, 255, 255);
color_t color_t::black(0, 0, 0, 255);

image::image(size_t width, size_t height, size_t bit_depth)
    : width(width), height(height), bit_depth(bit_depth), row_size(width * bit_depth / 8)
{
    raw = std::make_shared<std::vector<unsigned char> >(row_size * height);
    for (auto &c : *raw)
    {
        c = 0;
    }
}