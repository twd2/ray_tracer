#ifndef _LIGHT_H_
#define _LIGHT_H_

#include "vector3d.hpp"
#include "world.h"

class world;

class light_info
{
public:
    vector3df lightness, direction;

    light_info(const vector3df &lightness, const vector3df &direction)
        : lightness(lightness), direction(direction)
    {

    }

    static light_info dark;
};

class light
{
public:
    world &w;

    light(world &w)
        : w(w)
    {

    }

    virtual light_info illuminate(const vector3df &p) const
    {
        return light_info::dark;
    }
};

#endif // _LIGHT_H_