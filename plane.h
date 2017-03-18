#ifndef _PLANE_H_
#define _PLANE_H_

#include "object.h"
#include "ray.h"
#include "vector3d.hpp"

class plane
    : public object
{
public:
    const vector3df p, n;
    const double D; // Ax + By + Cz + D = 0

    plane(const vector3df &p, const vector3df &n)
        : object(), p(p), n(n), D(-p.dot(n))
    {
        
    }

    intersect_result intersect(const ray &r) const override;
};

#endif // _PLANE_H_