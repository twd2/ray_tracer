#include <cstddef>
#include <cstdio>

#include "disc.h"

#include "object.h"
#include "plane.h"
#include "ray.h"
#include "vector3d.hpp"

intersect_result disc::intersect(const ray &r) const
{
    intersect_result ir = plane::intersect(r);
    if (!ir.succeeded)
    {
        return ir;
    }

    if ((ir.p - p).length2() > r2)
    {
        return intersect_result::failed;
    }

    return ir;
}