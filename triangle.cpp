#include <cstddef>
#include <cstdio>

#include "triangle.h"

#include "object.h"
#include "ray.h"
#include "vector3d.hpp"

intersect_result triangle::intersect(const ray &r) const
{
    double divisor = E1xE2.dot(r.direction);
    if (divisor <= eps && divisor >= -eps)
    {
        return intersect_result::failed;
    }
    double divisor_inv = 1.0 / divisor;

    vector3df S = a - r.origin;
    double t = E1xE2.dot(S) * divisor_inv;
    if (t <= eps)
    {
        return intersect_result::failed;
    }
    vector3df DxS = r.direction.cross(S);
    double beta = DxS.dot(E2) * divisor_inv;
    if (beta <= eps || beta > 1.0)
    {
        return intersect_result::failed;
    }

    double gamma = DxS.dot(-E1) * divisor_inv;

    if (gamma <= eps || (beta + gamma) > 1.0)
    {
        return intersect_result::failed;
    }

    return intersect_result(r.origin + r.direction * t, n, t,
                            1.0 - (beta + gamma), beta);
}