#ifndef _OBJECT_H_
#define _OBJECT_H_

#include <memory>
#include <vector>

#include "image.h"
#include "ray.h"
#include "vector3d.hpp"

class intersect_result
{
public:
    bool succeeded = false;
    vector3df p; // point
    vector3df n; // normal vector
    double distance;
    std::size_t index = 0; // (optional) index
    double u, v; // (optional) parameters for surface

    explicit intersect_result(bool succeeded) // Failed.
        : succeeded(succeeded)
    {

    }

    intersect_result(const vector3df &p, const vector3df &n, double distance)
        : succeeded(true), p(p), n(n), distance(distance)
    {

    }

    intersect_result(const vector3df &p, const vector3df &n, double distance, double u, double v,
                     std::size_t index = 0)
        : succeeded(true), p(p), n(n), distance(distance), index(index), u(u), v(v)
    {

    }

    static const intersect_result failed;
};

class object
{
public:
    vector3df diffuse = vector3df(0.0, 0.7, 0.4);
    std::shared_ptr<image> texture = nullptr;
    vector3df emission = vector3df::zero, specular = vector3df(0.5, 0.5, 0.5);
    double shininess = 16.0, reflectiveness = 0.0;
    vector3df refractiveness = vector3df::zero;
    double refractive_index = 1.0; // Refractive index.

    // First intersection.
    virtual intersect_result intersect(const ray &r) const
    {
        return intersect_result::failed;
    }

    virtual std::vector<intersect_result> intersect_all(const ray &r) const
    {
        intersect_result ir = intersect(r);
        if (ir.succeeded)
        {
            return std::vector<intersect_result> { ir };
        }
        else
        {
            return std::vector<intersect_result>();
        }
    }

    virtual vector3df get_diffuse(const intersect_result &ir)
    {
        if (!texture)
        {
            return diffuse;
        }
        else
        {
            vector3df uv = _texture_uv(ir);
            uv.y = 1.0 - uv.y;
            std::size_t x = texture->width * uv.x,
                        y = texture->height * uv.y;
            // TODO: configurable
            x %= texture->width;
            y %= texture->height;
            color_t c = (*texture)(x, y);
            return vector3df((double)c.r / 255.0, (double)c.g / 255.0, (double)c.b / 255.0);
        }
    }

    virtual vector3df brdf(const vector3df &p,
                           const vector3df &out_direction, const vector3df &in_direction) const
    {
        // TODO: texture
        return diffuse * 1200000.0; // TODO: BRDF
    }

    virtual ~object()
    {

    }

    static object dummy;

private:
    virtual vector3df _texture_uv(const intersect_result &ir)
    {
        // TODO
        return vector3df::zero;
    }
};

#endif // _OBJECT_H_