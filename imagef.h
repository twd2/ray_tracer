#ifndef _IMAGEF_H_
#define _IMAGEF_H_

#include <cstddef>
#include <vector>

#include "vector3d.hpp"
#include "image.h"

class imagef
{
public:
    const size_t width, height;
    std::vector<vector3df> raw;

public:
    imagef(size_t width, size_t height);

    vector3df &operator()(size_t x, size_t y)
    {
        return raw[y * width + x];
    }

    const vector3df &operator()(size_t x, size_t y) const
    {
        return raw[y * width + x];
    }

    image to_image() const;
};

#endif // _IMAGEF_H_