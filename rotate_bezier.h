#ifndef _ROTATE_BEZIER_H_
#define _ROTATE_BEZIER_H_

#include "object.h"
#include "bezier_curve.h"
#include "mesh_object.h"
#include "ray.h"
#include "vector3d.hpp"

class rotate_bezier
    : public object
{
public:
    const vector3df position;
    bezier_curve curve;
    bool native = false;

private:
    mesh_object _mo;

public:
    rotate_bezier(const vector3df &position, const bezier_curve &bc, double dt, double dtheta)
        : object(), position(position), curve(bc),
          _mo(curve.to_rotate_surface_mesh(dt, dtheta))
    {
        
    }

    intersect_result intersect(const ray &r) const override;
    intersect_result intersect(const ray &r, double t0, double u0, double v0) const;

private:
    vector3df _texture_uv(const intersect_result &ir) const override
    {
        return vector3df(ir.u / (2 * M_PI), ir.v, 0.0);
    }
};

#endif // _ROTATE_BEZIER_H_