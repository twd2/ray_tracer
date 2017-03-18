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
    const plane p;

    triangle(const vector3df &a, const vector3df &b, const vector3df &c)
        : object(), a(a), b(b), c(c), p(a, (b - a).cross(c - a).normalize())
    {

    }

    intersect_result intersect(const ray &r) const override;
};

#endif // _TRIANGLE_H_