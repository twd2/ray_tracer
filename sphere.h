#ifndef _SPHERE_H_
#define _SPHERE_H_

#include "object.h"
#include "ray.h"
#include "vector3d.hpp"

class sphere
    : public object
{
public:
    const vector3df c;
    const double r, r2;

    sphere(const vector3df &c, double r)
        : object(), c(c), r(r), r2(r * r)
    {

    }

    intersect_result intersect(const ray &r) override;
};


#endif // _SPHERE_H_