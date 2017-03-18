#include <cstddef>
#include <cstdio>

#include "plane.h"

#include "object.h"
#include "ray.h"
#include "vector3d.hpp"

intersect_result plane::intersect(const ray &r) const
{
    double divisor = n.dot(r.direction);
    if (divisor <= eps && divisor >= -eps)
    {
        return intersect_result::failed;
    }

    double t = -(D + n.dot(r.origin)) / divisor;

    if (t <= eps)
    {
        return intersect_result::failed;
    }

    return intersect_result(r.origin + r.direction * t, n, t);
}