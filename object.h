#ifndef _OBJECT_H_
#define _OBJECT_H_

#include <vector>

#include "ray.h"
#include "vector3d.hpp"

class intersect_result
{
public:
    bool succeeded = false;
    vector3df p; // point
    vector3df n; // normal vector
    double distance;

    explicit intersect_result(bool succeeded) // Failed.
        : succeeded(succeeded)
    {
        
    }

    intersect_result(const vector3df &p, const vector3df &n, double distance)
        : succeeded(true), p(p), n(n), distance(distance)
    {
        
    }

    static const intersect_result failed;
};

class object
{
public:
    vector3df diffuse = vector3df(0.0, 1.0, 5.0);
    vector3df ambient, specular = vector3df(0.5, 0.5, 0.5);
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

    virtual ~object()
    {

    }

    static object dummy;
};

#endif // _OBJECT_H_