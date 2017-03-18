#ifndef _PARALLEL_LIGHT_H_
#define _PARALLEL_LIGHT_H_

#include "vector3d.hpp"
#include "light.h"

class parallel_light
    : public light
{
public:
    vector3df direction, color;

    parallel_light(world &w, const vector3df &direction, const vector3df &color)
        : light(w), direction(direction), color(color)
    {

    }

    light_info illuminate(const vector3df &p) const override;
};

#endif // _PARALLEL_LIGHT_H_