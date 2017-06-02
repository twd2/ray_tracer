#ifndef _POINT_LIGHT_H_
#define _POINT_LIGHT_H_

#include "vector3d.hpp"
#include "light.h"

class point_light
    : public light
{
public:
    vector3df location, color;

    point_light(world &w, const vector3df &location, const vector3df &color)
        : light(w), location(location), color(color)
    {

    }

    light_info illuminate(const vector3df &p) const override;

    ray emit(std::default_random_engine &engine) const override;

    vector3df flux() const override
    {
        return color;
    }
};

#endif // _POINT_LIGHT_H_