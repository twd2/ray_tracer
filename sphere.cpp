#include <cstddef>
#include <cstdio>

#include "sphere.h"

#include "object.h"
#include "ray.h"
#include "vector3d.hpp"

intersect_result sphere::intersect(const ray &r) const
{
    vector3df l = c - r.origin;
    double tp = l.dot(r.direction);
    double d2 = l.length2() - tp * tp;
    if (d2 >= r2)
    {
        return intersect_result::failed;
    }
    double delta_t = sqrt(r2 - d2);
    double t;

    if (l.length2() > r2 + eps) // outside
    {
        t = tp - delta_t;
    }
    else if (l.length2() < r2 - eps) // inside
    {
        t = tp + delta_t;
    }
    else // l.length2() == r2
    {
        vector3df n = -l;
        double n_dot_direction = n.dot(r.direction);
        if (n_dot_direction > eps) // to outside
        {
            t = tp - delta_t;
        }
        else if (n_dot_direction < -eps) // to inside
        {
            t = tp + delta_t;
        }
        else
        {
            return intersect_result::failed;
        }
    }

    if (t <= eps)
    {
        return intersect_result::failed;
    }

    vector3df p = r.origin + r.direction * t;
    intersect_result ir(p, (p - c) / this->r, t);
    ir.n = _get_normal(ir);
    return ir;
}

std::vector<intersect_result> sphere::intersect_all(const ray &r) const
{
    vector3df r_c = r.origin - c;
    double B = r.direction.dot(r_c), C = r_c.length2() - r2; // t^2 + 2Bt + C = 0
    double delta2 = B * B - C;
    if (delta2 <= eps2)
    {
        return std::vector<intersect_result>();
    }
    double delta = sqrt(delta2);
    double t1 = -B - delta, t2 = -B + delta;
    std::vector<intersect_result> results;
    if (t1 > eps)
    {
        vector3df p = r.origin + r.direction * t1;
        results.push_back(intersect_result(p, (p - c) / this->r, t1));
    }
    if (t2 > eps)
    {
        vector3df p = r.origin + r.direction * t2;
        results.push_back(intersect_result(p, (p - c) / this->r, t2));
    }
    return results;
}

vector3df sphere::_get_normal(const intersect_result &ir) const
{
    if (!bump_texture)
    {
        return ir.n;
    }
    else
    {
        // bump mapping

        // x = r * sin(theta) * cos(phi)
        // z = r * sin(theta) * sin(phi)
        // y = r * cos(theta)
        // dx/dtheta = r * cos(theta) * cos(phi) = y * cos(phi)
        // dz/dtheta = r * cos(theta) * sin(phi) = y * sin(phi)
        // dy/dtheta = r * -sin(theta)
        // dx/dphi = r * sin(theta) * -sin(phi) = -z
        // dz/dphi = r * sin(theta) * cos(phi) = x
        // dy/dphi = 0

        vector3df p = ir.p - c;

        vector3df uv = _texture_uv(ir);
        double phi = uv.x * 2 * M_PI,
               theta = M_PI - uv.y * M_PI;
        vector3df pu = vector3df(-p.z, 0.0, p.x),
                  pv = vector3df(p.y * cos(phi), r * -sin(theta), p.y * sin(phi));
        if (pu.length2() < eps2)
        {
            return ir.n;
        }

        constexpr double delta = 0.01;
        double f = _get_bump_texture(uv);
        double fu = (_get_bump_texture(uv + vector3df(delta, 0.0, 0.0)) -
                     _get_bump_texture(uv - vector3df(delta, 0.0, 0.0))) / 
                    (2 * delta * 2 * M_PI),
               fv = (_get_bump_texture(uv + vector3df(0.0, delta, 0.0)) -
                     _get_bump_texture(uv - vector3df(0.0, delta, 0.0))) /
                    (2 * delta * M_PI);

        return (pu + ir.n * fu).cross(pv + ir.n * fv).normalize();
    }
}

double sphere::_get_bump_texture(const vector3df &uv) const
{
    std::size_t x = (std::size_t)(bump_texture->width * uv.x),
                y = (std::size_t)(bump_texture->height * uv.y);
    // TODO: configurable
    // Roll back
    x %= bump_texture->width;
    y %= bump_texture->height;

    y = bump_texture->height - 1 - y;
    color_t c = (*bump_texture)(x, y);
    return (c.r / 255.0 - 0.5) * 2 * 0.2;
}
