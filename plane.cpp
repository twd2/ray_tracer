#include <cstddef>
#include <cstdio>

#include "plane.h"

#include "object.h"
#include "ray.h"
#include "vector3d.hpp"

intersect_result plane::intersect(const ray &r)
{
    double divisor = n.dot(r.direction);
    if (divisor == 0.0)
    {
        return intersect_result(false);
    }

    double t = -(D + n.dot(r.location)) / divisor;

    if (t < 0)
    {
        return intersect_result(false);
    }

    return intersect_result(r.location + r.direction * t, n);
}