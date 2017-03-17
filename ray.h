#ifndef _RAY_H_
#define _RAY_H_

#include "vector3d.hpp"

class ray
{
public:
    vector3df location, direction;

    ray(const vector3df &location, const vector3df &direction)
        : location(location), direction(direction)
    {

    }
};

#endif // _RAY_H_