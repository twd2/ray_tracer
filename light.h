#ifndef _LIGHT_H_
#define _LIGHT_H_

#include <random>

#include "vector3d.hpp"
#include "world.h"
#include "ray.h"

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

    // TODO: template
    virtual ray emit(std::default_random_engine &engine) const = 0;

    virtual vector3df flux() const = 0;
};

#endif // _LIGHT_H_