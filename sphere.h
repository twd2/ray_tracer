#ifndef _SPHERE_H_
#define _SPHERE_H_

#include "object.h"
#include "ray.h"
#include "vector3d.hpp"

class sphere
    : public object
{
public:
    const vector3df c;
    const double r, r2; // radius and its squared
    std::shared_ptr<image> bump_texture = nullptr;

    sphere(const vector3df &c, double r)
        : object(), c(c), r(r), r2(r * r)
    {

    }

    intersect_result intersect(const ray &r) const override;
    std::vector<intersect_result> intersect_all(const ray &r) const override;

private:
    vector3df _get_normal(const intersect_result &ir) const;
    double _get_bump_texture(const vector3df &uv) const;

    vector3df _texture_uv(const intersect_result &ir) const override
    {
        // x = r * sin(theta) * cos(phi)
        // z = r * sin(theta) * sin(phi)
        // y = r * cos(theta)
        double phi = atan2(ir.n.z, ir.n.x);
        if (phi < 0.0)
        {
            phi += 2.0 * M_PI;
        }
        double theta = M_PI - acos(ir.n.y); // assert ir.n.length() == 1.0
        return vector3df(phi / (2.0 * M_PI), theta / M_PI, 0.0); // normalize
    }
};

#endif // _SPHERE_H_