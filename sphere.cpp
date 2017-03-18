#include <cstddef>
#include <cstdio>

#include "sphere.h"

#include "object.h"
#include "ray.h"
#include "vector3d.hpp"

intersect_result sphere::intersect(const ray &r) const
{
    vector3df l = c - r.location;
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

    vector3df p = r.location + r.direction * t;
    return intersect_result(p, (p - c) / this->r, t);
}

std::vector<intersect_result> sphere::intersect_all(const ray &r) const
{
    vector3df r_c = r.location - c;
    double B = r.direction.dot(r_c), C = r_c.length2() - r2; // t^2 + 2Bt + C = 0
    double delta2 = B * B - C;
    if (delta2 <= eps)
    {
        return std::vector<intersect_result>();
    }
    double delta = sqrt(delta2);
    double t1 = -B - delta, t2 = -B + delta;
    std::vector<intersect_result> results;
    if (t1 > eps)
    {
        vector3df p = r.location + r.direction * t1;
        results.push_back(intersect_result(p, (p - c) / this->r, t1));
    }
    if (t2 > eps)
    {
        vector3df p = r.location + r.direction * t2;
        results.push_back(intersect_result(p, (p - c) / this->r, t2));
    }
    return results;
}
