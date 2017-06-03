#include "imagef.h"

imagef::imagef(size_t width, size_t height)
    : width(width), height(height), raw(width * height)
{
    for (auto &c : raw)
    {
        c = vector3df::zero;
    }
}

image imagef::to_image() const
{
    image img(width, height);
    for (std::size_t y = 0; y < height; ++y)
    {
        for (std::size_t x = 0; x < width; ++x)
        {
            const vector3df &c = (*this)(x, y);
            img.set_color(x, y, color_t(c.x * 255, c.y * 255, c.z * 255));
        }
    }
    return img;
}
