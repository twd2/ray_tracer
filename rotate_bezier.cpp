#include <cstddef>
#include <cstdio>

#include "rotate_bezier.h"

#include "object.h"
#include "ray.h"
#include "vector3d.hpp"

intersect_result rotate_bezier::intersect(const ray &r) const
{
    ray rel_r(r.origin - position, r.direction);

    // find initial values using mesh
    intersect_result mo_ir = _mo.intersect(rel_r);

    if (!mo_ir.succeeded)
    {
        return mo_ir;
    }

    vector3df uv = _mo.texture_uv(mo_ir);
    uv.x *= 2 * M_PI;

    if (!native)
    {
        mo_ir.p += position;
        mo_ir.u = uv.x;
        mo_ir.v = uv.y;
        return mo_ir;
    }

    intersect_result curve_ir = intersect(rel_r, mo_ir.distance, uv.x, uv.y);
    curve_ir.p += position;

    return curve_ir;
}

intersect_result rotate_bezier::intersect(const ray &r, double t0, double u0, double v0) const
{
    //printf("init: t=%0.10lf, v=%0.10lf, u=%0.10lf, u/pi=%0.10lf\n", t0, v0, u0, u0 / M_PI);
    // u: theta, v: t
    double t = t0, u = u0, v = v0;
    vector3df point, d_dt, d_dtheta;
    for (std::size_t i = 0; i < 15; ++i)
    {
        curve.get(v, u, point, d_dt, d_dtheta);
        vector3df f = r.origin + r.direction * t - point;
        if (f.length2() < eps2)
        {
            if (t <= eps || v < 0.0 || v > 1.0)
            {
                return intersect_result::failed;
            }

            return intersect_result(r.origin + r.direction * t,
                                    d_dtheta.cross(d_dt).normalize(), t,
                                    u, v);
        }

        double D = r.direction.dot(d_dt.cross(d_dtheta));
        t -= d_dt.dot(d_dtheta.cross(f)) / D;
        v -= r.direction.dot(d_dtheta.cross(f)) / D;
        u += r.direction.dot(d_dt.cross(f)) / D;
        if (u < 0.0)
        {
            u = -u + M_PI;
        }
        if (u >= 2 * M_PI)
        {
            u = fmod(u, 2 * M_PI);
        }
        //printf("i%llu: t=%0.10lf, v=%0.10lf, u=%0.10lf, u/pi=%0.10lf\n", i, t, v, u, u / M_PI);
    }
    return intersect_result::failed;
}