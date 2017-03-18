#ifndef _TRIANGLE_H_
#define _TRIANGLE_H_

#include "object.h"
#include "ray.h"
#include "vector3d.hpp"
#include "plane.h"

class triangle
    : public object
{
public:
    const vector3df a, b, c;
    const vector3df E1, E2, E1xE2, n;

    triangle(const vector3df &a, const vector3df &b, const vector3df &c)
        : object(), a(a), b(b), c(c), E1(a - b), E2(a - c),
        E1xE2(E1.cross(E2)), n(E1xE2.normalize())
    {

    }

    intersect_result intersect(const ray &r) const override;
};

#endif // _TRIANGLE_H_