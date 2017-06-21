#ifndef _SPHERE_LIGHT_H_
#define _SPHERE_LIGHT_H_

#include "vector3d.hpp"
#include "light.h"
#include "point_light.h"

class sphere_light
    : public point_light
{
public:
    const double r, r2;

    sphere_light(world &w, const vector3df &location, double r, const vector3df &color)
        : point_light(w, location, color), r(r), r2(r * r)
    {

    }

    ray emit(std::default_random_engine &engine) const override;
};

#endif // _SPHERE_LIGHT_H_