#ifndef _TRIANGLE_H_
#define _TRIANGLE_H_

#include "object.h"
#include "ray.h"
#include "vector3d.hpp"
#include "plane.h"

class triangle
    : public object
{
public:
    const vector3df a, b, c;
    const vector3df E1, E2, E1xE2, n;
    vector3df vta, vtb, vtc;

    triangle(const vector3df &a, const vector3df &b, const vector3df &c)
        : object(), a(a), b(b), c(c), E1(a - b), E2(a - c),
        E1xE2(E1.cross(E2)), n(E1xE2.normalize())
    {

    }

    intersect_result intersect(const ray &r) const override;

    void bind_texture(const vector3df &vta, const vector3df &vtb, const vector3df &vtc)
    {
        this->vta = vta;
        this->vtb = vtb;
        this->vtc = vtc;
    }

private:
    vector3df _texture_uv(const intersect_result &ir) const override
    {
        double alpha = ir.u, beta = ir.v, gamma = 1.0 - (ir.u + ir.v);
        return vta * alpha + vtb * beta + vtc * gamma;
    }
};

#endif // _TRIANGLE_H_