#ifndef _RAY_H_
#define _RAY_H_

#include "vector3d.hpp"

class ray
{
public:
    vector3df origin, direction;
    double refractive_index; // origin refractive index

    ray(const vector3df &origin, const vector3df &direction, double refractive_index = 1.0)
        : origin(origin), direction(direction), refractive_index(refractive_index)
    {

    }
};

#endif // _RAY_H_