#include <cstddef>
#include <cstdio>

#include "triangle.h"

#include "object.h"
#include "ray.h"
#include "vector3d.hpp"

intersect_result triangle::intersect(const ray &r) const
{
    intersect_result ir = p.intersect(r);
    // TODO: test if inside.
    return ir;
}