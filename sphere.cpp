#include <cstddef>
#include <cstdio>

#include "sphere.h"

#include "object.h"
#include "ray.h"
#include "vector3d.hpp"

intersect_result sphere::intersect(const ray &r)
{
    vector3df l = c - r.location;
    double tp = l.dot(r.direction);
    double d2 = l.length2() - tp * tp;
    if (d2 > r2)
    {
        return intersect_result::failed;
    }
    double delta_t = sqrt(r2 - d2);
    double t;
    // TODO: l.length2() == r2 ?
    if (l.length2() >= r2) // outside
    {
        t = tp - delta_t;
    }
    else // inside
    {
        t = tp + delta_t;
    }

    if (t < 0.0)
    {
        return intersect_result::failed;
    }

    vector3df p = r.location + r.direction * t;
    return intersect_result(p, (p - c).normalize(), t);
}