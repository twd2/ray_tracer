#ifndef _CIRCLE_H_
#define _CIRCLE_H_

#include "object.h"
#include "plane.h"
#include "ray.h"
#include "vector3d.hpp"

class circle
    : public plane
{
public:
    const double r, r2;

    circle(const vector3df &p, double r, const vector3df &n)
        : plane(p, n), r(r), r2(r * r)
    {
        
    }

    intersect_result intersect(const ray &r) const override;
};

#endif // _CIRCLE_H_